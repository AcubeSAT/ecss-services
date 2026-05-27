#pragma once

#include <etl/optional.h>

#include "Filesystem.hpp"
#include "ECSS_Definitions.hpp"

namespace Filesystem {

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

	inline etl::vector<FileSystemDescriptor, ECSSMaxRegisteredFileSystems> fileSystems{}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

	/**
	 * Registers a filesystem descriptor. Returns false if the registry is full.
	 */
	inline bool registerFileSystem(const FileSystemDescriptor& fs) {
		if (fileSystems.full()) {
			return false;
		}
		fileSystems.push_back(fs);
		return true;
	}

	/**
	 * Finds the first registered filesystem whose prefix matches the start of @p repoPath.
	 * @return The matching descriptor, or etl::nullopt if no registered filesystem covers the path.
	 */
	inline etl::optional<FileSystemDescriptor> findFileSystemForPath(const Path& repoPath) {
		for (const auto& fs : fileSystems) {
			if (repoPath.find(fs.prefix) == 0) {
				return fs;
			}
		}
		return etl::nullopt;
	}
} // namespace Filesystem
