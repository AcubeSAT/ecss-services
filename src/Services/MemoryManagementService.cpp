#include "ECSS_Configuration.hpp"
#include "ServicePool.hpp"
#include "Services/RequestVerificationService.hpp"
#ifdef SERVICE_MEMORY

#include <cerrno>
#include <etl/String.hpp>
#include "Services/MemoryManagementService.hpp"
#include "Helpers/Filesystem.hpp"

using namespace Filesystem;

MemoryManagementService::MemoryManagementService() : rawDataMemorySubservice(*this),
                                                     structuredDataMemoryManagementSubService(*this) {
	serviceType = MemoryManagementService::ServiceType;
}

MemoryManagementService::RawDataMemoryManagementSubService::RawDataMemoryManagementSubService(
	MemoryManagementService& parent)
	: mainService(parent) {
}

MemoryManagementService::StructuredDataMemoryManagementSubService::StructuredDataMemoryManagementSubService(
	MemoryManagementService& parent)
	: mainService(parent) {
}

void MemoryManagementService::loadRawData(Message& request) {
	/**
	 * Bear in mind that there is currently no error checking for invalid parameters.
	 * A future version will include error checking and the corresponding error report/notification,
	 * as the manual implies.
	 *
	 * @todo (#255): Add error checking and reporting for the parameters
	 * @todo (#256): Add failure reporting
	 */
	if (not request.assertTC(MemoryManagementService::ServiceType,
		MemoryManagementService::MessageType::LoadRawMemoryDataAreas)) {
		return;
	}

	auto memoryID = static_cast <MemoryManagementService::MemoryID>(request.read <MemoryId>());

	if (!memoryIdValidator(static_cast <MemoryManagementService::MemoryID>(memoryID))) {
		// TODO(#257): Send a failed start of execution
		return;
	}

	etl::array <ReadData, ECSSMaxStringSize> readData = {};
	uint16_t const iterationCount = request.readUint16();

	if (memoryID == MemoryManagementService::MemoryID::FLASH_MEMORY) {
		// TODO(#258): Define FLASH specific access code when we transfer to embedded
	} else {
		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read <StartAddress>();
			const MemoryDataLength dataLength = request.readOctetString(readData.data());
			// NOLINT(cppcoreguidelines-init-variables)
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
				*(reinterpret_cast <uint8_t*>(startAddress) + i) = readData[i];
			}

			for (std::size_t i = 0; i < dataLength; i++) {
				readData[i] = *(reinterpret_cast <uint8_t*>(startAddress) + i);
			}

			if (checksum != CRCHelper::calculateCRC(readData.data(), dataLength)) {
				ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
			}
		}
	}
}

