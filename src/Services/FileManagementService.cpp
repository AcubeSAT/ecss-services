#include "Services/FileManagementService.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/FilepathValidators.hpp"
#include "Helpers/Filesystem.hpp"
#include "Message.hpp"
#include "ServicePool.hpp"

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
	if (fileAttributeResult.has_value()) {
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

void FileManagementService::lockFile(Message& message) {
	if (not message.assertTC(ServiceType, LockFile)) {
		return;
	}

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, fileName);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	if (auto result = Filesystem::lockFile(fullPath); !result.has_value()) {
		ErrorHandler::ExecutionCompletionErrorType error; // NOLINT(cppcoreguidelines-init-variables)
		switch (result.error()) {
			case Filesystem::FilePermissionModificationError::FileDoesNotExist: {
				error = ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist;
				break;
			}
			case Filesystem::FilePermissionModificationError::PathLeadsToDirectory: {
				error = ErrorHandler::ExecutionCompletionErrorType::AttemptedAccessModificationOnDirectory;
				break;
			}
			default: {
				error = ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError;
				break;
			}
		}
		ErrorHandler::reportError(message, error);
	}
}

void FileManagementService::unlockFile(Message& message) {
	if (not message.assertTC(ServiceType, UnlockFile)) {
		return;
	}

	auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto fullPath = getFullPath(repositoryPath, fileName);

	if (findWildcardPosition(fullPath)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}

	if (auto result = Filesystem::unlockFile(fullPath); !result.has_value()) {
		ErrorHandler::ExecutionCompletionErrorType error; // NOLINT(cppcoreguidelines-init-variables)
		switch (result.error()) {
			case Filesystem::FilePermissionModificationError::FileDoesNotExist: {
				error = ErrorHandler::ExecutionCompletionErrorType::ObjectDoesNotExist;
				break;
			}
			case Filesystem::FilePermissionModificationError::PathLeadsToDirectory: {
				error = ErrorHandler::ExecutionCompletionErrorType::AttemptedAccessModificationOnDirectory;
				break;
			}
			default: {
				error = ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError;
				break;
			}
		}
		ErrorHandler::reportError(message, error);
	}
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

void FileManagementService::copyFile(Message& message) {
	if (not message.assertTC(ServiceType, CopyFile)) {
		return;
	}
	if (auto [operationId, sourceFullPath, targetFullPath] = parseFileCopyRequest(message);
		validateFileCopyOperationRegistration(message, operationId, sourceFullPath, targetFullPath, FileCopyOperation::Type::COPY)) {
		Filesystem::copyFile(operationId);
	}
}

void FileManagementService::moveFile(Message& message) {
	if (not message.assertTC(ServiceType, MoveFile)) {
		return;
	}
	if (auto [operationId, sourceFullPath, targetFullPath] = parseFileCopyRequest(message);
		validateFileCopyOperationRegistration(message, operationId, sourceFullPath, targetFullPath, FileCopyOperation::Type::MOVE)) {
		Filesystem::moveFile(operationId);
	}
}

void FileManagementService::suspendFileCopyOperations(Message& message) {
    if (not message.assertTC(ServiceType, SuspendFileCopyOperation)) {
		return;
	}
	const uint8_t operationCount = message.readUint8();
    for (uint8_t i = 0; i < operationCount; i++) {
		const auto operationId = message.readUint16();
        FileCopyOperation* operation = findFileCopyOperation(operationId);
        if (operation == nullptr) {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdNotFound);
            continue;
        }
    	operation->updateOperationStateIfMatchesPath<FileCopyOperation::State::IN_PROGRESS, FileCopyOperation::State::ON_HOLD>(ObjectPath(""));
    }
}

void FileManagementService::resumeFileCopyOperations(Message& message) {
    if (not message.assertTC(ServiceType, ResumeFileCopyOperation)) {
		return;
	}
	const uint8_t operationCount = message.readUint8();
    for (uint8_t i = 0; i < operationCount; i++) {
		const auto operationId = message.readUint16();
        FileCopyOperation* operation = findFileCopyOperation(operationId);
        if (operation == nullptr) {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdNotFound);
            continue;
        }
    	operation->updateOperationStateIfMatchesPath<FileCopyOperation::State::ON_HOLD, FileCopyOperation::State::IN_PROGRESS>(ObjectPath(""));
    }
}

