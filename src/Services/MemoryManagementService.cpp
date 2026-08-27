#include "Services/MemoryManagementService.hpp"
#include "ServicePool.hpp"
#include "Services/RequestVerificationService.hpp"

using namespace Filesystem;

MemoryManagementService::MemoryManagementService()
    : rawDataMemorySubservice(*this), structuredDataMemoryManagementSubService(*this) {
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
		const MemoryAddress memoryAddress = request.read<MemoryAddress>();
		const MemoryDataLength dataLength = request.readOctetString(readData.data()); // NOLINT(cppcoreguidelines-init-variables)
		const MemoryManagementChecksum checksum = request.readBits(BitsInMemoryManagementChecksum);

		if (dataLength > readData.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnableToHandleMemoryDataLength);
			continue;
		}

		if (!dataValidator(readData.data(), checksum, dataLength)) {
			ErrorHandler::reportError(request, ErrorHandler::ChecksumFailed);
			continue;
		}

		if (!memory.isValidAddress(memoryAddress) ||
		    !memory.isValidAddress(memoryAddress + dataLength)) {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			continue;
		}

		for (std::size_t i = 0; i < dataLength; i++) {
			memory.writeData(memoryAddress, i, readData[i]);
		}

		for (std::size_t i = 0; i < dataLength; i++) {
			readData[i] = memory.readData(memoryAddress, i);
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
		const MemoryAddress memoryAddress = request.read<MemoryAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (readLength > readData.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnableToHandleMemoryDataLength);
			continue;
		}
		if (!memory.isValidAddress(memoryAddress) ||
		    !memory.isValidAddress(memoryAddress + readLength)) {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			continue;
		}

		for (std::size_t i = 0; i < readLength; i++) {
			readData[i] = memory.readData(memoryAddress, i);
		}

		report.append<MemoryAddress>(memoryAddress);
		report.appendOctetString(String<ECSSMaxFixedOctetStringSize>(readData.data(), readLength));
		report.append<CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
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
		const MemoryAddress memoryAddress = request.read<MemoryAddress>();
		const MemoryDataLength readLength = request.read<MemoryDataLength>();

		if (readLength > readData.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnableToHandleMemoryDataLength);
			continue;
		}
		if (!memory.isValidAddress(memoryAddress) ||
		    !memory.isValidAddress(memoryAddress + readLength)) {
			ErrorHandler::reportError(request, ErrorHandler::AddressOutOfRange);
			continue;
		}

		for (std::size_t i = 0; i < readLength; i++) {
			readData[i] = memory.readData(memoryAddress, i);
		}

		report.append<MemoryAddress>(memoryAddress);
		report.append<MemoryDataLength>(readLength);
		report.append<CRCSize>(CRCHelper::calculateCRC(readData.data(), readLength));
	}

	mainService.storeMessage(report);
	request.resetRead();
}

void MemoryManagementService::StructuredDataMemoryManagementSubService::loadObjectMemoryData(Message& request) {
	if (not request.assertTC(ServiceType, LoadObjectMemoryData)) {
		return;
	}

	const Path fullPath = readFullPath(request);

	auto remainingInstructions = request.read<InstructionType>();
	bool hasError = false;

	while (remainingInstructions-- != 0U) {
		const FileOffset offset = request.read<FileOffset>();
		const FileDataLength dataLength = request.read<FileDataLength>();

		etl::array<uint8_t, ChunkMaxFileSizeBytes> chunkData = {};
		if (dataLength > chunkData.size()) {
			// The remaining data of this instruction cannot be safely skipped, so parsing stops here
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MemoryBufferSizeError);
			hasError = true;
			break;
		}
		request.readString(chunkData.data(), static_cast<uint16_t>(dataLength));
		auto result = writeFile(fullPath, offset, dataLength, chunkData);

		if (!result.has_value()) {
			hasError = true;
			ErrorHandler::ExecutionStartErrorType error; // NOLINT(cppcoreguidelines-init-variables)

			switch (result.error()) {
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

	Message report = mainService.createTM(MemoryManagementService::MessageType::DumpedObjectMemoryDataReport);
	const Path fullPath = readFullPath(request);

	auto remainingInstructions = request.read<InstructionType>();
	report.appendOctetString(fullPath);
	report.append<InstructionType>(remainingInstructions);

	while (remainingInstructions-- != 0U) {
		const FileOffset offset = request.read<FileOffset>();
		const FileDataLength readLength = request.read<FileDataLength>();
		appendDumpedStructuredData(request, report, fullPath, offset, readLength);
	}

	mainService.storeMessage(report);
}

void MemoryManagementService::StructuredDataMemoryManagementSubService::appendDumpedStructuredData(
    Message& request, Message& report, const Path& filePath, const FileOffset offset,
    const FileDataLength readLength) const {
	etl::array<uint8_t, ChunkMaxFileSizeBytes> chunkData = {};
	auto result = readFile(filePath, offset, readLength, chunkData);

	if (!result.has_value()) {
		ErrorHandler::ExecutionStartErrorType error; // NOLINT(cppcoreguidelines-init-variables)

		switch (result.error()) {
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
		ErrorHandler::reportError(request, error);

		report.append<FileOffset>(offset);
		report.append<FileDataLength>(0);
		return;
	}

	report.append<FileOffset>(offset);
	report.append<FileDataLength>(readLength);
	report.appendString(String<ChunkMaxFileSizeBytes>(chunkData.data(), readLength));
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
