# Filesystem Implementation {#filesystem}

@tableofcontents

ecss-services provides a filesystem interface that needs to be implemented for each platform. This interface allows services 
like Memory Management and Storage & Retrieval to work with files in a platform-independent way.

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
etl::optional<FileReadError> readFile(const Path& path, Offset offset, 
                                    FileDataLength length, etl::span<uint8_t> buffer);
etl::optional<FileWriteError> writeFile(const Path& path, Offset offset, 
                                      FileDataLength length, etl::span<uint8_t> buffer);
```

These functions handle file content operations. They should:
- Validate buffer sizes match the length parameter
- Handle reading/writing at specific offsets
- Return appropriate errors for invalid operations
- Return `etl::nullopt` on success

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
void lockFile(const Path& path);
void unlockFile(const Path& path);
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
`src/Platform/x86/Helpers/Filesystem.cpp`.

## Error Handling

The filesystem interface uses several error enums:
- `FileCreationError` for file creation issues
- `FileDeletionError` for file deletion issues
- `FileReadError` for read operation failures
- `FileWriteError` for write operation failures
- `DirectoryCreationError` for directory creation issues
- `DirectoryDeletionError` for directory deletion issues
- `FileAttributeError` for metadata access issues

Each operation should carefully consider possible error conditions and return appropriate error codes. 