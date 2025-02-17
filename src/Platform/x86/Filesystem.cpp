#include "Helpers/Filesystem.hpp"
#include <filesystem>
#include <fstream>

/**
 * These functions are built on the x86_services target and will never run.
 * To combat undefined function errors, they are defined here.
 * Each function returns the minimum viable option without errors.
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

		fs::permissions(path.data(), newPermissions);

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

		fs::permissions(path.data(), newPermissions);

		return {};
	}

	etl::result<Attributes, FileAttributeError> getFileAttributes(const Path& path) {
		return Attributes{};
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
		return 42U;
	}
} // namespace Filesystem
