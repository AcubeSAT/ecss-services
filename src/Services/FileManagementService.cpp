#include "Services/FileManagementService.hpp"
#include <stdint.h>
#include "ErrorHandler.hpp"
#include "Helpers/lfs_stub.h"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "etl/vector.h"

int8_t FileManagementService::checkForWildcard(String<ECSSMaxStringSize> messageString) {
	auto* messageStringChar = reinterpret_cast<uint8_t*>(messageString.data());

	for (uint64_t currentChar = 0; currentChar < messageString.size(); currentChar++) {
		if (messageStringChar[currentChar] == FileManagementService::wildcard) {
			return currentChar;
		}
	}

	return NO_WILDCARD_FOUND;
}

uint8_t FileManagementService::getStringUntilZeroTerminator(Message& message,
                                                            String<ECSSMaxStringSize>& extractedString) {
	uint8_t charCounter = 0;
	char currentChar = static_cast<char>(message.readByte());

	while (currentChar != FileManagementService::variableStringTerminator) {

		if (charCounter == ECSSMaxStringSize - 1) {
			return STRING_TERMINATOR_NOT_FOUND;
		}

		extractedString.append(1, currentChar);
		charCounter++;
		currentChar = message.readByte();
	}

	return STRING_TERMINATOR_FOUND;
}

int32_t FileManagementService::pathIsValidForCreation(String<ECSSMaxStringSize> repositoryString) {
	lfs_info infoStruct;

	int8_t repositoryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);
	if (repositoryStringWildcardStatus != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	auto* repositoryStringChar = reinterpret_cast<uint8_t*>(repositoryString.data());
	int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject,
	                                        reinterpret_cast<const char*>(repositoryStringChar),
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

void FileManagementService::checkForSlashes(String<ECSSMaxStringSize>& objectPathString, uint8_t*& fileNameChar) {

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
                                                  int32_t flags) {

	if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
		return OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE;
	}

	int8_t fileNameWildcardStatus = FileManagementService::checkForWildcard(fileName);
	if (fileNameWildcardStatus != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	auto* repositoryPathChar = reinterpret_cast<uint8_t*>(repositoryPath.data());
	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileName.data());
	String<ECSSMaxStringSize> objectPathString = "";

	objectPathString.append(reinterpret_cast<const char*>(repositoryPathChar));
	checkForSlashes(objectPathString, fileNameChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	int32_t lfsCreateFileStatus = lfs_file_open(&onBoardFileSystemObject, file, objectPathString.data(), flags);
	return lfsCreateFileStatus;
}

int32_t FileManagementService::pathIsValidForDeletion(String<ECSSMaxStringSize> repositoryString,
                                                      String<ECSSMaxStringSize> fileNameString) {
	int8_t repositoyryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);
	if (repositoyryStringWildcardStatus != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	int8_t fileNameStringWildcardStatus = FileManagementService::checkForWildcard(fileNameString);
	if (fileNameStringWildcardStatus != NO_WILDCARD_FOUND) {
		return WILDCARD_FOUND;
	}

	auto* repositoryPathChar = reinterpret_cast<uint8_t*>(repositoryString.data());
	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileNameString.data());
	String<ECSSMaxStringSize> objectPathString = "";

	objectPathString.append(reinterpret_cast<const char*>(repositoryPathChar));
	checkForSlashes(objectPathString, fileNameChar);
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
	auto* repositoryPathChar = reinterpret_cast<uint8_t*>(repositoryPath.data());
	auto* fileNameChar = reinterpret_cast<uint8_t*>(fileName.data());
	String<ECSSMaxStringSize> objectPathString = "";

	objectPathString.append(reinterpret_cast<const char*>(repositoryPathChar));
	checkForSlashes(objectPathString, fileNameChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

	int32_t lfsDeleteFileStatus = lfs_remove(fs, objectPathString.data());
	return lfsDeleteFileStatus;
}

int32_t FileManagementService::littleFsReportFile(String<ECSSMaxStringSize> repositoryString,
                                                  String<ECSSMaxStringSize> fileNameString,
                                                  lfs_info* infoStruct) {

	auto* repositoryStringChar = reinterpret_cast<uint8_t*>(repositoryString.data());
	auto* fileNameStringChar = reinterpret_cast<uint8_t*>(fileNameString.data());
	String<ECSSMaxStringSize> objectPathString = "";

	objectPathString.append(reinterpret_cast<const char*>(repositoryStringChar));
	checkForSlashes(objectPathString, fileNameStringChar);
	objectPathString.append(reinterpret_cast<const char*>(fileNameStringChar));

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
	// TC[23,1]
	message.assertTC(FileManagementService::ServiceType,
	                 FileManagementService::MessageType::CreateFile);

	String<ECSSMaxStringSize> repositoryPathString("");
	uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

	String<ECSSMaxStringSize> fileNameString("");
	uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

	uint16_t fileSizeBytes = message.readUint32();

	if (repositoryPathExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (fileNameExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (fileSizeBytes > MAX_FILE_SIZE_BYTES) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
		return;
	} else {

		int32_t pathIsValidForCreationStatus = pathIsValidForCreation(repositoryPathString);
		switch (pathIsValidForCreationStatus) {
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
}

void FileManagementService::deleteFile(Message& message) {
	// TC[23,2]
	message.assertTC(FileManagementService::ServiceType,
	                 FileManagementService::MessageType::DeleteFile);

	String<ECSSMaxStringSize> repositoryPathString("");
	uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

	String<ECSSMaxStringSize> fileNameString("");
	uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

	if (repositoryPathExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (fileNameExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	} else {
		int32_t pathIsValidForDeletionStatus = pathIsValidForDeletion(repositoryPathString,
		                                                              fileNameString);
		switch (pathIsValidForDeletionStatus) {

			case LFS_TYPE_REG: {

				int32_t littleFsDeleteFileStatus = littleFsDeleteFile(&onBoardFileSystemObject, repositoryPathString,
				                                                      fileNameString);
				if (littleFsDeleteFileStatus >= LFS_ERR_OK) {
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
}

void FileManagementService::reportAttributes(Message& message) {
	// TC[23,3]
	message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

	String<ECSSMaxStringSize> repositoryPathString("");
	uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

	String<ECSSMaxStringSize> fileNameString("");
	uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

	int8_t repositoyryStringWildcardStatus = checkForWildcard(repositoryPathString);
	int8_t fileNameStringWildcardStatus = checkForWildcard(fileNameString);

	if (repositoryPathExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (fileNameExtractionStatus != STRING_TERMINATOR_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	}
	if (repositoyryStringWildcardStatus != NO_WILDCARD_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if (fileNameStringWildcardStatus != NO_WILDCARD_FOUND) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
		return;
	}
	if ((repositoryPathString.size() + fileNameString.size()) > ECSSMaxStringSize) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
		return;
	} else {

		lfs_info infoStruct;

		int32_t reportFileStatus = littleFsReportFile(repositoryPathString,
		                                              fileNameString,
		                                              &infoStruct);

		switch (reportFileStatus) {

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
}

void FileManagementService::fileAttributeReport(const String<ECSSMaxStringSize>& repositoryString,
                                                const String<ECSSMaxStringSize>& fileNameString,
                                                uint32_t fileSize) {
	//TM[23,4]
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
			createFile(message); // TC[23,1]
			break;

		case DeleteFile:
			deleteFile(message); // TC[23,2]
			break;

		case ReportAttributes:
			reportAttributes(message); // TC[23,3]
			break;

		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}