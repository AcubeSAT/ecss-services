//
// Created by kyriakum on 24/2/2025.
//

#include "MemoryAddressProvider.hpp"
#include "TestMemory.hpp"

TestMemory<0, 0> testMemory;

const etl::unordered_map<MemoryId, Memory*, MaxValidMemoryIdsSize> memoryMap = {
	{0, &testMemory}	// Add more entries as needed
};

// constexpr etl::unordered_map<MemoryId, MemoryLimits, MaxMemoryLimitsMapSize> memoryLimitsMap = {};