void MemoryManagementService::RawDataMemoryManagementSubService::dumpRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DumpRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	const MemoryId memoryID = request.read <MemoryId>();

	if (memoryIdValidator(static_cast <MemoryManagementService::MemoryID>(memoryID))) {
		etl::array <ReadData, ECSSMaxStringSize> readData = {};
		uint16_t const iterationCount = request.readUint16();

		report.append <MemoryId>(memoryID);
		report.appendUint16(iterationCount);

		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read <StartAddress>();
			const MemoryDataLength readLength = request.read <MemoryDataLength>();

			if (addressValidator(static_cast <MemoryManagementService::MemoryID>(memoryID), startAddress) &&
			    addressValidator(static_cast <MemoryManagementService::MemoryID>(memoryID),
				    startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast <uint8_t*>(startAddress) + i);
				}

				report.append <StartAddress>(startAddress);
				report.appendOctetString(String <ECSSMaxFixedOctetStringSize>(readData.data(), readLength));
				report.append <CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
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

void MemoryManagementService::RawDataMemoryManagementSubService::checkRawData(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CheckRawMemoryData)) {
		return;
	}

	Message report = mainService.createTM(MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	const MemoryId memoryID = request.read <MemoryId>();

	if (memoryIdValidator(static_cast <MemoryManagementService::MemoryID>(memoryID))) {
		etl::array <ReadData, ECSSMaxStringSize> readData = {};
		uint16_t const iterationCount = request.readUint16();

		report.append <MemoryId>(memoryID);
		report.appendUint16(iterationCount);

		for (std::size_t j = 0; j < iterationCount; j++) {
			const StartAddress startAddress = request.read <StartAddress>();
			const MemoryDataLength readLength = request.read <MemoryDataLength>();

			if (addressValidator(static_cast <MemoryManagementService::MemoryID>(memoryID), startAddress) &&
			    addressValidator(static_cast <MemoryManagementService::MemoryID>(memoryID),
				    startAddress + readLength)) {
				for (std::size_t i = 0; i < readLength; i++) {
					readData[i] = *(reinterpret_cast <uint8_t*>(startAddress) + i);
				}

				report.append <StartAddress>(startAddress);
				report.append <MemoryDataLength>(readLength);
				report.append <CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
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

inline bool MemoryManagementService::dataValidator(const uint8_t* data, MemoryManagementChecksum checksum,
	MemoryDataLength length) {
	return (checksum == CRCHelper::calculateCRC(data, length));
}

void MemoryManagementService::StructuredDataMemoryManagementSubService::loadObjectMemoryData(Message& request) {
	if (not request.assertTC(ServiceType, LoadObjectMemoryData)) {
		return;
	}

	Path fullPath = "";
	readFullPath(request, fullPath);

	auto remainingInstructions = request.read <InstructionType>();
	bool hasError = false;

	while (remainingInstructions-- != 0u) {
		const Offset offset = request.read <Offset>();
		const FileDataLength dataLength = request.read <FileDataLength>();

		etl::array <uint8_t, ChunkMaxFileSizeBytes> chunkData = {};
		request.readString(chunkData.data(), dataLength);
		auto result = writeFile(fullPath, offset, dataLength, chunkData);

		if (result.has_value()) {
			hasError = true;
			ErrorHandler::ExecutionStartErrorType error; // NOLINT(cppcoreguidelines-init-variables)

			switch (result.value()) {
				case FileWriteError::FileNotFound:
					error = ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist;
					break;
				case FileWriteError::InvalidBufferSize:
					error = ErrorHandler::ExecutionStartErrorType::MemoryBufferSizeError;
					break;
				case FileWriteError::InvalidOffset:
					error = ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset;
					break;
				case FileWriteError::WriteError:
					error = ErrorHandler::ExecutionStartErrorType::MemoryWriteError;
					break;
				default:
					error = ErrorHandler::ExecutionStartErrorType::UnknownMemoryWriteError;
					break;
			}
			ErrorHandler::reportError(request, error);
			break;
		}
	}

	if (!hasError) {
		Services.requestVerification.successCompletionExecutionVerification(request);
	} else {
		Services.requestVerification.failCompletionExecutionVerification(request,
			ErrorHandler::ExecutionCompletionErrorType::LoadObjectMemoryData);
	}
}

void MemoryManagementService::StructuredDataMemoryManagementSubService::dumpObjectMemoryData(Message& request) const {
	if (not request.assertTC(ServiceType, DumpObjectMemoryData)) {
		return;
	}

	auto report = Message(ServiceType, DumpedObjectMemoryDataReport, Message::TM);
	Path fullPath = "";
	readFullPath(request, fullPath);

	auto remainingInstructions = request.read <InstructionType>();
	report.appendOctetString(fullPath);
	report.append <InstructionType>(remainingInstructions);

	while (remainingInstructions--) {
		const Offset offset = request.read <Offset>();
		const FileDataLength readLength = request.read <FileDataLength>();
		dumpedStructuredDataReport(report, fullPath, offset, readLength, remainingInstructions == 0);
	}
}

void MemoryManagementService::StructuredDataMemoryManagementSubService::dumpedStructuredDataReport(Message& report,
	const Path& filePath, const Offset offset, const FileDataLength readLength, const bool isFinal) const {
	etl::array <uint8_t, ChunkMaxFileSizeBytes> chunkData = {};
	auto result = readFile(filePath, offset, readLength, chunkData);
	bool hasError = false;
	if (result.has_value()) {
		hasError = true;
		ErrorHandler::ExecutionStartErrorType error; // NOLINT(cppcoreguidelines-init-variables)

		switch (result.value()) {
			case FileReadError::FileNotFound:
				error = ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist;
				break;
			case FileReadError::InvalidBufferSize:
				error = ErrorHandler::ExecutionStartErrorType::MemoryBufferSizeError;
				break;
			case FileReadError::InvalidOffset:
				error = ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset;
				break;
			case FileReadError::ReadError:
				error = ErrorHandler::ExecutionStartErrorType::MemoryReadError;
				break;
			default:
				error = ErrorHandler::ExecutionStartErrorType::UnknownMemoryReadError;
		}
		ErrorHandler::reportError(report, error);
	}
	if (hasError) {
		report.append <Offset>(0);
		report.append <FileDataLength>(0);
	} else {
		report.append <Offset>(offset);
		report.append <FileDataLength>(readLength);
		report.appendString(String<ChunkMaxFileSizeBytes>(chunkData.data(), readLength));
	}

	if (isFinal) {
		mainService.storeMessage(report);
	}
}

void MemoryManagementService::execute(Message& message) {
	switch (message.messageType) {
		case LoadObjectMemoryData:
			StructuredDataMemoryManagementSubService::loadObjectMemoryData(message);
			break;
		case LoadRawMemoryDataAreas:
			loadRawData(message);
			break;
		case DumpRawMemoryData:
			rawDataMemorySubservice.dumpRawData(message);
			break;
		case CheckRawMemoryData:
			rawDataMemorySubservice.checkRawData(message);
			break;
		case DumpObjectMemoryData:
			structuredDataMemoryManagementSubService.dumpObjectMemoryData(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