void FileManagementService::abortFileCopyOperations(Message& message) {
    if (not message.assertTC(ServiceType, AbortFileCopyOperation)) {
		return;
	}
	const uint8_t operationCount = message.readUint8();
    for (uint8_t i = 0; i < operationCount; i++) {
		const auto operationId = message.readUint16();
        FileCopyOperation* operation = findFileCopyOperation(operationId);
        if (operation == nullptr) {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdNotFound);
            continue;
        }
        if (!operation->setState(FileCopyOperation::State::FAILED)) {
        	ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::InvalidStateTransition);
        	continue;
        }
        removeFileCopyOperation(operationId);
    }
}

void FileManagementService::suspendFileCopyOperationsInPath(Message& message) {
    if (not message.assertTC(ServiceType, SuspendFileCopyOperationInPath)) {
		return;
	}
	const auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
    for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
    	fileCopyOperations[i].updateOperationStateIfMatchesPath<FileCopyOperation::State::IN_PROGRESS, FileCopyOperation::State::ON_HOLD>(repositoryPath);
    }
}

void FileManagementService::resumeFileCopyOperationsInPath(Message& message) {
    if (not message.assertTC(ServiceType, ResumeFileCopyOperationInPath)) {
		return;
	}
	const auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
    for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
    	fileCopyOperations[i].updateOperationStateIfMatchesPath<FileCopyOperation::State::ON_HOLD, FileCopyOperation::State::IN_PROGRESS>(repositoryPath);
    }
}

void FileManagementService::abortFileCopyOperationsInPath(Message& message) {
	if (not message.assertTC(ServiceType, AbortFileCopyOperationInPath)) {
		return;
	}

	const auto repositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	bool hasActiveOperations = false;

	for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
		if (fileCopyOperations[i].isActive() && fileCopyOperations[i].involvesRepositoryPath(repositoryPath)) {
			hasActiveOperations = true;
			const uint16_t operationId = fileCopyOperations[i].operationId;
			if (!fileCopyOperations[i].setState(FileCopyOperation::State::FAILED)) {
				ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::InvalidStateTransition);
				continue;
			}
			removeFileCopyOperation(operationId);
		}
	}

	if (!hasActiveOperations) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::NoActiveFileCopyOperationsFound);
	}
}

void FileManagementService::enablePeriodicFileCopyStatusReporting(Message& message) {
    if (not message.assertTC(ServiceType, EnablePeriodicReportingOfFileCopy)) {
		return;
	}
	const uint16_t intervalMs = message.readUint16();
    if (intervalMs < MinFileCopyReportingIntervalMs || intervalMs > MaxFileCopyReportingIntervalMs) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::InvalidReportingInterval);
        return;
    }
    fileCopyReportingIntervalMs = intervalMs;
    periodicFileCopyReportingEnabled = true;
}

void FileManagementService::disablePeriodicFileCopyStatusReporting(const Message& message) {
    if (not message.assertTC(ServiceType, DisablePeriodicReportingOfFileCopy)) {
        return;
    }
    periodicFileCopyReportingEnabled = false;
}

FileManagementService::FileCopyRequest FileManagementService::parseFileCopyRequest(Message& message) {
	FileCopyRequest request;
	request.operationId = message.readUint16();
	auto sourceRepositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto sourceFileName = message.readOctetString<Filesystem::ObjectPathSize>();
	auto targetRepositoryPath = message.readOctetString<Filesystem::ObjectPathSize>();
	auto targetFileName = message.readOctetString<Filesystem::ObjectPathSize>();

	request.sourceFullPath = getFullPath(sourceRepositoryPath, sourceFileName);
	request.targetFullPath = getFullPath(targetRepositoryPath, targetFileName);

	return request;
}

