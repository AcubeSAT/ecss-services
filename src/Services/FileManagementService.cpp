#include "Services/FileManagementService.hpp"
#include <stdint.h>
#include "ErrorHandler.hpp"
#include "Helpers/lfs_stub.h"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "etl/vector.h"

int16_t FileManagementService::findWildcardPosition(String<ECSSMaxStringSize> messageString) {

	int16_t wildcardPosition = messageString.find(wildcard, 0);

	if (wildcardPosition == -1) {
		return NO_WILDCARD_FOUND;
	} else {
		return wildcardPosition;
	}
}

FileManagementService::StringTerminatorStatus FileManagementService::getStringUntilZeroTerminator(Message& message,
                                                                                                  String<ECSSMaxStringSize>& extractedString) {
	uint8_t charCounter = 0;
	char currentChar = static_cast<char>(message.readByte());

	while (currentChar != FileManagementService::variableStringTerminator) {

		if (charCounter == ECSSMaxStringSize - 1) {
			return stringTerminatorNotFound;
		}

		extractedString.append(1, currentChar);
		charCounter++;
		currentChar = message.readByte();
	}

	return stringTerminatorFound;
}

int32_t FileManagementService::pathIsValidForCreation(String<ECSSMaxStringSize> repositoryString) {
	lfs_info infoStruct;

	if (FileManagementService::findWildcardPosition(repositoryString) != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	const char* repositoryStringChar = repositoryString.data();
	int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject,
	                                        repositoryStringChar,
	                                        &infoStruct);

	if (infoStructFillStatus >= LFS_ERR_OK) {
		switch (infoStruct.type) {
			case LFS_TYPE_DIR:
				return LFS_TYPE_DIR;
				break;

			case LFS_TYPE_REG:
				return LFS_TYPE_REG;
				break;

			default:
				return OBJECT_TYPE_IS_INVALID;
				break;
		}
	} else {
		return infoStructFillStatus;
	}
}

void FileManagementService::checkForSlashesAndCompensate(String<ECSSMaxStringSize>& objectPathString, uint8_t*& fileNameChar) {

	char lastPathCharacter = objectPathString.back();
	char firstFileCharacter = *fileNameChar;

	if (lastPathCharacter == '/') {
		if (firstFileCharacter == '/') {
			fileNameChar = fileNameChar + 1;
		} else {
		}
	} else {
		if (firstFileCharacter == '/') {
		} else {
			objectPathString.append(1, '/');
		}
	}
}

int32_t FileManagementService::littleFsCreateFile(lfs_t* fileSystem,
                                                  lfs_file_t* file,
                                                  String<ECSSMaxStringSize> repositoryPath,
                                                  String<ECSSMaxStringSize> fileName,
                                                  const int32_t flags) {

	if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
		return OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE;
	}

	if (FileManagementService::findWildcardPosition(fileName) != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	char* const repositoryPathChar = repositoryPath.data();
	String<ECSSMaxStringSize> objectPathString = "";
	objectPathString.append(repositoryPathChar);

	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileName.data());
	checkForSlashesAndCompensate(objectPathString, fileNameChar);

	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	int32_t lfsCreateFileStatus = lfs_file_open(fileSystem, file, const_cast<const char*>(objectPathString.data()), flags);
	return lfsCreateFileStatus;
}

int32_t FileManagementService::pathIsValidForDeletion(String<ECSSMaxStringSize> repositoryString,
                                                      String<ECSSMaxStringSize> fileNameString) {

	if (FileManagementService::findWildcardPosition(repositoryString) != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	if (FileManagementService::findWildcardPosition(fileNameString) != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	const char* repositoryPathChar = repositoryString.data();
	String<ECSSMaxStringSize> objectPathString = "";
	objectPathString.append(repositoryPathChar);

	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileNameString.data());
	checkForSlashesAndCompensate(objectPathString, fileNameChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	if (objectPathString.size() > ECSSMaxStringSize) {
		return OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE;
	}

	lfs_info infoStruct;
	int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), &infoStruct);

	if (infoStructFillStatus >= LFS_ERR_OK) {
		switch (infoStruct.type) {
			case (LFS_TYPE_REG):

				return LFS_TYPE_REG;
				break;

			case (LFS_TYPE_DIR):

				return LFS_TYPE_DIR;
				break;

			default:

				return OBJECT_TYPE_IS_INVALID;
		}
	} else {
		return infoStructFillStatus;
	}
}

