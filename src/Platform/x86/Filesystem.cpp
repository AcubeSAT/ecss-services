#include "Helpers/Filesystem.hpp"

/**
 * These functions are built on the x86_services target and will never run.
 * To combat undefined function errors, they are defined here.
 * Each function returns the minimum viable option without errors.
 */
namespace Filesystem {
	etl::optional<FileCreationError> createFile(const Path& path) {
		return etl::nullopt;
	}

	etl::optional<FileDeletionError> deleteFile(const Path& path) {
		return etl::nullopt;
	}

	etl::optional<NodeType> getNodeType(const Path& path) {
		return etl::nullopt;
	}

	FileLockStatus getFileLockStatus(const Path& path) {
		return FileLockStatus::Unlocked;
	}

	void lockFile(const Path& path) {
	}

	void unlockFile(const Path& path) {
	}

	etl::result<Attributes, FileAttributeError> getFileAttributes(const Path& path) {
		return Attributes{};
	}

	etl::optional<DirectoryCreationError> createDirectory(const Path& path) {
		return etl::nullopt;
	}

	etl::optional<DirectoryDeletionError> deleteDirectory(const Path& path) {
		return etl::nullopt;
	}

	uint32_t getUnallocatedMemory() {
		return 0;
	}
} // namespace Filesystem
