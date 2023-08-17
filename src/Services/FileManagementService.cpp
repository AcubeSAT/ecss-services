#include "Services/FileManagementService.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/FilepathValidators.hpp"
#include "Helpers/Filesystem.hpp"
#include "Message.hpp"

void FileManagementService::createFile(Message& message) {
	using namespace FilepathValidators;

	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile);

	Filesystem::Path repositoryPath("");

	auto repositoryPathIsValid = getStringUntilTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

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

	Filesystem::Path fileName("");
	auto fileNameIsValid = getStringUntilTerminator(message);
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

	bool isFileLocked = message.readBoolean();

	if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	auto fullPath = repositoryPath;
	fullPath.append(fileName);

	if (findWildcardPosition(fullPath).has_value()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	etl::optional<Filesystem::FileCreationError> fileCreationError = Filesystem::createFile(fullPath);
	if (fileCreationError.has_value()) {
		switch (fileCreationError.value()) {
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

	auto repositoryPathIsValid = getStringUntilTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

	if (findWildcardPosition(repositoryPath).has_value()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	Filesystem::Path fileName("");
	auto fileNameIsValid = getStringUntilTerminator(message);
	if (not fileNameIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	fileName = fileNameIsValid.value();

	if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	auto fullPath = repositoryPath;
	fullPath.append(fileName);

	if (findWildcardPosition(fullPath).has_value()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto fileDeletionError = Filesystem::deleteFile(fullPath);
	if (fileDeletionError.has_value()) {
		using Filesystem::FileDeletionError;
		switch (fileDeletionError.value()) {
			case FileDeletionError::FileDoesNotExist:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteOnMissingFile);
				break;
			case FileDeletionError::PathLeadsToDirectory:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteOnDirectory);
				break;
			case FileDeletionError::FileIsLocked:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteOnLockedFile);
				break;
			case FileDeletionError::UnknownError:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownFileDeleteError);
				break;
		}
	}
}

void FileManagementService::reportAttributes(Message& message) {
	using namespace FilepathValidators;

	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

	String<ECSSMaxStringSize> repositoryPath("");
	auto repositoryPathIsValid = getStringUntilTerminator(message);
	if (not repositoryPathIsValid) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	repositoryPath = repositoryPathIsValid.value();

	auto fileNameIsValid = getStringUntilZeroTerminator(message);
	auto fileNameIsValid = getStringUntilTerminator(message);
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
                                                const Filesystem::Attributes& attributes) {
	Message report = createTM(MessageType::CreateAttributesReport);

	report.appendString(repositoryPath);
	report.appendUint8('@');
	report.appendString(fileName);
	report.appendUint32(attributes.sizeInBytes);
	report.appendBoolean(attributes.isLocked);

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
