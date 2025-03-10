//
// Created by kyriakum on 24/2/2025.
//

#include "Helpers/Memory/MemoryAddressProvider.hpp"
#include "Platform/x86/Helpers/TestMemory.hpp"

TestMemory testMemory(0, std::numeric_limits<uintptr_t>::max());

const etl::unordered_map<MemoryId, Memory*, MaxValidMemoryIdsSize> MemoryAddressProvider::memoryMap = {
	{0, &testMemory}	// Add more entries as needed
};

// constexpr etl::unordered_map<MemoryId, MemoryLimits, MaxMemoryLimitsMapSize> memoryLimitsMap = {};
