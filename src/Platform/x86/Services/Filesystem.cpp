#include <fstream>
#include <filesystem>
#include "Helpers/Filesystem.hpp"

namespace fs = std::filesystem;

etl::optional<Filesystem::FileCreationError> Filesystem::createFile(const Filesystem::Path& path){
	etl::optional<NodeType> nodeType = getNodeType(path);
	if (nodeType.has_value()) {
		return FileCreationError::FileAlreadyExists;
	}

	std::ofstream file(path.data());

	file.flush();
	file.close();
}

etl::optional<Filesystem::FileDeletionError> Filesystem::deleteFile(const Filesystem::Path& path) {
	etl::optional<NodeType> nodeType = getNodeType(path);
	if (not nodeType.has_value()) {
		return FileDeletionError::FileDoesNotExist;
	}

	if (nodeType.value() != NodeType::File) {
		return FileDeletionError::PathLeadsToDirectory;
	}

	auto fileLockStatus = getFileLockStatus(path);
	if (fileLockStatus == FileLockStatus::Locked) {
		return FileDeletionError::FileIsLocked;
	}

	bool successfulFileDeletion = fs::remove(path.data());

	if (successfulFileDeletion) {
		return etl::nullopt;
	} else {
		return FileDeletionError::UnknownError;
	}
}

etl::optional<Filesystem::NodeType> Filesystem::getNodeType(const Filesystem::Path& path) {
	namespace fs = std::filesystem;

	fs::file_type type = fs::status(path.data()).type();
	switch (type) {
		case fs::file_type::regular:
			return Filesystem::NodeType::File;
		case fs::file_type::directory:
			return Filesystem::NodeType::Directory;
		default:
			return etl::nullopt;
	}
}

Filesystem::FileLockStatus Filesystem::getFileLockStatus(const Filesystem::Path& path) {
	fs::perms permissions = fs::status(path.data()).permissions();

	if ((permissions & fs::perms::owner_write) == fs::perms::none) {
		return Filesystem::FileLockStatus::Locked;
	}

	return Filesystem::FileLockStatus::Unlocked;
}

void Filesystem::lockFile(const Path& path) {
	fs::perms permissions = fs::status(path.data()).permissions();

	auto newPermissions = permissions & ~fs::perms::owner_write;

	fs::status(path.data()).permissions(newPermissions);
}

void Filesystem::unlockFile(const Path& path) {
	fs::perms permissions = fs::status(path.data()).permissions();

	auto newPermissions = permissions & fs::perms::owner_write;

	fs::status(path.data()).permissions(newPermissions);
}

etl::optional<Filesystem::Attributes> Filesystem::getFileAttributes(const Path& path) {
	Filesystem::Attributes attributes{};

	auto nodeType = getNodeType(path);
	if (not nodeType.has_value()) {
		return etl::nullopt;
	}

	if (nodeType.value() != NodeType::File) {
		return etl::nullopt;
	}

	attributes.sizeInBytes = fs::file_size(path.data());
	attributes.isLocked = getFileLockStatus(path) == Filesystem::FileLockStatus::Locked;

	return attributes;
}
