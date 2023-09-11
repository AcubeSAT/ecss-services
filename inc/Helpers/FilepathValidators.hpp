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
} //namespace FilepathValidators
