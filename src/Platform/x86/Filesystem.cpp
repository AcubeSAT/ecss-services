#include "Helpers/Filesystem.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "Helpers/FileSystemRegistry.hpp"
#include "Helpers/FilepathValidators.hpp"
#include "ServicePool.hpp"
#include "Services/FileManagementService.hpp"

/**
 * Implementation of a filesystem using C++'s standard library for x86 file access.
 *
 * (Un)locking is implemented by setting the _write_ permission on a file.
 */
namespace Filesystem {
	struct QueuedOperation;
	namespace fs = std::filesystem;

	static constexpr size_t CHUNK_SIZE = 1024;
	static constexpr size_t DELAY_MS = 3;

	static std::queue<QueuedOperation> operationQueue;
	static std::mutex queueMutex;
	static std::condition_variable queueCondition;
	static std::atomic<bool> taskRunning{false};
	static std::thread backgroundTask;

	struct QueuedOperation {
		enum class Type { COPY, MOVE };
		Type type{};
		uint16_t operationId{};
		ObjectPath sourcePath;
		ObjectPath targetPath;
	};

	etl::optional<FileCreationError> createFile(const Path& path) {
		if (getNodeType(path)) {
			return FileCreationError::FileAlreadyExists;
		}

		std::ofstream file(path.data());

		file.flush();
		file.close();

		return etl::nullopt;
	}

	etl::optional<FileDeletionError> deleteFile(const Path& path) {
		etl::optional<NodeType> nodeType = getNodeType(path);
		if (not nodeType) {
			return FileDeletionError::FileDoesNotExist;
		}

		if (nodeType.value() != NodeType::File) {
			return FileDeletionError::PathLeadsToDirectory;
		}

		if (getFileLockStatus(path) == FileLockStatus::Locked) {
			return FileDeletionError::FileIsLocked;
		}

		bool successfulFileDeletion = fs::remove(path.data());

		if (successfulFileDeletion) {
			return etl::nullopt;
		} else {
			return FileDeletionError::UnknownError;
		}
	}

	etl::optional<NodeType> getNodeType(const Path& path) {
		switch (fs::status(path.data()).type()) {
			case fs::file_type::regular:
				return NodeType::File;
			case fs::file_type::directory:
				return NodeType::Directory;
			default:
				return etl::nullopt;
		}
	}

	FileLockStatus getFileLockStatus(const Path& path) {
		fs::perms permissions = fs::status(path.data()).permissions();

		if ((permissions & fs::perms::owner_write) == fs::perms::none) {
			return FileLockStatus::Locked;
		}

		return FileLockStatus::Unlocked;
	}

	etl::expected<void, FilePermissionModificationError> lockFile(const Path& path) {
		etl::optional<NodeType> nodeType = getNodeType(path);
		if (not nodeType) {
			return etl::unexpected(FilePermissionModificationError::FileDoesNotExist);
		}

		if (nodeType.value() != NodeType::File) {
			return etl::unexpected(FilePermissionModificationError::PathLeadsToDirectory);
		}

		fs::perms permissions = fs::status(path.data()).permissions();

		auto newPermissions = permissions & ~fs::perms::owner_write;

		std::error_code ec;
		fs::permissions(path.data(), newPermissions, ec);
		if (ec) {
			return etl::unexpected(FilePermissionModificationError::FilePermissionModificationFailed);
		}

		return {};
	}

	etl::expected<void, FilePermissionModificationError> unlockFile(const Path& path) {
		etl::optional<NodeType> nodeType = getNodeType(path);
		if (not nodeType) {
			return etl::unexpected(FilePermissionModificationError::FileDoesNotExist);
		}

		if (nodeType.value() != NodeType::File) {
			return etl::unexpected(FilePermissionModificationError::PathLeadsToDirectory);
		}

		fs::perms permissions = fs::status(path.data()).permissions();

		auto newPermissions = permissions | fs::perms::owner_write;

		std::error_code ec;
		fs::permissions(path.data(), newPermissions, ec);
		if (ec) {
			return etl::unexpected(FilePermissionModificationError::FilePermissionModificationFailed);
		}

		return {};
	}

	void copyFile(const uint16_t operationId) {
        {
            std::lock_guard lock(queueMutex);
			const auto operation = Services.fileManagement.findFileCopyOperation(operationId);
            operationQueue.push({QueuedOperation::Type::COPY, operationId, operation->getSourcePath(), operation->getTargetPath()});
        }
        queueCondition.notify_one();
	}

    void moveFile(const uint16_t operationId) {
        {
            std::lock_guard lock(queueMutex);
        	const auto operation = Services.fileManagement.findFileCopyOperation(operationId);
            operationQueue.push({QueuedOperation::Type::MOVE, operationId, operation->getSourcePath(), operation->getTargetPath()});
        }
        queueCondition.notify_one();
    }

	etl::result<Attributes, FileAttributeError> getFileAttributes(const Path& path) {
		Attributes attributes;

		auto nodeType = getNodeType(path);
		if (not nodeType) {
			return FileAttributeError::FileDoesNotExist;
		}

		if (nodeType.value() != NodeType::File) {
			return FileAttributeError::PathLeadsToDirectory;
		}

		attributes.sizeInBytes = fs::file_size(path.data());
		attributes.isLocked = getFileLockStatus(path) == FileLockStatus::Locked;

		return attributes;
	}

	etl::optional<DirectoryCreationError> createDirectory(const Path& path) {
		if (getNodeType(path)) {
			return DirectoryCreationError::DirectoryAlreadyExists;
		}

		fs::create_directory(path.data());

		return etl::nullopt;
	}

