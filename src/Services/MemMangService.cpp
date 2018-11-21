#include "Services/MemMangService.hpp"
#include <iostream>

// Define the constructors for the classes
MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(this) {
	serviceType = 6;
	std::cout << "Constructor creation debuffing MemMeang Service" << std::endl;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(
	MemoryManagementService *parent) : mainService(parent) {}


// Function declarations for the raw data memory management subservice
//void MemoryManagementService::RawDataMemoryManagement::loadRawData(Message &request) {
	/**
	 * Bare in mind that there is currently no error checking for invalid parameters.
	 * A future version will include error checking and the corresponding error report/notification,
	 * as the manual implies.
	 *
	 * @todo Add error checking and reporting for the parameters
	 * @todo Add failure reporting
	 */
	/*uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request
	uint8_t iterationCount = 0; // Get the iteration count
	uint16_t dataLength = 0; // Data length to read (updated for each new iteration)
	uint32_t startAddress = 0; // Start address for the memory read (updated in each new iteration)

	// Read the packet's values
	iterationCount = request.readUint8();
	startAddress = request.readUint32();

	if (memoryID == MemoryManagementService::MemoryID::RAM) {
		for (std::size_t i = 0; i < dataLength; i++) {
			//\*(uint64_t *)startAddress = memoryData[i];
		}
	} else if (memoryID == MemoryManagementService::MemoryID::FLASH) {

	}
}*/

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message &request) {
	// Create the report message object of telemetry message subtype 6
	Message report = mainService->createTM(6);

	// Variable declaration
	uint8_t *readData = nullptr; // Pointer to store the data read from the memory
	uint16_t iterationCount = 0; // Get the iteration count
	uint16_t readLength = 0; // Data length to read (updated for each new iteration)
	uint64_t startAddress = 0; // Start address for the memory read (updated in each new iteration)

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request


	// Read the packet's values
	iterationCount = request.readUint16();
	startAddress = request.readUint64();
	readLength = request.readUint16();

	// Append the data to report message
	report.appendEnum8(memoryID); // Memory ID
	report.appendUint16(iterationCount); // Iteration count
	report.appendUint64(startAddress); // Start address
	report.appendUint16(readLength); // Data read length

	readData = static_cast<uint8_t *>( malloc(static_cast<std::size_t >(readLength)) );
	for (std::size_t i = 0; i < readLength; i++) {
		readData[i] = *(reinterpret_cast<uint8_t *>(startAddress) + i);
	}

	report.appendOctetString(readLength, readData);

	mainService->storeMessage(report); // Save the report message
	report.resetRead(); // Reset the reading count
	free(readData); // Free the allocated memory
}
