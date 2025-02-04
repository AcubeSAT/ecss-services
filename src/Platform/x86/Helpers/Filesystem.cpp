#include "Helpers/Filesystem.hpp"
#include <fstream>

#include "Helpers/TypeDefinitions.hpp"

/**
 * These functions are built on the x86_services target and will never run.
 * To combat undefined function errors, they are defined here.
 * Each function returns the minimum viable option without errors.
 */
namespace Filesystem {
	etl::optional <FileCreationError> createFile(const Path& path) {
		return etl::nullopt;
	}

	etl::optional <FileDeletionError> deleteFile(const Path& path) {
		return etl::nullopt;
	}

	etl::optional <NodeType> getNodeType(const Path& path) {
		return etl::nullopt;
	}

	FileLockStatus getFileLockStatus(const Path& path) {
		return FileLockStatus::Unlocked;
	}

	void lockFile(const Path& path) {
	}

	void unlockFile(const Path& path) {
	}

	etl::result <Attributes, FileAttributeError> getFileAttributes(const Path& path) {
		return Attributes{};
	}

	etl::optional <DirectoryCreationError> createDirectory(const Path& path) {
		return etl::nullopt;
	}

	etl::optional <DirectoryDeletionError> deleteDirectory(const Path& path) {
		return etl::nullopt;
	}

	uint32_t getUnallocatedMemory() {
		return 0;
	}


	etl::optional<FileReadError> readFile(const Path& path, Offset offset, FileDataLength length, String<ChunkMaxFileSizeBytes>& buffer) {
		if (buffer.size() != length) {
			return FileReadError::InvalidBufferSize;
		}

		std::ifstream file(path.c_str(), std::ios::binary);
		if (!file.is_open()) {
			return FileReadError::FileNotFound;
		}

		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();

		if (offset + length > fileSize) {
			return FileReadError::InvalidOffset;
		}

		file.seekg(offset, std::ios::beg);
		file.read(buffer.data(), length);

		if (file.fail()) {
			return FileReadError::ReadError;
		}

		return etl::nullopt;
	}

	etl::optional<FileWriteError> writeFile(const Path& path, Offset offset, FileDataLength fileDataLength, String<ChunkMaxFileSizeBytes>& buffer) {
		if (buffer.size() != fileDataLength) {
			return FileWriteError::InvalidBufferSize;
		}

		std::fstream file(path.c_str(), std::ios::binary | std::ios::in | std::ios::out);
		if (!file.is_open()) {
			return FileWriteError::FileNotFound;
		}

		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();

		if (offset > fileSize) {
			return FileWriteError::InvalidOffset;
		}

		file.seekp(offset, std::ios::beg);
		file.write(buffer.data(), fileDataLength);

		if (file.fail()) {
			return FileWriteError::WriteError;
		}

		return etl::nullopt;
	}
} // namespace Filesystem