bool FileManagementService::validateFileCopyOperationRegistration(
    const Message& message,
    const uint16_t operationId,
    const Filesystem::Path& sourceFullPath,
    const Filesystem::Path& targetFullPath,
    const FileCopyOperation::Type operationType) {

    if (operationIdManager.isInUse(operationId)) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdAlreadyInUse);
        return false;
    }

    if (findWildcardPosition(sourceFullPath) || findWildcardPosition(targetFullPath)) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
        return false;
    }

    const auto sourceRepositoryType = Filesystem::getNodeType(sourceFullPath);
    if (not sourceRepositoryType) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
        return false;
    }

    if (sourceRepositoryType.value() == Filesystem::NodeType::Directory) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationRequestedOnDirectory);
        return false;
    }

    if (not Filesystem::copyOperationInvolvesLocalPath(sourceFullPath, targetFullPath)) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationRequestedFromRemoteToRemoteRepository);
        return false;
    }

    if (Filesystem::FileLockStatus::Locked == Filesystem::getFileLockStatus(sourceFullPath)) {
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::FileCopyOperationRequestedOnLockedFile);
        return false;
    }

	if (const auto result = addFileCopyOperation(operationId, sourceFullPath.data(), targetFullPath.data(), operationType, message);
		!result.has_value()) {
		ErrorHandler::ExecutionStartErrorType error; // NOLINT(cppcoreguidelines-init-variables)
		switch (result.error()) {
			case FileCopyOperation::OperationRegistrationError::MAXIMUM_CONCURRENT_OPERATIONS_REACHED: {
				error = ErrorHandler::ExecutionStartErrorType::MaximumNumberOfFileCopyOperationsReached;
				break;
			}
			case FileCopyOperation::OperationRegistrationError::OPERATION_ID_ALREADY_EXISTS: {
				error = ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdAlreadyInUse;
				break;
			}
			default: {
				error = ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError;
				break;
			}
		}
		ErrorHandler::reportError(message, error);
        return false;
    }
    return true;
}

void FileManagementService::generateFileCopyStatusReport() {
	Message report = createTM(FileCopyStatusReport);
	uint8_t ongoingCount = 0;
	for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
		if (fileCopyOperations[i].isActive()) {
			ongoingCount++;
		}
	}
	report.appendUint8(ongoingCount);
	for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
		if (fileCopyOperations[i].isActive()) {
			report.appendUint16(fileCopyOperations[i].operationId);
			report.appendUint8(static_cast<uint8_t>(fileCopyOperations[i].state));
			report.appendUint8(fileCopyOperations[i].getProgressPercentage());
		}
	}
	storeMessage(report);
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
		case LockFile:
			lockFile(message);
			break;
		case UnlockFile:
			unlockFile(message);
			break;
		case CreateDirectory:
			createDirectory(message);
			break;
		case DeleteDirectory:
			deleteDirectory(message);
			break;
		case CopyFile:
			copyFile(message);
			break;
		case MoveFile:
			moveFile(message);
			break;
		case SuspendFileCopyOperation:
			suspendFileCopyOperations(message);
			break;
		case ResumeFileCopyOperation:
			resumeFileCopyOperations(message);
			break;
		case AbortFileCopyOperation:
			abortFileCopyOperations(message);
			break;
		case SuspendFileCopyOperationInPath:
			suspendFileCopyOperationsInPath(message);
			break;
		case ResumeFileCopyOperationInPath:
			resumeFileCopyOperationsInPath(message);
			break;
		case AbortFileCopyOperationInPath:
			abortFileCopyOperationsInPath(message);
			break;
		case EnablePeriodicReportingOfFileCopy:
			enablePeriodicFileCopyStatusReporting(message);
			break;
		case DisablePeriodicReportingOfFileCopy:
			disablePeriodicFileCopyStatusReporting(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

FileManagementService::FileCopyOperation* FileManagementService::findFileCopyOperation(const uint16_t operationId) {
	for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
	    if (fileCopyOperations[i].operationId == operationId) {
	        return &fileCopyOperations[i];
	    }
	}
	return nullptr;
}

etl::expected<void, FileManagementService::FileCopyOperation::OperationRegistrationError> FileManagementService::addFileCopyOperation(const uint16_t operationId,
                                               const ObjectPath& sourcePath,
                                               const ObjectPath& targetPath,
                                               const FileCopyOperation::Type type,
                                               const Message& message) {
    if (activeFileCopyOperationCount >= MaxConcurrentFileCopyOperations) {
        return etl::unexpected(FileCopyOperation::OperationRegistrationError::MAXIMUM_CONCURRENT_OPERATIONS_REACHED);
    }
    if (!operationIdManager.reserveId(operationId)) {
        return etl::unexpected(FileCopyOperation::OperationRegistrationError::OPERATION_ID_ALREADY_EXISTS);
    }
    for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
        if (fileCopyOperations[i].state == FileCopyOperation::State::IDLE) {
            fileCopyOperations[i].initialize(operationId, sourcePath, targetPath, type, message);
            activeFileCopyOperationCount++;
            return {};
        }
    }
    operationIdManager.releaseId(operationId);
    return etl::unexpected(FileCopyOperation::OperationRegistrationError::UNKNOWN_ERROR);
}

