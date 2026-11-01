#include "Platform/x86/Helpers/TestMemory.hpp"

void TestMemory::writeData(MemoryAddress address, MemoryAddress offset, ReadData data) {
    dummyArray[address + offset] = data;
}

ReadData TestMemory::readData(MemoryAddress address, MemoryAddress offset) const {
	return dummyArray[address + offset];
}
