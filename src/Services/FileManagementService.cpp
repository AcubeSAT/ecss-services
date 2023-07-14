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

	// ?????
	uint16_t fileSizeBytes = message.readUint32();

	if (fileSizeBytes > MaxFileSizeBytes) {
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

	switch (pathIsValidForCreation(repositoryPath)) {
		case LFS_TYPE_DIR: {
			lfs_file_t file;
			int32_t createFileStatus = littleFsCreateFile(&onBoardFileSystemObject,
			                                              &file,
			                                              repositoryPath,
			                                              fileName,
			                                              LFS_O_CREAT);
			if (createFileStatus >= LFS_ERR_OK) {
				if (lfs_file_close(&onBoardFileSystemObject, &file) >= LFS_ERR_OK) {
					return;
				} else {
					ErrorHandler::reportError(message,
					                          ErrorHandler::ExecutionCompletionErrorType::LittleFsFileCloseFailed);
					return;
				}
			}

			if (createFileStatus == OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
				return;
			}

			if (createFileStatus == WILDCARD_FOUND) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
				return;
			}

			if (createFileStatus == LFS_ERR_EXIST) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::FileAlreadyExists);
				return;
			}

			if (createFileStatus < LFS_ERR_OK) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::LittleFsFileOpenFailed);
				return;
			}
			break;
		}

		case LFS_TYPE_REG:
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
			break;
		case (WILDCARD_FOUND):
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
			break;
		case (OBJECT_TYPE_IS_INVALID):
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;
		default:
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
			break;
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

	switch (pathIsValidForDeletion(repositoryPath, fileName)) {
		case LFS_TYPE_REG: {
			if (littleFsDeleteFile(&onBoardFileSystemObject, repositoryPath, fileName) >= LFS_ERR_OK) {
				return;
			} else {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::LittleFsRemoveFailed);
				return;
			}
			break;
		}

		case LFS_TYPE_DIR:
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;

		case (WILDCARD_FOUND):
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
			break;

		case (OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE):
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
			break;

		case (OBJECT_TYPE_IS_INVALID):
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;

		default:
			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
			break;
	}
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

	lfs_info infoStruct;

	switch (littleFsReportFile(repositoryPath, fileName, &infoStruct)) {

		case (OBJECT_TYPE_IS_INVALID):

			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;

		case (LFS_TYPE_REG):

			fileAttributeReport(repositoryPath, fileName, infoStruct.size);
			break;

		case (LFS_TYPE_DIR):

			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;

		default:

			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
			break;
	}
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
