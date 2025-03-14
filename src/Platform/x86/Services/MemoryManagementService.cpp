#include "Services/MemoryManagementService.hpp"
#include "Platform/x86/Helpers/TestMemory.hpp"

TestMemory testMemory{0, std::numeric_limits<std::uintptr_t>::max()};

void MemoryManagementService::initializeMemoryVector() {
	memoryVector.push_back(etl::ref(static_cast<Memory&>(testMemory)));
}
