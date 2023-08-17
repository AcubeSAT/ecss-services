#pragma once

#include <cstdint>
#include <etl/algorithm.h>
#include <etl/optional.h>
#include "Helpers/Filesystem.hpp"
#include "ECSS_Definitions.hpp"
#include "Services/FileManagementService.hpp"
#include "etl/String.hpp"

namespace FilepathValidators {
	/**
     * If a wildcard is encountered, then it returns its position in the string (starting from 0).
     * @param path : The path passed as a String.
     * @return An optional that either contains the position of the wildcard, or no value.
     */
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path) {
		size_t wildcardPosition = path.find(FileManagementService::Wildcard, 0);

		if (wildcardPosition == -1) {
			return {};
		}

		return wildcardPosition;
	}

	/**
     * The purpose of this function is to take care of the extraction process for the object path variable
     * Parses the message until a '@' is found. Then returns the actual string, excluding the '@' char
     * @param message : The message that we want to parse
     * @param extractedString : pointer to a Filesystem::Path that will house the extracted string
     * @return status of execution
     *  stringTerminatorFound: Successful completion,
     *  stringTerminatorNotFound: Error occurred
     */
	etl::optional<Filesystem::Path> getStringUntilTerminator(Message& message) {
		uint8_t charCounter = 0;
		Filesystem::Path extractedString = "";
		char currentChar = static_cast<char>(message.readByte());

		while (currentChar != FileManagementService::VariableStringTerminator) {
			if (charCounter == ECSSMaxStringSize - 1) {
				return etl::nullopt;
			}

			extractedString.append(1, currentChar);
			charCounter++;
			currentChar = static_cast<char>(message.readByte());
		}

		return extractedString;
	}

	/**
     * The purpose of this function is to check if the the strings that compose the object path (repository string and
     * file name string) are seperated with a slash "/" between them. If they are not seperated by one and only one
     * slash, then it modifies the object path accordingly.
     * There are 4 possible conditions :
     * 1) Both are slashes
     * 2) Only the last character of the repository path has
     * 3) Only the last character of the file name has
     * 4) None of the has a slash
     * @param objectPathString : String that will house the complete object path
     * @param fileNameString : String with the file name
     * @return -
     */
	void checkForSlashesAndCompensate(Filesystem::Path& objectPathString, uint8_t*& fileNameChar, Filesystem::Path& fullPath) {
		while (fullPath.find("//") != Filesystem::Path::npos) {
			fullPath.replace("//", '/');
		}
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

	/**
     * The purpose of this function is to check if the object path is valid for deletion
     * Checks if there is a file at the object path, does not contain any wildcards and if the
     * object's path size if less than ECSSMaxStringSize
     * @param repositoryString : String with the repository name
     * @param fileNameString : String with the file name
     * @return status of execution
     *  LFS_TYPE_DIR: Object is a directory,
     *  LFS_TYPE_REG: Object is a file,
     *  WILDCARD_FOUND: If there is a wildcard in the repository's or file's name path string
     *  OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE: Object path size is too large
     *  OBJECT_TYPE_IS_INVALID: Invalid object type
     *  Other negative code: lfs_stat returned error code
     */
	int32_t pathIsValidForDeletion(Filesystem::Path repositoryString,
	                               Filesystem::Path fileNameString) {

//		if (findWildcardPosition(repositoryString)) {
//			return WILDCARD_FOUND;
//		}
//
//		if (findWildcardPosition(fileNameString)) {
//			return WILDCARD_FOUND;
//		}
//
//		const char* repositoryPathChar = repositoryString.data();
//		Filesystem::Path objectPathString = "";
//		objectPathString.append(repositoryPathChar);
//
//		auto* fileNameChar = reinterpret_cast<uint8_t*>(fileNameString.data());
//		checkForSlashesAndCompensate(objectPathString, fileNameChar);
//		objectPathString.append(reinterpret_cast<const char*>(fileNameChar));
//
//		if (objectPathString.size() > ECSSMaxStringSize) {
//			return OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE;
//		}
//
//		lfs_info infoStruct;
//		int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), &infoStruct);
//
//		if (infoStructFillStatus >= LFS_ERR_OK) {
//			switch (infoStruct.type) {
//				case (LFS_TYPE_REG):
//
//					return LFS_TYPE_REG;
//					break;
//
//				case (LFS_TYPE_DIR):
//
//					return LFS_TYPE_DIR;
//					break;
//
//				default:
//
//					return OBJECT_TYPE_IS_INVALID;
//			}
//		} else {
//			return infoStructFillStatus;
//		}
	}

	/**
     * The purpose of this function is to initiate a creation of a file using littleFs
     * @param fileSystem : Pointer to the file system struct
     * @param file : Pointer to the file struct
     * @param repositoryPath : The repository path
     * @param fileName : The file name
     * @param flags : Input flags that determines the creation status
     * @return status of execution
     *  OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE : File's object path name is too large,
     *  WILDCARD_FOUND: there is a wildcard in the repository's path string,
     *  lfs_open_file status: Status of the lfs function that creates a file
     */
	int32_t littleFsCreateFile(void* fileSystem,
	                           void* file,
	                           Filesystem::Path repositoryPath,
	                           Filesystem::Path fileName,
	                           const int32_t flags) {

//		if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize) {
//			return OBJECT_PATH_LARGER_THAN_ECSS_MAX_STRING_SIZE;
//		}
//
//		if (FileManagementService::findWildcardPosition(fileName) != NO_WILDCARD_FOUND) {
//			return WILDCARD_FOUND;
//		}
//
//		char* const repositoryPathChar = repositoryPath.data();
//		Filesystem::Path objectPathString = "";
//		objectPathString.append(repositoryPathChar);
//
//		auto* fileNameChar = reinterpret_cast<uint8_t*>(fileName.data());
//		checkForSlashesAndCompensate(objectPathString, fileNameChar);
//
//		objectPathString.append(reinterpret_cast<const char*>(fileNameChar));
//
//		int32_t lfsCreateFileStatus = lfs_file_open(fileSystem, file, const_cast<const char*>(objectPathString.data()), flags);
//		return lfsCreateFileStatus;
	}

	/**
     * The purpose of this function is to initiate the deletion of a file using littleFs
     * Checks the type of the object, the existence of wildcards in its name and it's path name size
     * @param fs : Pointer to the file system struct
     * @param repositoryPath : The repository path
     * @param fileName : The file name
     * @return lfs_remove status of execution
     */
	int32_t littleFsDeleteFile(void* fs, Filesystem::Path& repositoryPath, const Filesystem::Path& fileName) {

		const char* repositoryPathChar = repositoryPath.data();
		Filesystem::Path objectPathString = "";
		objectPathString.append(repositoryPathChar);

		auto* fileNameChar = reinterpret_cast<uint8_t*>(repositoryPath.data());
		checkForSlashesAndCompensate(objectPathString, fileNameChar);
		objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

//		int32_t lfsDeleteFileStatus = lfs_remove(fs, objectPathString.data());
//		return lfsDeleteFileStatus;
	}

	/**
     * The purpose of this function is to initiate the lfs_stat function, which will fill the info struct with all
     * the necessary information about a file's report.
     * Checks the type of object by means of it's type. If it is not LFS_TYPE_REG, then there is an issue.
     * @param repositoryString : String with the repository name
     * @param fileNameString : String with the file name
     * @param infoStruct : lfs_info which will house the file's attributes
     * @return status of execution
     *   OBJECT_TYPE_IS_INVALID invalid object type,
     *   LFS_TYPE_REG: Object is a file,
     *   LFS_TYPE_DIR: Object is a directory
     *   Any error code that lfs_stat might return)
     */
	int32_t littleFsReportFile(Filesystem::Path repositoryString,
	                           Filesystem::Path fileNameString,
	                           void* infoStruct) {

		const char* repositoryPathChar = repositoryString.data();
		Filesystem::Path objectPathString = "";
		objectPathString.append(repositoryPathChar);

		auto* fileNameChar = reinterpret_cast<uint8_t*>(fileNameString.data());
		checkForSlashesAndCompensate(objectPathString, fileNameChar);
		objectPathString.append(reinterpret_cast<const char*>(fileNameChar));

//		int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), infoStruct);
//		if (infoStructFillStatus >= LFS_ERR_OK) {
//			switch (infoStruct->type) {
//				case (LFS_TYPE_REG):
//
//					return LFS_TYPE_REG;
//
//				case (LFS_TYPE_DIR):
//
//					return LFS_TYPE_DIR;
//
//				default:
//
//					return OBJECT_TYPE_IS_INVALID;
//			}
//		} else {
//			return infoStructFillStatus;
//		}
	}

} //namespace FilepathValidators
