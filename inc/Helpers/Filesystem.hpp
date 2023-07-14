#pragma once

#include "ECSS_Definitions.hpp"
#include "etl/String.hpp"
#include "etl/optional.h"

namespace Filesystem {
	typedef String<ECSSMaxStringSize> Path;

	struct Attributes {
		bool is_locked;
	};

	enum NodeType: uint8_t {
		Directory = 0,
		File = 1
	};

	void deleteFile(Path path);

	void createFile(Path path);

	void writeFile(Path path, String<ECSSMaxStringSize> data);

	void readFile(Path path);

	void getFileAttributes(Path path);

	void writeFileAttributes(Path path, Attributes attributes);

	etl::optional<NodeType> getNodeType(Path path);

	void lockFile();

	void unlockFile();
}  // namespace Filesystem