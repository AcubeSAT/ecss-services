#include "Services/FileManagementService.hpp"
#include <cstdint>
#include "ErrorHandler.hpp"
#include "Helpers/FilepathValidators.hpp"
#include "Helpers/Filesystem.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "etl/vector.h"

void FileManagementService::createFile(Message& message) {
	using namespace FilepathValidators;

	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile);

	Filesystem::Path repositoryPath("");
	Filesystem::Path fileName("");

	auto repositoryPathIsValid = getStringUntilZeroTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

	auto fileNameIsValid = getStringUntilZeroTerminator(message);
	if (not fileNameIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	fileName = fileNameIsValid.value();

	uint32_t maxFileSizeBytes = message.readUint32();

	if (maxFileSizeBytes > MaxPossibleFileSizeBytes) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
		return;
	}

	if (findWildcardPosition(repositoryPath).has_value()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto repositoryType = Filesystem::getNodeType(repositoryPath);
	if (not repositoryType) {
		ErrorHandler::reportError(message,
		                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
		return;
	}

	if (repositoryType.value() == Filesystem::NodeType::File) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
		return;
	}

	auto fullPath = repositoryPath;
	fullPath.append(fileName);

	etl::optional<Filesystem::FileCreationError> fileCreationStatus = Filesystem::createFile(fullPath);
	if (fileCreationStatus.has_value()) {
		switch (fileCreationStatus.value()) {
			case Filesystem::FileCreationError::FileAlreadyExists: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::FileAlreadyExists);
				break;
			}
			case Filesystem::FileCreationError::UnknownError: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
				break;
			}
		}
		return;
	}
}

void FileManagementService::deleteFile(Message& message) {
	using namespace FilepathValidators;

	message.assertTC(FileManagementService::ServiceType,
	                 FileManagementService::MessageType::DeleteFile);

	Filesystem::Path repositoryPath("");
	Filesystem::Path fileName("");

	auto repositoryPathIsValid = getStringUntilZeroTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

	auto fileNameIsValid = getStringUntilZeroTerminator(message);
	if (not fileNameIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	fileName = fileNameIsValid.value();

	//	switch (pathIsValidForDeletion(repositoryPath, fileName)) {
	//		case LFS_TYPE_REG: {
	//			if (littleFsDeleteFile(&onBoardFileSystemObject, repositoryPath, fileName) >= LFS_ERR_OK) {
	//				return;
	//			} else {
	//				ErrorHandler::reportError(message,
	//				                          ErrorHandler::ExecutionCompletionErrorType::LittleFsRemoveFailed);
	//				return;
	//			}
	//			break;
	//		}
	//
	//		case LFS_TYPE_DIR:
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
	//			break;
	//
	//		case (WILDCARD_FOUND):
	//			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
	//			break;
	//
	//		case (OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE):
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
	//			break;
	//
	//		case (OBJECT_TYPE_IS_INVALID):
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
	//			break;
	//
	//		default:
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
	//			break;
	//	}
}

void FileManagementService::reportAttributes(Message& message) {
	using namespace FilepathValidators;

	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

	String<ECSSMaxStringSize> repositoryPath("");
	String<ECSSMaxStringSize> fileName("");

	auto repositoryPathIsValid = getStringUntilZeroTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

	auto fileNameIsValid = getStringUntilZeroTerminator(message);
	if (not fileNameIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	fileName = fileNameIsValid.value();

	if (findWildcardPosition(repositoryPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if (findWildcardPosition(fileName)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}

	//	lfs_info infoStruct;
	//
	//	switch (littleFsReportFile(repositoryPath, fileName, &infoStruct)) {
	//
	//		case (OBJECT_TYPE_IS_INVALID):
	//
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
	//			break;
	//
	//		case (LFS_TYPE_REG):
	//
	//			fileAttributeReport(repositoryPath, fileName, infoStruct.size);
	//			break;
	//
	//		case (LFS_TYPE_DIR):
	//
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
	//			break;
	//
	//		default:
	//
	//			ErrorHandler::reportError(message,
	//			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
	//			break;
	//	}
}

void FileManagementService::fileAttributeReport(const String<ECSSMaxStringSize>& repositoryPath,
                                                const String<ECSSMaxStringSize>& fileName,
                                                uint32_t fileSize) {
	Message report = createTM(MessageType::CreateAttributesReport);

	report.appendString(repositoryPath);
	report.appendUint8('@');
	report.appendString(fileName);
	report.appendUint32(fileSize);

	storeMessage(report);
}

void FileManagementService::execute(Message& message) {
	switch (message.messageType) {
		case CreateFile:
			createFile(message);
			break;
		case DeleteFile:
			deleteFile(message);
			break;
		case ReportAttributes:
			reportAttributes(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
