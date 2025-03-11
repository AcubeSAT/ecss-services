#include "Platform/x86/Helpers/TestMemory.hpp"

bool TestMemory::writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data)
{
	// Assume one byte (?) for now
	*(reinterpret_cast<uint8_t*>(address) + offset) = data;

	// We'll see what return type we'll need
	return true;
}

ReadData TestMemory::readData(std::uintptr_t address, std::uintptr_t offset) const {
	// Assume one byte (?) for now
	return *(reinterpret_cast<uint8_t*>(address) + offset);
}
