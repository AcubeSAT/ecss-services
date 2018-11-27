#include "Services/MemoryManagementService.hpp"
#include <iostream>
#include <cerrno>

// Define the constructors for the classes
MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = 6;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(
	MemoryManagementService &parent) : mainService(parent) {}


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

	// Variable declaration
	uint8_t readData[ECSS_MAX_STRING_SIZE]; // Preallocate the array

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request
	uint16_t iterationCount = request.readUint16(); // Get the iteration count

	// Check for a valid memory ID first
	if (memoryIdValidator(MemoryManagementService::MemoryID(memoryID))) {
		if (memoryID == MemoryManagementService::MemoryID::FLASH) {
			// todo: Define FLASH specific access code when we transfer to embedded
		} else {
			for (std::size_t j = 0; j < iterationCount; j++) {
				uint64_t startAddress = request.readUint64(); // Start address of the memory
				uint16_t dataLength = request.readOctetString(readData); // Data length to load
				// todo: Error logging has to be included, if memory allocation above fails
				// todo: Continue only if the checksum passes (when the checksum will be implemented)

				for (std::size_t i = 0; i < dataLength; i++) {
					*(reinterpret_cast<uint8_t *>(startAddress) + i) = readData[i];
				}
			}
		}
	}
}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message &request) {
	// Check if we have the correct packet
	assert(request.serviceType == 6);
	assert(request.messageType == 5);

	// Create the report message object of telemetry message subtype 6
	Message report = mainService.createTM(6);

	// Variable declaration
	uint8_t readData[ECSS_MAX_STRING_SIZE]; // Preallocate the array

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request
	// todo: Add checks depending on the memory type

	// Check for a valid memory ID first
	if (memoryIdValidator(MemoryManagementService::MemoryID(memoryID))) {
		uint16_t iterationCount = request.readUint16(); // Get the iteration count

		// Append the data to report message
		report.appendEnum8(memoryID); // Memory ID
		report.appendUint16(iterationCount); // Iteration count

		// Iterate N times, as specified in the command message
		for (std::size_t j = 0; j < iterationCount; j++) {
			uint64_t startAddress = request.readUint64(); // Data length to read
			uint16_t readLength = request.readUint16(); // Start address for the memory read

			// Read memory data, an octet at a time, checking for a valid address first
			if (addressValidator(MemoryManagementService::MemoryID(memoryID), startAddress) &&
				addressValidator(MemoryManagementService::MemoryID(memoryID),
					startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast<uint8_t *>(startAddress) + i);
				}

				// This part is repeated N-times (N = iteration count)
				report.appendUint64(startAddress); // Start address
				report.appendOctetString(readLength, readData); // Save the read data
			} else {
				/* Send wrong address failure report */
			}
		}
		// todo: implement and append the checksum part of the reporting packet

		mainService.storeMessage(report); // Save the report message
		request.resetRead(); // Reset the reading count
	} else {
		/* Generate a false start report */
	}
}

void MemoryManagementService::RawDataMemoryManagement::checkRawData(Message &request) {
	// Check if we have the correct packet
	assert(request.serviceType == 6);
	assert(request.messageType == 9);

	// Create the report message object of telemetry message subtype 10
	Message report = mainService.createTM(10);

	// Variable declaration
	uint8_t readData[ECSS_MAX_STRING_SIZE]; // Preallocate the array

	uint8_t memoryID = request.readEnum8(); // Read the memory ID from the request
	// todo: Add checks depending on the memory type

	uint16_t iterationCount = request.readUint16(); // Get the iteration count

	// Append the data to report message
	report.appendEnum8(memoryID); // Memory ID
	report.appendUint16(iterationCount); // Iteration count

	// Iterate N times, as specified in the command message
	for (std::size_t j = 0; j < iterationCount; j++) {
		uint64_t startAddress = request.readUint64(); // Data length to read
		uint16_t readLength = request.readUint16(); // Start address for the memory read

		// Read memory data and save them for checksum calculation
		for (std::size_t i = 0; i < readLength; i++) {
			readData[i] = *(reinterpret_cast<uint8_t *>(startAddress) + i);
		}

		// This part is repeated N-times (N = iteration count)
		report.appendUint64(startAddress); // Start address
		report.appendUint16(readLength); // Save the read data
		// todo: Calculate and append checksum in the report
		//report.appendBits(16, /* checksum bits */);
	}
	// todo: implement and append the checksum part of the reporting packet

	mainService.storeMessage(report); // Save the report message
	request.resetRead(); // Reset the reading count
}


// Private function declaration section
bool MemoryManagementService::RawDataMemoryManagement::addressValidator(
	MemoryManagementService::MemoryID memId, uint64_t address) {
	bool validIndicator = false;

	switch (memId) {
		case MemoryManagementService::MemoryID::DTCMRAM:
			if (address >= DTCMRAM_LOWER_LIM && address <= DTCMRAM_UPPER_LIM) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::ITCMRAM:
			if (address >= ITCMRAM_LOWER_LIM && address <= ITCMRAM_UPPER_LIM) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D1:
			if (address >= RAM_D1_LOWER_LIM && address <= RAM_D1_UPPER_LIM) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D2:
			if (address >= RAM_D2_LOWER_LIM && address <= RAM_D2_UPPER_LIM) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::RAM_D3:
			if (address >= RAM_D3_LOWER_LIM && address <= RAM_D3_UPPER_LIM) {
				validIndicator = true;
			}
			break;
		case MemoryManagementService::MemoryID::FLASH:
			if (address >= FLASH_LOWER_LIM && address <= FLASH_UPPER_LIM) {
				validIndicator = true;
			}
			break;

		default:
			validIndicator = true; // todo: Implemented so addresses from PC can be read. Remove.
			break;
	}

	return validIndicator;
}

inline bool MemoryManagementService::RawDataMemoryManagement::memoryIdValidator(
	MemoryManagementService::MemoryID memId) {
	return (memId == MemoryManagementService::MemoryID::RAM_D1) ||
		(memId == MemoryManagementService::MemoryID::RAM_D2) ||
		(memId == MemoryManagementService::MemoryID::RAM_D3) ||
		(memId == MemoryManagementService::MemoryID::DTCMRAM) ||
		(memId == MemoryManagementService::MemoryID::ITCMRAM) ||
		(memId == MemoryManagementService::MemoryID::FLASH) ||
		(memId == MemoryManagementService::MemoryID::EXTERNAL);
}
