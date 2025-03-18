#include "Services/MemoryManagementService.hpp"

MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = MemoryManagementService::ServiceType;
	initializeMemoryVector();
}

MemoryManagementService::RawDataMemoryManagement::RawDataMemoryManagement(MemoryManagementService& parent)
    : mainService(parent) {}

void MemoryManagementService::loadRawData(Message& request) {
	/**
	 * Bear in mind that there is currently no error checking for invalid parameters.
	 * A future version will include error checking and the corresponding error report/notification,
	 * as the manual implies.
	 *
	 * @todo (#255): Add error checking and reporting for the parameters
	 * @todo (#256): Add failure reporting
	 */

	request.assertTC(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas);

	MemoryId memoryID = request.read<MemoryId>();

	auto memoryOpt = getMemoryFromId(memoryID);

	if (!memoryOpt.has_value()) {
		// @todo(#257): Send a failed start of execution
		return;
	}

	auto& memory = memoryOpt.value().get();

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

		if (!memory.isValidAddress(startAddress) ||
		    !memory.isValidAddress(startAddress + dataLength)) {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			continue;
		}

		for (std::size_t i = 0; i < dataLength; i++) {
			memory.writeData(startAddress, i, readData[i]);
		}

		for (std::size_t i = 0; i < dataLength; i++) {
			readData[i] = memory.readData(startAddress, i);
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

	auto memoryOpt = mainService.getMemoryFromId(memoryID);

	if (!memoryOpt.has_value()) {
		// @todo(#257): Send a failed start of execution
		return;
	}

	const auto& memory = memoryOpt.value().get();

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	report.append<MemoryId>(memoryID);
	report.appendUint16(iterationCount);

	for (std::size_t j = 0; j < iterationCount; j++) {
		const StartAddress startAddress = request.read<StartAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (memory.isValidAddress(startAddress) &&
		    memory.isValidAddress(startAddress + readLength)) {
			for (std::size_t i = 0; i < readLength; i++) {
				readData[i] = memory.readData(startAddress, i);
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

	auto memoryOpt = mainService.getMemoryFromId(memoryID);

	if (!memoryOpt.has_value()) {
		// @todo(#257): Send a failed start of execution
		return;
	}

	auto& memory = memoryOpt.value().get();

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	report.append<MemoryId>(memoryID);
	report.appendUint16(iterationCount);

	for (std::size_t j = 0; j < iterationCount; j++) {
		const StartAddress startAddress = request.read<StartAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (memory.isValidAddress(startAddress) &&
		    memory.isValidAddress(startAddress + readLength)) {
			for (std::size_t i = 0; i < readLength; i++) {
				readData[i] = memory.readData(startAddress, i);
			}

			report.append<StartAddress>(startAddress);
			report.append<MemoryDataLength>(readLength);
			report.append<CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
		} else {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
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