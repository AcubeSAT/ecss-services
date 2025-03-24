#include "Helpers/Filesystem.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>
#include <regex>

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

	etl::result<FoundFiles, FileSearchError> findFiles(const Path& path) {
		FoundFiles files;

		const fs::path full_path(path.data());
		const fs::path directory = full_path.parent_path();
		const std::string pattern = full_path.filename().string();
		const std::string regex_pattern = std::regex_replace(pattern, std::regex(R"(\*)"), ".*");
		const std::regex file_regex(regex_pattern);
		if (!exists(directory) || !is_directory(directory)) {
			return FileSearchError::DirectoryDoesNotExist;
		}

		for (const auto& entry : fs::directory_iterator(directory)) {
			if (is_regular_file(entry.path())) {
				std::string full_file_path = entry.path().string();
				if (const std::string filename = entry.path().filename().string(); std::regex_match(filename, file_regex)) {
					files.paths.emplace_back(full_file_path.data());
				}
			}
		}
		return files;
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
} // namespace Filesystem
