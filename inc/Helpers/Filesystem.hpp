#pragma once

#include <etl/span.h>
#include <etl/string_utilities.h>

#include "ECSS_Definitions.hpp"
#include "TypeDefinitions.hpp"
#include "etl/String.hpp"
#include "etl/optional.h"
#include "etl/result.h"

namespace Filesystem {
	constexpr size_t FullPathSize = ECSSMaxStringSize;
	using Path = String<FullPathSize>;

	/**
	 * ObjectPathSize is half the maximum size, minus one character for the '/' delimiter between the
	 * repository and file paths.
	 */
	constexpr size_t ObjectPathSize = (FullPathSize / 2) - 1;
	using ObjectPath = String<ObjectPathSize>;

	/**
	 * The available metadata for a file
	 */
	struct Attributes {
		size_t sizeInBytes;
		bool isLocked;
	};

	/**
	 * The type of a node in the file system
	 */
	enum class NodeType : uint8_t {
		Directory = 0,
		File = 1
	};

	/**
	 * Possible errors returned by the filesystem during file creation
	 */
	enum class FileCreationError : uint8_t {
		FileAlreadyExists = 0,
		UnknownError = 255,
	};

	/**
	 * Possible errors returned by the filesystem during file deletion
	 */
	enum class FileDeletionError : uint8_t {
		FileDoesNotExist = 0,
		PathLeadsToDirectory = 1,
		FileIsLocked = 2,
		UnknownError = 255
	};

	/**
	 * Possible errors returned by the filesystem during directory creation
	 */
	enum class DirectoryCreationError : uint8_t {
		DirectoryAlreadyExists = 0,
		UnknownError = 255
	};

	/**
	 * Possible errors returned by the filesystem during directory deletion
	 */
	enum class DirectoryDeletionError : uint8_t {
		DirectoryDoesNotExist = 0,
		DirectoryIsNotEmpty = 1,
		UnknownError = 255
	};

	/**
	 * The current file lock status
	 */
	enum class FileLockStatus : uint8_t {
		Unlocked = 0,
		Locked = 1,
	};

	/**
	 * Possible errors returned by the filesystem during a file attribute check
	 */
	enum class FileAttributeError : uint8_t {
		PathLeadsToDirectory = 0,
		FileDoesNotExist = 1
	};

	/**
	 * Possible errors returned by the filesystem during a file read operation
	 */
	enum class FileReadError : uint8_t {
		FileNotFound = 0,
		InvalidBufferSize = 1,
		InvalidOffset = 2,
		ReadError = 3,
		UnknownError = 255
	};

	/**
	 * Possible errors returned by the filesystem during a file write operation
	 */
	enum class FileWriteError : uint8_t {
		FileNotFound = 0,
		InvalidBufferSize = 1,
		InvalidOffset = 2,
		WriteError = 3,
		UnknownError = 255
	};

	/**
	 * Creates a file using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @return Optionally, a file creation error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<FileCreationError> createFile(const Path& path);

	/**
	 * Deletes a file using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @return Optionally, a file deletion error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<FileDeletionError> deleteFile(const Path& path);

	/**
	 * Creates a directory using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @return Optionally, a directory creation error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<DirectoryCreationError> createDirectory(const Path& path);

	/**
	 * Deletes a directory using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @return Optionally, a directory deletion error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<DirectoryDeletionError> deleteDirectory(const Path& path);

	/**
	 * Gets the file metadata
	 * @param path A String representing the path on the filesystem
	 * @return Either an Attributes struct if there were no errors, either a FileAttributeError.
	 */
	etl::result<Attributes, FileAttributeError> getFileAttributes(const Path& path);

	/**
	 * Gets the type of node in the filesystem
	 * @param path A String representing the path on the filesystem
	 * @return A NodeType value, or nothing if the file can't be accessed
	 */
	etl::optional<NodeType> getNodeType(const Path& path);

	/**
	 * An overloaded function providing support for getNodeType on repository objects.
	 * @param objectPath A String representing a path on the filesystem
	 * @return A NodeType value
	 */
	inline etl::optional<NodeType> getNodeType(const ObjectPath& objectPath) {
		const Path path = objectPath.data();
		return getNodeType(path);
	}

	/**
	 * Locks a file using the filesystem functions.
	 * @param path A String representing the path on the filesystem
	 */
	void lockFile(const Path& path);

	/**
	 * Unlocks a file using the filesystem functions.
	 * @param path A String representing the path on the filesystem
	 */
	void unlockFile(const Path& path);

	/**
	 * Creates a file using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @param offSet the starting byte to read from
	 * @param fileDataLength the number of bytes to read on
	 * @param buffer A buffer with the required data to be read from the file. Should be the same size as the offset,
	 * otherwise an error is produced
	 * @return Optionally, a file creation error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<FileReadError> readFile(const Path& path, Offset offSet, FileDataLength fileDataLength, etl::span<uint8_t> buffer);

	/**
	 * Creates a file using platform specific filesystem functions
	 * @param path A String representing the path on the filesystem
	 * @param offSet the starting byte to write from
	 * @param fileDataLength the number of bytes to write on
	 * @param buffer A buffer with the required data to be written in the file. Should be the same size as the offset,
	 * otherwise an error is produced
	 * @return Optionally, a file creation error. If no errors occur, returns etl::nullopt
	 */
	etl::optional<FileWriteError> writeFile(const Path& path, Offset offSet, FileDataLength fileDataLength, etl::span<uint8_t> buffer);

	/**
	 * Gets the current file lock status
	 * @param path A String representing the path on the filesystem
	 * @return The FileLockStatus value
	 */
	FileLockStatus getFileLockStatus(const Path& path);

	/**
	 * Get the Unallocated Memory
	 * @return The unallocated memory in bytes 
	 */
	uint32_t getUnallocatedMemory();

	using ObjectPath = Filesystem::ObjectPath;
	using Path = Filesystem::Path;

	/**
	 * Returns the full filesystem path for an object given the repository path and the file path
	 * @param repositoryPath The repository path
	 * @param filePath The file path
	 * @return The full path, where the repository path and file path are separated by a single '/' (slash)
	 *
	 * @note All leading and trailing slashes are removed from the repositoryPath and filePath objects.
	 */
	inline static Path getFullPath(ObjectPath& repositoryPath, ObjectPath& filePath) {
		etl::trim_from_left(repositoryPath, "/");
		etl::trim_from_right(repositoryPath, "/");

		etl::trim_from_left(filePath, "/");
		etl::trim_from_right(filePath, "/");

		Path fullPath = ("");
		fullPath.append(repositoryPath);
		fullPath.append("/");
		fullPath.append(filePath);
		return fullPath;
	}

} // namespace Filesystem