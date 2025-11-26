#include "Helpers/Filesystem.hpp"
#include <filesystem>
#include <fstream>
#include <optional>
#include <system_error>

/**
 * Implementation of a filesystem using C++'s standard library for x86 file access.
 *
 * (Un)locking is implemented by setting the _write_ permission on a file.
 */
namespace Filesystem {
	namespace fs = std::filesystem;

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

	etl::expected<DirectoryContentSummary, ReportDirectorySummaryError> reportDirectorySummary(const Path& path) {
		DirectoryContentSummary contentSummary;

		etl::optional<NodeType> nodeType = getNodeType(path);
		if (not nodeType) {
			return etl::unexpected(ReportDirectorySummaryError::DirectoryDoesNotExist);
		}

		if (nodeType.value() == NodeType::File) {
			return etl::unexpected(ReportDirectorySummaryError::PathLeadsToFile);
		}

		const fs::path dir(path.c_str());

		try {
			for (const auto& entry : fs::directory_iterator(dir)) {
				DirectoryContentSummaryNotification notification {
					path.data(),
					NodeType::File,
					entry.path().filename().string().data()
				};
				if (fs::is_directory(entry.status())) {
					notification.nodeType = NodeType::Directory;
				}

				if (!contentSummary.notifications.full()) {
					contentSummary.notifications.push_back(notification);
				} else {
					return etl::unexpected(ReportDirectorySummaryError::MaximumAllowedDirectoryNotificationsReached);
				}
			}
		} catch ([[maybe_unused]] const fs::filesystem_error& e) {
			return etl::unexpected(ReportDirectorySummaryError::UnknownError);
		}

		return contentSummary;
	}

	uint32_t getUnallocatedMemory() {
		// Dummy value for use during testing
		return 42U;
	}
} // namespace Filesystem
