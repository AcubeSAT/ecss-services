#include "Services/MemoryManagementService.hpp"
#include "Platform/x86/Helpers/TestMemory.hpp"

TestMemory testMemory;

void MemoryManagementService::initializeMemoryVector() {
	memoryVector.push_back(etl::ref(static_cast<Memory&>(testMemory)));
}
