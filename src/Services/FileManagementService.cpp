#include "Services/FileManagementService.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/FilepathValidators.hpp"
#include "Helpers/Filesystem.hpp"
#include "Message.hpp"

using namespace FilepathValidators;

void FileManagementService::createFile(Message& message) {
	message.assertTC(ServiceType, CreateFile);

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, fileName);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto repositoryType = Filesystem::getNodeType(repositoryPath);
	if (not repositoryType) {
		ErrorHandler::reportError(message,
		                          ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist);
		return;
	}

	if (repositoryType.value() != Filesystem::NodeType::Directory) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
		return;
	}

	uint32_t const maxFileSizeBytes = message.readUint32();
	if (maxFileSizeBytes > MaxPossibleFileSizeBytes) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
		return;
	}

	bool const isFileLocked = message.readBoolean();

	if (auto fileCreationError = Filesystem::createFile(fullPath)) {
		switch (fileCreationError.value()) {
			case Filesystem::FileCreationError::FileAlreadyExists: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::FileAlreadyExists);
				return;
			}
			default: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
				return;
			}
		}
	}

	if (isFileLocked) {
		Filesystem::lockFile(fullPath);
	}
}

void FileManagementService::deleteFile(Message& message) {
	message.assertTC(ServiceType, DeleteFile);

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, fileName);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto repositoryType = Filesystem::getNodeType(repositoryPath);
	if (not repositoryType) {
		ErrorHandler::reportError(message,
		                          ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
		return;
	}

	if (repositoryType.value() != Filesystem::NodeType::Directory) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
		return;
	}

	if (auto fileDeletionError = Filesystem::deleteFile(fullPath)) {
		using Filesystem::FileDeletionError;
		switch (fileDeletionError.value()) {
			case FileDeletionError::FileDoesNotExist:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist);
				break;
			case FileDeletionError::PathLeadsToDirectory:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteOnDirectory);
				break;
			case FileDeletionError::FileIsLocked:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteOnLockedFile);
				break;
			default:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownFileDeleteError);
				break;
		}
	}
}

void FileManagementService::reportAttributes(Message& message) {
	message.assertTC(ServiceType, ReportAttributes);

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, fileName);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	using namespace Filesystem;
	auto fileAttributeResult = getFileAttributes(fullPath);
	if (fileAttributeResult.is_value()) {
		fileAttributeReport(repositoryPath, fileName, fileAttributeResult.value());
		return;
	}

	switch (fileAttributeResult.error()) {
		case FileAttributeError::PathLeadsToDirectory:
			ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedReportAttributesOnDirectory);
			break;
		case FileAttributeError::FileDoesNotExist:
			ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist);
			break;
		default:
			ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
			break;
	}
}

void FileManagementService::fileAttributeReport(const ObjectPath& repositoryPath, const ObjectPath& fileName, const Filesystem::Attributes& attributes) {
	Message report = createTM(MessageType::CreateAttributesReport);

	report.appendOctetString(repositoryPath);
	report.appendOctetString(fileName);
	report.appendUint32(attributes.sizeInBytes);
	report.appendBoolean(attributes.isLocked);

	storeMessage(report);
}

void FileManagementService::createDirectory(Message& message) {
	message.assertTC(ServiceType, CreateDirectory);

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto directoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, directoryPath);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto repositoryType = Filesystem::getNodeType(repositoryPath);
	if (not repositoryType) {
		ErrorHandler::reportError(message,
		                          ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist);
		return;
	}

	if (repositoryType.value() != Filesystem::NodeType::Directory) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
		return;
	}

	if (auto fileCreationError = Filesystem::createDirectory(fullPath)) {
		switch (fileCreationError.value()) {
			case Filesystem::DirectoryCreationError::DirectoryAlreadyExists: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::DirectoryAlreadyExists);
				return;
			}
			default: {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
				return;
			}
		}
	}
}

void FileManagementService::deleteDirectory(Message& message) {
	message.assertTC(ServiceType, DeleteDirectory);

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto directoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, directoryPath);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	auto repositoryType = Filesystem::getNodeType(repositoryPath);
	if (not repositoryType) {
		ErrorHandler::reportError(message,
		                          ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
		return;
	}

	if (repositoryType.value() != Filesystem::NodeType::Directory) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
		return;
	}

	if (auto fileDeletionError = Filesystem::deleteDirectory(fullPath)) {
		using Filesystem::DirectoryDeletionError;
		switch (fileDeletionError.value()) {
			case DirectoryDeletionError::DirectoryDoesNotExist:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist);
				break;
			case DirectoryDeletionError::DirectoryIsNotEmpty:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::AttemptedDeleteNonEmptyDirectory);
				break;
			default:
				ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
				break;
		}
	}
}

uint32_t FileManagementService::getUnallocatedMemory() {
	return Filesystem::getUnallocatedMemory();
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
		case CreateDirectory:
			createDirectory(message);
			break;
		case DeleteDirectory:
			deleteDirectory(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
