#include "Services/MemMangService.hpp"
#include <iostream>

void MemoryManagementService::RawDataMemoryManagement::loadRawData(Message &request) {

}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message &request) {
	Message report = mainService->createTM(6); // Create the report message object of telemetry message subtype 6

	uint8_t iterationCount = 0; // Get the iteration count
	uint8_t *readData = nullptr; // Pointer to store the data read from the memory
	uint16_t readLength = 0; // Data length to read (updated for each new iteration)
	uint32_t startAddress = 0; // Start address for the memory read (updated in each new iteration)

	// Read the packet's values
	iterationCount = request.readUint8();
	startAddress = request.readUint32();
	readLength = request.readUint16();

	readData = (uint8_t *)malloc(static_cast<std::size_t >(readLength + 1));
	for (std::size_t i = 0; i < readLength; i++) {
		readData[i] = *(uint8_t *)(startAddress + i);
	}
	readData[readLength] = '\0';

	report.appendUint8(iterationCount); // Iteration count
	report.appendUint32(startAddress); // Start address
	report.appendUint16(readLength); // Data read length

	//report.appendString(readLength, static_cast<const char *>(readData));
	// todo: add the rest of data fields

	// todo: complete the function and fully specify it
	mainService->storeMessage(report);
}

void MemoryManagementService::RawDataMemoryManagement::dumpedRawDataReport() {

}