	etl::optional<DirectoryDeletionError> deleteDirectory(const Path& path) {
		etl::optional<NodeType> nodeType = getNodeType(path);
		if (not nodeType) {
			return DirectoryDeletionError::DirectoryDoesNotExist;
		}

		if (not fs::is_empty(path.data())) {
			return DirectoryDeletionError::DirectoryIsNotEmpty;
		}

		bool successfulFileDeletion = fs::remove(path.data());

		if (successfulFileDeletion) {
			return etl::nullopt;
		} else {
			return DirectoryDeletionError::UnknownError;
		}
	}

	uint32_t getUnallocatedMemory() {
		// Dummy value for use during testing
		return 42U;
	}

	bool copyOperationInvolvesLocalPath(const Path& source, const Path& destination) {
		auto* srcFs = findFileSystemForPath(source);
		auto* dstFs = findFileSystemForPath(destination);

		auto isLocal = [](const FileSystemDescriptor* fs) {
			return fs && fs->kind == FileSystemKind::OnboardLocal;
		};

		return isLocal(srcFs) || isLocal(dstFs);
	}

	void performChunkedCopy(uint16_t operationId, const fs::path& source, const fs::path& destination, bool isMove) {
		Services.fileManagement.setOperationState(operationId, FileManagementService::FileCopyOperation::State::IN_PROGRESS);
		if (fs::exists(destination)) {
			Services.fileManagement.setOperationState(operationId,FileManagementService::FileCopyOperation::State::FAILED);
			Services.fileManagement.notifyOperationFailure(operationId,FileCopyError::DestinationFileAlreadyExists);
			return;
		}
	    std::ifstream sourceStream(source, std::ios::binary);
	    std::ofstream destinationStream(destination, std::ios::binary);
	    if (!sourceStream.is_open()) {
    		Services.fileManagement.setOperationState(operationId, FileManagementService::FileCopyOperation::State::FAILED);
	        Services.fileManagement.notifyOperationFailure(operationId, FileCopyError::ReadFailure);
	        return;
	    }
	    if (!destinationStream.is_open()) {
    		Services.fileManagement.setOperationState(operationId, FileManagementService::FileCopyOperation::State::FAILED);
	        Services.fileManagement.notifyOperationFailure(operationId, FileCopyError::WriteFailure);
	        return;
	    }
	    size_t fileSize = fs::file_size(source);
	    char buffer[CHUNK_SIZE];
	    size_t transferred = 0;
	    while (sourceStream.read(buffer, CHUNK_SIZE) || sourceStream.gcount() > 0) {
	        std::streamsize bytesRead = sourceStream.gcount();
	        while (Services.fileManagement.isOperationSuspended(operationId)) {
	            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
	        }
	        destinationStream.write(buffer, bytesRead);
	        if (!destinationStream) {
	            sourceStream.close();
	            destinationStream.close();
	            fs::remove(destination);
        		Services.fileManagement.setOperationState(operationId, FileManagementService::FileCopyOperation::State::FAILED);
	            Services.fileManagement.notifyOperationFailure(operationId, FileCopyError::WriteFailure);
	            return;
	        }
	        transferred += static_cast<size_t>(bytesRead);
	        Services.fileManagement.updateOperationProgress(operationId, transferred, fileSize);
	        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
	    }
		if (isMove) {
			fs::remove(source);
		}
		sourceStream.close();
	    destinationStream.close();
	    Services.fileManagement.notifyOperationSuccess(operationId);
	}

    void fileCopyTaskFunction() {
        while (taskRunning) {
            std::unique_lock lock(queueMutex);
            queueCondition.wait(lock, [] {
                return !operationQueue.empty() || !taskRunning;
            });
            if (!taskRunning) break;
            while (!operationQueue.empty()) {
                auto [type, operationId, sourcePath, targetPath] = operationQueue.front();
                operationQueue.pop();
                lock.unlock();
                try {
                    if (type == QueuedOperation::Type::COPY) {
                        performChunkedCopy(operationId, sourcePath.data(), targetPath.data(), false);
                    } else {
                    	performChunkedCopy(operationId, sourcePath.data(), targetPath.data(), true);
                    }
                } catch (const std::exception&) {
                	Services.fileManagement.setOperationState(operationId, FileManagementService::FileCopyOperation::State::FAILED);
                	Services.fileManagement.notifyOperationFailure(operationId, FileCopyError::UnknownError);
                }
                lock.lock();
            }
        }
    }

	void initializeFileSystems() {
		using Filesystem::FileSystemDescriptor;
		using Filesystem::FileSystemRole;
		using Filesystem::FileSystemKind;

		registerFileSystem(FileSystemDescriptor{
			.prefix = Path("st23"),
			.role   = FileSystemRole::SourceAndDestination,
			.kind   = FileSystemKind::OnboardLocal
		});

		registerFileSystem(FileSystemDescriptor{
			.prefix = Path("remote"),
			.role   = FileSystemRole::SourceAndDestination,
			.kind   = FileSystemKind::GroundRemote
		});
	}

    void initializeFileCopyTask() {
        taskRunning = true;
        backgroundTask = std::thread(fileCopyTaskFunction);
    }

    void shutdownFileCopyTask() {
        {
            std::lock_guard lock(queueMutex);
            taskRunning = false;
        }
        queueCondition.notify_one();
        if (backgroundTask.joinable()) {
            backgroundTask.join();
        }
    }
} // namespace Filesystem
