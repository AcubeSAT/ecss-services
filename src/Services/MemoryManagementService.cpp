#include "ECSS_Configuration.hpp"
#ifdef SERVICE_MEMORY

#include <cerrno>
#include <etl/String.hpp>
#include "Services/MemoryManagementService.hpp"

MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this) {
	serviceType = MemoryManagementService::ServiceType;
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
	auto memoryID = static_cast<MemoryManagementService::MemoryID>(request.read<MemoryId>());

	if (!memoryIdValidator(static_cast<MemoryManagementService::MemoryID>(memoryID))) {
		// TODO(#257): Send a failed start of execution
		return;
	}

	etl::array<ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	if (memoryID == MemoryManagementService::MemoryID::FLASH_MEMORY) {
		// TODO(#258): Define FLASH specific access code when we transfer to embedded
	} else {
		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read<StartAddress>();
			const MemoryDataLength dataLength = request.readOctetString(readData.data()); // NOLINT(cppcoreguidelines-init-variables)
			const MemoryManagementChecksum checksum = request.readBits(BitsInMemoryManagementChecksum);

			if (!dataValidator(readData.data(), checksum, dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
				continue;
			}

			if (!addressValidator(memoryID, startAddress) ||
			    !addressValidator(memoryID, startAddress + dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
				continue;
			}

			for (std::size_t i = 0; i < dataLength; i++) {
				*(reinterpret_cast<uint8_t*>(startAddress) + i) = readData[i];
			}

			for (std::size_t i = 0; i < dataLength; i++) {
				readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
			}

			if (checksum != CRCHelper::calculateCRC(readData.data(), dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
			}
		}
	}
}

void MemoryManagementService::RawDataMemoryManagement::dumpRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DumpRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	const MemoryId memoryID = request.read<MemoryId>();

	if (memoryIdValidator(static_cast<MemoryManagementService::MemoryID>(memoryID))) {
		etl::array<ReadData, ECSSMaxStringSize> readData = {};
		uint16_t const iterationCount = request.readUint16();

		report.append<MemoryId>(memoryID);
		report.appendUint16(iterationCount);

		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read<StartAddress>();
			const MemoryDataLength readLength = request.read<MemoryDataLength>();

			if (addressValidator(static_cast<MemoryManagementService::MemoryID>(memoryID), startAddress) &&
			    addressValidator(static_cast<MemoryManagementService::MemoryID>(memoryID), startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
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
	} else {
		// TODO(#257): Send a failed start of execution
	}
}

void MemoryManagementService::RawDataMemoryManagement::checkRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CheckRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	const MemoryId memoryID = request.read<MemoryId>();

	if (memoryIdValidator(static_cast<MemoryManagementService::MemoryID>(memoryID))) {
		etl::array<ReadData, ECSSMaxStringSize> readData = {};
		uint16_t const iterationCount = request.readUint16();

		report.append<MemoryId>(memoryID);
		report.appendUint16(iterationCount);

		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read<StartAddress>();
			const MemoryDataLength readLength = request.read<MemoryDataLength>();

			if (addressValidator(static_cast<MemoryManagementService::MemoryID>(memoryID), startAddress) &&
			    addressValidator(static_cast<MemoryManagementService::MemoryID>(memoryID), startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast<uint8_t*>(startAddress) + i);
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
	} else {
		// TODO(#257): Send a failed start of execution report
	}
}

bool MemoryManagementService::addressValidator(MemoryManagementService::MemoryID memId, uint64_t address) {
	bool validIndicator = true;
	auto iterator = MemoryManagementService::memoryLimitsMap.find(memId);
	if (iterator != MemoryManagementService::memoryLimitsMap.end()) {
		const auto& limits = iterator->second;
		validIndicator = (address >= limits.lowerLim) && (address <= limits.upperLim);
	} else {
		// Default case (unknown MemoryID)
		validIndicator = true;
		// TODO(#259): Implemented so addresses from PC can be read. Remove.
	}
	return validIndicator;
}

inline bool MemoryManagementService::memoryIdValidator(MemoryManagementService::MemoryID memId) {
	return etl::find(MemoryManagementService::validMemoryIds.begin(), MemoryManagementService::validMemoryIds.end(),
	                 memId) != MemoryManagementService::validMemoryIds.end();
}

inline bool MemoryManagementService::dataValidator(const uint8_t* data, MemoryManagementChecksum checksum, MemoryDataLength length) {
	return (checksum == CRCHelper::calculateCRC(data, length));
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

#endif
