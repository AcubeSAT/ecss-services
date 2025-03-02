//
// Created by kyriakum on 24/2/2025.
//

#include "MemoryAddressProvider.hpp"
#include "TestMemory.cpp"

constexpr TestMemory testMemory;

constexpr etl::unordered_map<MemoryId, MemoryAddressProvider::ReadFunction> memoryMap = {
	{0, [](std::size_t address, MemoryDataLength len){testMemory.readData(address, len); }}	// Add more entries as needed
};

// constexpr etl::unordered_map<MemoryId, MemoryLimits, MaxMemoryLimitsMapSize> memoryLimitsMap = {};
