//
// Created by kyriakum on 3/1/2025.
//
#include "TestMemory.hpp"
#include "TypeDefinitions.hpp"
#include <etl/array.h>

	etl::array<ReadData, ECSSMaxStringSize> TestMemory::readData(const std::size_t address, const MemoryDataLength dataLength) const {}

	bool TestMemory::writeData(const std:size_t address, const MemoryDataLength dataLength, const etl::array<ReadData, const ECSSMaxStringSize>& data) {}

	uint8_t TestMemory::readByte(const std::size_t address) const {}

	bool TestMemory::writeByte(const std::size_t address) {}