int32_t FileManagementService::littleFsDeleteFile(lfs_t* fs,
                                                  String<ECSSMaxStringSize> repositoryPath,
                                                  String<ECSSMaxStringSize> fileName) {

	const char* repositoryPathChar = repositoryPath.data();
	String<ECSSMaxStringSize> objectPathString = "";
	objectPathString.append(repositoryPathChar);

	auto* fileNameChar = reinterpret_cast<uint8_t*>(repositoryPath.data());
	checkForSlashesAndCompensate(objectPathString, fileNameChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	int32_t lfsDeleteFileStatus = lfs_remove(fs, objectPathString.data());
	return lfsDeleteFileStatus;
}

int32_t FileManagementService::littleFsReportFile(String<ECSSMaxStringSize> repositoryString,
                                                  String<ECSSMaxStringSize> fileNameString,
                                                  lfs_info* infoStruct) {

	const char* repositoryPathChar = repositoryString.data();
	String<ECSSMaxStringSize> objectPathString = "";
	objectPathString.append(repositoryPathChar);

	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileNameString.data());
	checkForSlashesAndCompensate(objectPathString, fileNameChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), infoStruct);
	if (infoStructFillStatus >= LFS_ERR_OK) {
		switch (infoStruct->type) {
			case (LFS_TYPE_REG):

				return LFS_TYPE_REG;

			case (LFS_TYPE_DIR):

				return LFS_TYPE_DIR;

			default:

				return OBJECT_TYPE_IS_INVALID;
		}
	} else {
		return infoStructFillStatus;
	}
}

void FileManagementService::createFile(Message& message) {

	message.assertTC(FileManagementService::ServiceType,
	                 FileManagementService::MessageType::CreateFile);

	String<ECSSMaxStringSize> repositoryPathString("");
	String<ECSSMaxStringSize> fileNameString("");

	if (getStringUntilZeroTerminator(message, repositoryPathString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (getStringUntilZeroTerminator(message, fileNameString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}

	uint16_t fileSizeBytes = message.readUint32();

	if (fileSizeBytes > MaxFileSizeBytes) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
		return;
	}

	switch (pathIsValidForCreation(repositoryPathString)) {
		case LFS_TYPE_DIR: {
			lfs_file_t file;
			int32_t createFileStatus = littleFsCreateFile(&onBoardFileSystemObject,
			                                              &file,
			                                              repositoryPathString,
			                                              fileNameString,
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

	message.assertTC(FileManagementService::ServiceType,
	                 FileManagementService::MessageType::DeleteFile);

	String<ECSSMaxStringSize> repositoryPathString("");
	String<ECSSMaxStringSize> fileNameString("");

	if (getStringUntilZeroTerminator(message, repositoryPathString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (getStringUntilZeroTerminator(message, fileNameString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}

	switch (pathIsValidForDeletion(repositoryPathString, fileNameString)) {
		case LFS_TYPE_REG: {
			if (littleFsDeleteFile(&onBoardFileSystemObject, repositoryPathString, fileNameString) >= LFS_ERR_OK) {
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

	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

	String<ECSSMaxStringSize> repositoryPathString("");
	String<ECSSMaxStringSize> fileNameString("");

	if (getStringUntilZeroTerminator(message, repositoryPathString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (getStringUntilZeroTerminator(message, fileNameString) != stringTerminatorFound) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (findWildcardPosition(repositoryPathString) != NO_WILDCARD_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if (findWildcardPosition(fileNameString) != NO_WILDCARD_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if ((repositoryPathString.size() + fileNameString.size()) > ECSSMaxStringSize) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}

	lfs_info infoStruct;

	switch (littleFsReportFile(repositoryPathString, fileNameString, &infoStruct)) {

		case (OBJECT_TYPE_IS_INVALID):

			ErrorHandler::reportError(message,
			                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
			break;

		case (LFS_TYPE_REG):

			fileAttributeReport(repositoryPathString, fileNameString, infoStruct.size);
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

void FileManagementService::fileAttributeReport(const String<ECSSMaxStringSize>& repositoryString,
                                                const String<ECSSMaxStringSize>& fileNameString,
                                                uint32_t fileSize) {
	Message report = createTM(MessageType::CreateAttributesReport);

	report.appendString(repositoryString);
	report.appendUint8('@');
	report.appendString(fileNameString);
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
