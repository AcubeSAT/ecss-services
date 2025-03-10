//
// Created by kyriakum on 8/3/2025.
//

#include "Platform/x86/Helpers/TestMemory.hpp"
#include <stdexcept>

bool TestMemory::writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data)
{
	// Assume one byte (?) for now
	*(reinterpret_cast<uint8_t*>(address) + offset) = data;

	// We'll see what return type we'll need
	return true;
}

ReadData TestMemory::readData(std::uintptr_t address, std::uintptr_t offset) const {

	if (!isValidAddress(address + offset)) { // Add validation
        throw std::runtime_error("Invalid address");
    }
	// Assume one byte (?) for now
	return *(reinterpret_cast<uint8_t*>(address) + offset);
}
