#pragma once

#include "ECSS_Definitions.hpp"
#include "etl/String.hpp"
#include "etl/optional.h"

namespace Filesystem {
	using Path = String<ECSSMaxStringSize>;

	struct Attributes {
		size_t sizeInBytes;
		bool isLocked;
	};

	enum class NodeType: uint8_t {
		Directory = 0,
		File = 1
	};

	enum class FileCreationError: uint8_t {
		FileAlreadyExists = 0,
		UnknownError = 1
	};

	enum class FileDeletionError: uint8_t {
		FileDoesNotExist = 0,
		PathLeadsToDirectory = 1,
		FileIsLocked = 2,
		UnknownError = 3
	};

	enum class DirectoryCreationError: uint8_t {
		DirectoryAlreadyExists = 0,
		UnknownError = 1
	};

	enum class DirectoryDeletionError: uint8_t {
		DirectoryDoesNotExist = 0,
		DirectoryIsNotEmpty = 1,
		UnknownError = 2
	};

	enum class FileLockStatus: uint8_t {
		Locked = 0,
		Unlocked = 1
	};

	etl::optional<FileCreationError> createFile(const Path& path);

	etl::optional<FileDeletionError> deleteFile(const Path& path);

	etl::optional<DirectoryCreationError> createDirectory(const Path& path);

	etl::optional<DirectoryDeletionError> deleteDirectory(const Path& path);

	etl::optional<Attributes> getFileAttributes(const Path& path);

	etl::optional<NodeType> getNodeType(const Path& path);

	void lockFile(const Path& path);

	void unlockFile(const Path& path);

	FileLockStatus getFileLockStatus(const Path& path);
}  // namespace Filesystem