# Filesystem Implementation {#filesystem}

@tableofcontents

ecss-services provides a filesystem interface that needs to be implemented for each platform. This interface allows services 
like Memory Management and File Management to work with files in a platform-independent way.

## Functions to implement

The following sections list and explain the functions that need to be implemented. Place the function implementations inside 
your platform-specific directory (e.g., `src/Platform/x86/Helpers/Filesystem.cpp`).

### Basic File Operations

#### Create and Delete Files

```cpp
etl::optional<FileCreationError> createFile(const Path& path);
etl::optional<FileDeletionError> deleteFile(const Path& path);
```

These functions handle basic file creation and deletion. They should:
- Handle file paths according to your platform's filesystem
- Return appropriate errors if operations fail
- Return `etl::nullopt` on success

#### Read and Write Files

```cpp
etl::expected<void, FileReadError> readFile(const Path& path, FileOffset offset,
                                            FileDataLength fileDataLength, etl::span<uint8_t> buffer);
etl::expected<void, FileWriteError> writeFile(const Path& path, FileOffset offset,
                                              FileDataLength fileDataLength, etl::span<const uint8_t> buffer);
```

These functions handle file content operations. They should:
- Validate that the buffer can hold at least `fileDataLength` bytes
- Handle reading/writing at specific offsets, rejecting ranges beyond the current file size
- Return the appropriate error for invalid operations
- Return an empty (valid) `etl::expected` on success

### Directory Operations

```cpp
etl::optional<DirectoryCreationError> createDirectory(const Path& path);
etl::optional<DirectoryDeletionError> deleteDirectory(const Path& path);
```

These functions manage directories. They should:
- Create/delete directories as needed
- Handle nested paths appropriately
- Return errors for invalid operations
- Return `etl::nullopt` on success

### File Attributes and Status

```cpp
etl::optional<NodeType> getNodeType(const Path& path);
etl::optional<NodeType> getNodeType(const ObjectPath& objectPath);
etl::result<Attributes, FileAttributeError> getFileAttributes(const Path& path);
```

These functions provide metadata about filesystem nodes. They should:
- Determine if a path points to a file or directory
- Provide file size and other attributes
- Handle non-existent paths appropriately

### File Locking

```cpp
etl::expected<void, FilePermissionModificationError> lockFile(const Path& path);
etl::expected<void, FilePermissionModificationError> unlockFile(const Path& path);
FileLockStatus getFileLockStatus(const Path& path);
```

These functions manage file access control. They should:
- Implement a locking mechanism suitable for your platform
- Prevent concurrent access to locked files
- Track lock status for each file

### System Information

```cpp
uint32_t getUnallocatedMemory();
```

This function should:
- Return available filesystem space in bytes
- Be efficient enough for frequent calls
- Handle filesystem-specific space calculations

## Example Implementation

For a reference implementation, see the x86 platform implementation in 
`src/Platform/x86/Filesystem.cpp`.

## Error Handling

The filesystem interface uses several error enums:
- `FileCreationError` for file creation issues
- `FileDeletionError` for file deletion issues
- `FileReadError` for read operation failures
- `FileWriteError` for write operation failures
- `FilePermissionModificationError` for lock/unlock failures
- `DirectoryCreationError` for directory creation issues
- `DirectoryDeletionError` for directory deletion issues
- `FileAttributeError` for metadata access issues

Each operation should carefully consider possible error conditions and return appropriate error codes. 