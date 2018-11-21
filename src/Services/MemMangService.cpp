#include "Services/MemMangService.hpp"
#include <iostream>
#include <cerrno>

// Define the constructors for the classes
MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(this) {
	serviceType = 6;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(
	MemoryManagementService *parent) : mainService(parent) {}


// Function declarations for the raw data memory management subservice
void MemoryManagementService::RawDataMemoryManagement::loadRawData(Message &request) {
	/**
	 * Bare in mind that there is currently no error checking for invalid parameters.
	 * A future version will include error checking and the corresponding error report/notification,
	 * as the manual implies.
	 *
	 * @todo Add error checking and reporting for the parameters
	 * @todo Add failure reporting
	 */
	// Check if we have the correct packet
	assert(request.serviceType == 6);
	assert(request.messageType == 2);

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request

	// Variable declaration
	uint8_t *readData = nullptr; // Pointer to store the data read from the memory
	uint16_t iterationCount = 0; // Get the iteration count
	uint16_t dataLength = 0; // Data length to read (updated for each new iteration)
	uint16_t allocatedLength = 0; // Length allocated for the readData array
	uint64_t startAddress = 0; // Start address for the memory read (updated in each new iteration)

	// Read the packet's values
	iterationCount = request.readUint16();

	if (memoryID == MemoryManagementService::MemoryID::RAM) {
		for (std::size_t j =0; j < iterationCount; j++) {
			startAddress = request.readUint64(); // Start address of the memory
			dataLength = request.readUint16(); // Data length to append

			// Allocate more array space if needed
			if (allocatedLength < dataLength) {
				readData = static_cast<uint8_t *>(realloc(readData, dataLength));
				if (readData == nullptr) {
					// todo: Add error logging and reporting
					free(readData);
					return;
				}
			}

			for (std::size_t i = 0; i < dataLength; i++) {
				readData[i] = request.readByte();
			}
			// todo: Continue only if the checksum passes (when the checksum will be implemented)

			for (std::size_t i = 0; i < dataLength; i++) {
				*(reinterpret_cast<uint8_t *>(startAddress) + i) = readData[i];
			}
		}
	} else if (memoryID == MemoryManagementService::MemoryID::FLASH) {
		// todo: Define FLASH specific access code when we transfer to embedded
	}
}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message &request) {
	// Check if we have the correct packet
	assert(request.serviceType == 6);
	assert(request.messageType == 5);

	// Create the report message object of telemetry message subtype 6
	Message report = mainService->createTM(6);

	// Variable declaration
	uint8_t *readData = nullptr; // Pointer to store the data read from the memory
	uint16_t iterationCount = 0; // Get the iteration count
	uint16_t readLength = 0; // Data length to read (updated for each new iteration)
	uint16_t allocatedLength = 0; // Length allocated for the readData array
	uint64_t startAddress = 0; // Start address for the memory read (updated in each new iteration)

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request
	// todo: Add checks depending on the memory type

	// Read the packet's values
	iterationCount = request.readUint16();

	// Append the data to report message
	report.appendEnum8(memoryID); // Memory ID
	report.appendUint16(iterationCount); // Iteration count

	// Iterate N times, as specified in the command message
	for (std::size_t j = 0; j < iterationCount; j++) {
		startAddress = request.readUint64();
		readLength = request.readUint16();

		// Allocate more array space if needed
		if (allocatedLength < readLength) {
			readData = static_cast<uint8_t *>(realloc(readData, readLength));
			if (readData == nullptr) {
				// todo: Add error logging and reporting
				free(readData);
				return;
			}
		}

		// Read memory data, an octet at a time
		for (std::size_t i = 0; i < readLength; i++) {
			readData[i] = *(reinterpret_cast<uint8_t *>(startAddress) + i);
		}

		// This part is repeated N-times (N = iteration count)
		report.appendUint64(startAddress); // Start address
		report.appendUint16(readLength); // Data read length
		report.appendOctetString(readLength, readData); // Save the read data
	}
	// todo: implement and append the checksum part of the reporting packet

	mainService->storeMessage(report); // Save the report message
	request.resetRead(); // Reset the reading count
	free(readData); // Free the allocated memory
}
