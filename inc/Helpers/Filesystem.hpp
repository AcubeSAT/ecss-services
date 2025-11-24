#pragma once

#include <etl/vector.h>
#include "ECSS_Definitions.hpp"
#include "etl/String.hpp"
#include "etl/expected.h"
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
		UnknownError = 255
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
	 * Possible errors returned by the filesystem during file locking/unlocking
	 */
	enum class FilePermissionModificationError : uint8_t {
		FileDoesNotExist = 0,
		PathLeadsToDirectory = 1,
		FilePermissionModificationFailed = 2,
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
	 * Possible errors returned by the filesystem during copy operations
	 */
	enum class FileCopyError : uint8_t {
		DestinationFileAlreadyExists = 0,
		ReadFailure = 1,
		WriteFailure = 2,
		CommunicationFailure = 3,
		InsufficientSpace = 4,
		FileCopyOperationNotFound = 5,
		FailedToUpdateOperationState = 6,
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

	enum class FileSystemInitializationError : uint8_t {
		FileSystemListIsFull = 0
	};

	struct PersistedOperationState {
		uint16_t operationId{};
		uint8_t state{};
		uint8_t type{};
		ObjectPath sourcePath;
		ObjectPath targetPath;
		uint32_t bytesTransferred{};
		uint32_t totalBytes{};
		uint32_t startTime{};
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
	 * Retrieves metadata for a file at the given filesystem path.
	 * @param path Path object representing the location of the file.
	 * @return etl::expected containing an Attributes struct on success, or a FileAttributeError describing the failure.
	 */
	etl::expected<Attributes, FileAttributeError> getFileAttributes(const Path& path);

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
	etl::expected<void, FilePermissionModificationError> lockFile(const Path& path);

	/**
	 * Unlocks a file using the filesystem functions.
	 * @param path A String representing the path on the filesystem
	 */
	etl::expected<void, FilePermissionModificationError> unlockFile(const Path& path);

	/**
	 * Copies a file to the requested location using the filesystem functions.
	 * @param operationId The copy file operation ID.
	 */
	void copyFile(uint16_t operationId);

	/**
	 * Moves a file to the requested location using the filesystem functions.
	 * @param operationId The move file operation ID.
	 */
	void moveFile(uint16_t operationId);

	/**
	 * Initializes all filesystems.
	 */
	etl::expected<void, FileSystemInitializationError> initializeFileSystems();

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

	/**
	 * Checks if a file copy operation is allowed between the source full path and the destination full path.
	 * If both the source and the destination paths belong to a remote repository returns false, else true.
	 * @param source The source path passed in as a String.
	 * @param destination The destination path passed in as a String.
	 * @return true if the copy file operation is allowed, false otherwise.
	 */
	bool copyOperationInvolvesLocalPath(const Path& source, const Path& destination);

	/**
	 * Starts a thread that runs copy/move operations, emulating FreeRTOS task execution. Used for testing.
	 */
	void initializeFileCopyTask();

	/**
	 * Stops the thread that runs copy/move operations, emulating FreeRTOS task execution. Used for testing.
	 */
	void shutdownFileCopyTask();

	/**
	 * Called by the Filesystem internally, on initialization, to restore all persisted file copy/move operations.
	 */
	void restorePersistedOperations();

} // namespace Filesystem
