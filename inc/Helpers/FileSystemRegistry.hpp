#ifndef ECSS_SERVICES_FILESYSTEMREGISTRY_HPP
#define ECSS_SERVICES_FILESYSTEMREGISTRY_HPP

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

	inline etl::vector<FileSystemDescriptor, MaximumAllowedNumberOfFileSystems> fileSystems{};

	inline void registerFileSystem(const FileSystemDescriptor& fs) {
		fileSystems.push_back(fs);
	}

	inline const FileSystemDescriptor* findFileSystemForPath(const Path& repoPath) {
		for (auto& fs : fileSystems) {
			if (repoPath.find(fs.prefix) == 0) {
				return &fs;
			}
		}
		return nullptr;
	}
}
#endif //ECSS_SERVICES_FILESYSTEMREGISTRY_HPP