void FileManagementService::removeFileCopyOperation(const uint16_t operationId) {
    for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
        if (fileCopyOperations[i].operationId == operationId && fileCopyOperations[i].state != FileCopyOperation::State::IDLE) {
            fileCopyOperations[i].reset();
            operationIdManager.releaseId(operationId);
            activeFileCopyOperationCount--;
            break;
        }
    }
}

bool FileManagementService::setOperationState(const uint16_t operationId, const FileCopyOperation::State newState) {
	FileCopyOperation* operation = findFileCopyOperation(operationId);
	if (operation == nullptr) {
		return false;
	}
	return operation->setState(newState);
}

FileManagementService::FileCopyOperation::SuspensionStatus FileManagementService::getOperationSuspensionStatus(const uint16_t operationId) const {
	for (uint8_t i = 0; i < MaxConcurrentFileCopyOperations; i++) {
		if (fileCopyOperations[i].operationId == operationId) {
			if (fileCopyOperations[i].state == FileCopyOperation::State::ON_HOLD) {
				return FileCopyOperation::SuspensionStatus::SUSPENDED;
			}
			return FileCopyOperation::SuspensionStatus::NOT_SUSPENDED;
		}
	}
	return FileCopyOperation::SuspensionStatus::NOT_FOUND;
}

etl::expected<void, FileManagementService::FileCopyOperation::OperationProgressUpdateError> FileManagementService::updateOperationProgress(const uint16_t operationId,
                                                    const uint32_t bytesTransferred,
                                                    const uint32_t totalBytes) {
	if (FileCopyOperation* operation = findFileCopyOperation(operationId); operation != nullptr) {
        operation->bytesTransferred = bytesTransferred;
        operation->totalBytes = totalBytes;
        if (bytesTransferred >= totalBytes && totalBytes > 0) {
        	if (operation->setState(FileCopyOperation::State::COMPLETED)) {
        		return {};
        	}
        	return etl::unexpected(FileCopyOperation::OperationProgressUpdateError::INVALID_OPERATION_STATE_TRANSITION);
        }
    }
	return etl::unexpected(FileCopyOperation::OperationProgressUpdateError::FILE_COPY_OPERATION_NOT_FOUND);
}

void FileManagementService::notifyOperationSuccess(const uint16_t operationId) {
	const FileCopyOperation* operation = findFileCopyOperation(operationId);
	if (operation == nullptr) {
		return;
	}
	Services.requestVerification.successCompletionExecutionVerification(operation->requestMessage);
	removeFileCopyOperation(operationId);
}

void FileManagementService::notifyOperationFailure(const uint16_t operationId, const Filesystem::FileCopyError errorType) {
	const FileCopyOperation* operation = findFileCopyOperation(operationId);
    if (operation == nullptr) {
    	return;
    }
    ErrorHandler::ExecutionCompletionErrorType error; // NOLINT(cppcoreguidelines-init-variables)
    switch (errorType) {
        case Filesystem::FileCopyError::DestinationFileAlreadyExists:
            error = ErrorHandler::ExecutionCompletionErrorType::DestinationFileAlreadyExists;
            break;
        case Filesystem::FileCopyError::ReadFailure:
            error = ErrorHandler::ExecutionCompletionErrorType::FileSystemReadFailure;
            break;
        case Filesystem::FileCopyError::WriteFailure:
            error = ErrorHandler::ExecutionCompletionErrorType::FileSystemWriteFailure;
            break;
        case Filesystem::FileCopyError::CommunicationFailure:
            error = ErrorHandler::ExecutionCompletionErrorType::FileSystemCommunicationFailure;
            break;
        case Filesystem::FileCopyError::InsufficientSpace:
            error = ErrorHandler::ExecutionCompletionErrorType::FileSystemInsufficientSpace;
            break;
    	case Filesystem::FileCopyError::FileCopyOperationNotFound:
    		error = ErrorHandler::ExecutionCompletionErrorType::FileCopyOperationNotFound;
    		break;
    	case Filesystem::FileCopyError::FailedToUpdateOperationState:
    		error = ErrorHandler::ExecutionCompletionErrorType::FailedToUpdateOperationState;
    		break;
        default:
            error = ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError;
            break;
    }
	ErrorHandler::reportError(operation->requestMessage, error);
    removeFileCopyOperation(operationId);
}
