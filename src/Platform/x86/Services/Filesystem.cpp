#include <fstream>
#include "Helpers/Filesystem.hpp"

void Filesystem::createFile(Path path){
	std::ofstream file(path.data());

	file << "Hello, world!";

	file.close();
}
