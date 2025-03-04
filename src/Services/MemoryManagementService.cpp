//
// Created by kyriakum on 26/2/2025.
//

#include "MemoryManagementService.hpp"
#include "TypeDefinitions.hpp"
#include <inc/Helpers/Memory/IMemoryType.hpp>

MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = MemoryManagementService::ServiceType;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(MemoryManagementService& parent)
	: mainService(parent) {}

void MemoryManagementService::loadRawData(Message& request) {
	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas);
	auto memory = MemoryAddressProvider::memoryMap.at(request.read<MemoryId>());

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	// memory->readData();
}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message& request) {}

void MemoryManagementService::RawDataMemoryManagement::checkRawData(Message& request) {}

void MemoryManagementService::execute(Message& message) {
	switch (message.messageType) {
		case LoadRawMemoryDataAreas:
			loadRawData(message);
		break;
		case DumpRawMemoryData:
			rawDataMemorySubservice.dumpRawData(message);
		break;
		case CheckRawMemoryData:
			rawDataMemorySubservice.checkRawData(message);
		break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}


inline bool MemoryManagementService::memoryIdValidator(MemoryId memId) {
	return MemoryAddressProvider::memoryMap.find(memId) == MemoryAddressProvider::memoryMap.end();
}

inline bool MemoryManagementService::dataValidator(const uint8_t* data, MemoryManagementChecksum checksum,
												MemoryDataLength length) {
	return (checksum == CRCHelper::calculateCRC(data, length));
}