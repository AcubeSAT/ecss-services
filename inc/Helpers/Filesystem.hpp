#pragma once

#include "ECSS_Definitions.hpp"
#include "etl/String.hpp"

namespace Filesystem {
	typedef String<ECSSMaxStringSize> Path;

	struct Attributes {
		bool is_locked: 1;
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

	NodeType getNodeType(Path path);

	void lockFile();

	void unlockFile();
}  // namespace Filesystem