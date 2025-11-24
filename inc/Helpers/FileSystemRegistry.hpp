#pragma once

#include <etl/span.h>

#include "Filesystem.hpp"
namespace Filesystem {

	static constexpr uint8_t MaximumAllowedNumberOfFileSystems = 10;

	enum class FileSystemRole : uint8_t {
		SourceOnly,
		DestinationOnly,
		SourceAndDestination
	 };

	enum class FileSystemKind : uint8_t {
		OnboardLocal,
		OnboardRemote,
		GroundRemote
	 };

	struct FileSystemDescriptor {
		Path prefix;
		FileSystemRole role;
		FileSystemKind kind;
	};

	inline etl::vector<FileSystemDescriptor, MaximumAllowedNumberOfFileSystems> fileSystems{}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

	inline bool registerFileSystem(const FileSystemDescriptor& fs) {
		if (fileSystems.full()) {
			return false;
		}
		fileSystems.push_back(fs);
		return true;
	}

	inline const FileSystemDescriptor* findFileSystemForPath(const Path& repoPath) {
		for (auto& fs : fileSystems) {
			if (repoPath.find(fs.prefix) == 0) {
				return &fs;
			}
		}
		return nullptr;
	}
} // namespace Filesystem
