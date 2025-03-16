#include "Platform/x86/Helpers/TestMemory.hpp"

void TestMemory::writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data) {
	*(reinterpret_cast<uint8_t*>(address) + offset) = data;
}

ReadData TestMemory::readData(std::uintptr_t address, std::uintptr_t offset) const {
	return *(reinterpret_cast<uint8_t*>(address) + offset);
}
