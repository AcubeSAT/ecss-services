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

	if (memory == nullptr) {}

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read<StartAddress>();
			const MemoryDataLength dataLength = request.readOctetString(readData.data()); // NOLINT(cppcoreguidelines-init-variables)
			const MemoryManagementChecksum checksum = request.readBits(BitsInMemoryManagementChecksum);

			if (!dataValidator(readData.data(), checksum, dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
				continue;
			}

			if (!memory->isValidAddress(startAddress) ||
			    !memory->isValidAddress(startAddress + dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
				continue;
			}

			for (std::size_t i = 0; i < dataLength; i++) {
			    memory->writeData(startAddress, i, readData[i]);
			}

			for (std::size_t i = 0; i < dataLength; i++) {
			    readData[i] = memory->readData(startAddress, i);
			}

			if (checksum != CRCHelper::calculateCRC(readData.data(), dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
			}
		}
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