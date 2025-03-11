#include "Services/MemoryManagementService.hpp"

MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = MemoryManagementService::ServiceType;
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(MemoryManagementService& parent)
	: mainService(parent) {}

void MemoryManagementService::loadRawData(Message& request) {
	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas);

	MemoryId memoryID = request.read<MemoryId>();

	auto *memory = MemoryManagementService::getMemoryFromId(memoryID);

	if(memory == nullptr) {
		// TODO(no-assignee) throw an error (AddressOutOfRange) (This also needs to be implemented in the tests)
		return;
	}

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

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DumpRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::DumpRawMemoryDataReport);

	MemoryId memoryID = request.read<MemoryId>();

	auto *memory = MemoryManagementService::getMemoryFromId(memoryID);

	if(memory == nullptr) {
		// TODO(no-assignee) throw an error (AddressOutOfRange) (This also needs to be implemented in the tests)
		return;
	}

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	report.append<MemoryId>(memoryID);
	report.appendUint16(iterationCount);

	for (std::size_t j = 0; j < iterationCount; j++) {
		const StartAddress startAddress = request.read<StartAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (memory->isValidAddress(startAddress) &&
			memory->isValidAddress(startAddress + readLength)) {
			for (std::size_t i = 0; i < readLength; i++) {
				readData[i] = memory->readData(startAddress, i);
			}

			report.append<StartAddress>(startAddress);
			report.appendOctetString(String<ECSSMaxFixedOctetStringSize>(readData.data(), readLength));
			report.append<CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
		} else {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
		}
	}

	mainService.storeMessage(report);
	request.resetRead();

}

void MemoryManagementService::RawDataMemoryManagement::checkRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CheckRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	const MemoryId memoryID = request.read<MemoryId>();

	auto *memory = MemoryManagementService::getMemoryFromId(memoryID);

	if(memory == nullptr) {
		ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
		return;
	}

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	report.append<MemoryId>(memoryID);
	report.appendUint16(iterationCount);

	for (std::size_t j = 0; j < iterationCount; j++) {
		const StartAddress startAddress = request.read<StartAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (memory->isValidAddress(startAddress) &&
			memory->isValidAddress(startAddress + readLength)) {
			for (std::size_t i = 0; i < readLength; i++) {
				readData[i] = memory->readData(startAddress, i);
			}

			report.append<StartAddress>(startAddress);
			report.append<MemoryDataLength>(readLength);
			report.append<CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
		} else {
		}
	}

	mainService.storeMessage(report);
	request.resetRead();
}

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

Memory* MemoryManagementService::getMemoryFromId(MemoryId memId) {
    auto iter = MemoryAddressProvider::memoryMap.find(memId);
    return (iter != MemoryAddressProvider::memoryMap.end()) ? iter->second : nullptr;
}

inline bool MemoryManagementService::dataValidator(const uint8_t* data, MemoryManagementChecksum checksum,
												MemoryDataLength length) {
	return (checksum == CRCHelper::calculateCRC(data, length));
}