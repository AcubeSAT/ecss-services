#include "stdint.h"
#include "Services/FileManagementService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "../../lib/littlefs/lfs.h"

// Global lfs struct
lfs_t fs1;

#define REPOSITORY_SUMMARY_REPORT_MAX_OBJECTS 4096

int32_t FileManagementService::checkForWildcard(String<ECSS_MAX_STRING_SIZE> messageString)
{
    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto *messageStringChar = reinterpret_cast<uint8_t *>(messageString.data());

    // Check for wildcards in repositoryStringChar
    for (uint8_t currentChar = 0; currentChar < messageString.size(); currentChar++)
    {
        // Iterate over the string
        if (messageStringChar[currentChar] == FileManagementService::wildcard)
        {
            // Return the position of the index
            return currentChar;
        }
    }

    // Return status code -10 if there is no wildcard
    return -10;
}

uint8_t wildcardStringMatch(char *line, char *pattern) {
    // Wildcard flag
    uint8_t wildcard = 0;

    // Start iterating over the pattern's and line's chars
    do {
        // Check if the pattern char matches the string's char
        if (*pattern == *line) {
            // Increment both pointers to the next char
            line++;
            pattern++;
        } else if (*pattern == FileManagementService::wildcard) {
            // Check if the next char of the pattern is the null terminator (ex "test.tx*\0")
            // Increment the pattern counter in order to show the next char
            if (*(++pattern) == '\0') {
                return 1;
            }

            // Activate the wildcard flag
            wildcard = 1;
        } else if (wildcard) {
            // Check if the pattern's next character after the wildcard is matched with the string's char
            if (*line == *pattern) {
                // De-activate the wildcard flag and increment the pointers
                wildcard = 0;
                line++;
                pattern++;
            } else {
                line++;
            }
        } else {
            return 0;
        }
    } while (*line);

    // Check if the whole pattern has been scanned
    if (*pattern == '\0') {
        return 1;
    } else {
        return 0;
    }
}

uint8_t
FileManagementService::getStringUntilZeroTerminator(Message &message, String<ECSS_MAX_STRING_SIZE> &extractedString)
{

    char currentChar = '0';
    uint8_t charCounter = 0;
    currentChar = (char) message.readByte();

    // Increment the counter until '@' is reached
    while (currentChar != '@')
    {

        // Check if size is below the maximum allowed
        if (charCounter == ECSS_MAX_STRING_SIZE)
        {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
            return 1; // This should be interpreted as error code
        }

        // Pass the char byte and increment the size of the string
        extractedString.append(1, currentChar);
        charCounter++;

        // Increment the char pointer and read next byte
        currentChar = message.readByte();
    }

    return 0; // This should be interpreted as ok code
}

int32_t FileManagementService::pathIsValidForCreation(String<ECSS_MAX_STRING_SIZE> repositoryString)
{
    lfs_info infoStruct;

    // Check if there are any wildcards in the repositoryString
    int8_t repositoyryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);

    if (repositoyryStringWildcardStatus != -10)
    {
        // Return error code which indicates that there is a wildcard in the repository's path string
        return -1;
    }

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto *repositoryStringChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Call lfs_stat in order to fill the info_struct with data
    int32_t infoStructFillStatus = lfs_stat(&fs1, (const char *) repositoryStringChar, &infoStruct);

    // Check if the repository exists and no errors are produced during the lfs_stat() execution
    if (infoStructFillStatus >= 0)
    {
        // Check if the object at the end of the path is a file or a repository
        if (infoStruct.type == LFS_TYPE_DIR)
        {
            // The object is a directory
            return LFS_TYPE_DIR;
        }
        else if (infoStruct.type == LFS_TYPE_REG)
        {
            // The object is a file
            return LFS_TYPE_REG;
        }
        else
        {
            // Info type is invalid
            return -2;
        }
    }
    else
    {
        // lfs_stat() returned an error code
        return infoStructFillStatus;
    }
}

void getNumberOfBytesUntillZeroTerminator(char *characterArray, uint8_t &size)
{
    while (*characterArray != '\0')
    {
        characterArray++;
        size++;
    }
}

void checkForSlashes(String<ECSS_MAX_STRING_SIZE> &objectPathString, uint8_t *fileNameChar)
{
    // Last character of the repository path string
    char lastPathCharacter = objectPathString.back();
    // First character of the file name string
    char firstFileCharacter = *fileNameChar;

    // Check all possible conditions for the existence or not, of slashes
    // There are 4 possible conditions :
    // a) Both are slashes
    // b) Only the last character of the repository path has
    // c) Only the last character of the file name has
    // d) None of the has a slash
    if(lastPathCharacter == '/')
    {
        if(firstFileCharacter == '/')
        {
            // Condition a). Just increment the file name pointer
            fileNameChar++;
        }
        else
        {
            // Condition b). Leave as is.
        }
    }
    else
    {
        if(firstFileCharacter == '/')
        {
            // Condition c). Leave as is.
        }
        else
        {
            // Condition d). Add a slash between the 2 strings.
            objectPathString.append(1,'/');
        }
    }
}

int32_t
FileManagementService::littleFsCreateFile(lfs_t *fs, lfs_file_t *file, String<ECSS_MAX_STRING_SIZE> repositoryPath,
                                          String<ECSS_MAX_STRING_SIZE> fileName, int32_t flags)
{

    // Check the size of the object path
    if ((repositoryPath.size() + fileName.size()) > ECSS_MAX_STRING_SIZE)
    {
        // Return error code that indicates too large object path
        return -1;
    }

    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryPath.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileName.data());

    // String that will house the object path (repository path + file name)
    String<ECSS_MAX_STRING_SIZE> objectPathString = "";

    // Append the repository path
    objectPathString.append((const char *)repositoryPathChar);

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append((const char *)fileNameChar);

    // Create the file using lfs_file_open with the appropriate flags
    int32_t lfsCreateFileStatus = lfs_file_open(fs, file, objectPathString.data(), flags);

    return lfsCreateFileStatus;
}



int32_t FileManagementService::pathIsValidForDeletion(String<ECSS_MAX_STRING_SIZE> repositoryString,
                                                      String<ECSS_MAX_STRING_SIZE> fileNameString)
{
    // Check if there are any wildcards in the repositoryString
    int8_t repositoyryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);

    if (repositoyryStringWildcardStatus != -10) {
        // Return error code which indicates that there is a wildcard in the repository's path string
        return -1;
    }

    // Check if there are any wildcards in the fileNameString
    int8_t fileNameStringWildcardStatus = FileManagementService::checkForWildcard(fileNameString);

    if (fileNameStringWildcardStatus != -10)
    {
        // Return error code which indicates that there is a wildcard in the file's name string
        return -2;
    }

    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileNameString.data());

    // String that will house the object path (repository path + file name)
    String<ECSS_MAX_STRING_SIZE> objectPathString = "";

    // Append the repository path
    objectPathString.append((const char *)repositoryPathChar);

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append((const char *)fileNameChar);

    // Check for objectPath size
    if (objectPathString.size() > ECSS_MAX_STRING_SIZE )
    {
        // Return error code which indicates that the object path size is too large
        return -3;
    }

    // Info struct, that will house the type of the requested object
    lfs_info infoStruct;

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&fs1, objectPathString.data(), &infoStruct);

    // Check if the lfs_stat is completed successfully
    if (infoStructFillStatus >= 0)
    {
        // Check if the object is a file or a directory
        switch (infoStruct.type)
        {
            case (LFS_TYPE_REG):

                // Object type is file
                return LFS_TYPE_REG;
                break;

            case (LFS_TYPE_DIR):

                // Object type is directory
                return LFS_TYPE_DIR;
                break;

            default:

                // Return error, invalid object type (in case the return is other than those above)
                return -4;
        }
    }
    else
    {
        // lfs_stat returned an error code
        return infoStructFillStatus;
    }

}

int32_t FileManagementService::littleFsDeleteFile(lfs_t *fs, String<ECSS_MAX_STRING_SIZE> repositoryPath,
                                                  String<ECSS_MAX_STRING_SIZE> fileName)
{
    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryPath.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileName.data());

    // String that will house the object path (repository path + file name)
    String<ECSS_MAX_STRING_SIZE> objectPathString = "";

    // Append the repository path
    objectPathString.append((const char *)repositoryPathChar);

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append((const char *)fileNameChar);

    // Call lfs_remove
    int32_t lfsDeleteFileStatus = lfs_remove(fs, objectPathString.data());

    // Return status of deletion for the lfs function
    return lfsDeleteFileStatus;

}

int32_t FileManagementService::pathIsValidForARepository(String<ECSS_MAX_STRING_SIZE> repositoryString, uint8_t repositoryStringSize) {
    lfs_info *pInfo;
    lfs_info infoStruct;
    pInfo = &infoStruct;

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto *repositoryStringChar = reinterpret_cast<const char *>(repositoryString.data());

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&fs1, repositoryStringChar, pInfo);

    // Check if the lfs_stat is completed successfully
    if (infoStructFillStatus >= 0)
    {
        // Check what type of object is found
        if (infoStruct.type == LFS_TYPE_DIR)
        {
            // The object is a directory
            return 0;
        }
        else
        {
            // The object is a file
            return 1;
        }
    }
    else
    {
        // Return error code that there is no object
        return 2;
    }

}

int32_t FileManagementService::littleFsReportFile(String<ECSS_MAX_STRING_SIZE> repositoryString, uint8_t repositoryStringSize,
                           String<ECSS_MAX_STRING_SIZE> fileNameString, uint8_t fileNameStringSize,
                           lfs_info *infoStruct) {

    // Copy the repositoryString to a char array, in order to concatenate in one string
    auto *repositoryStringChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Copy the repositoryString to a char array, in order to concatenate in one string
    auto *fileNameStringChar = reinterpret_cast<uint8_t *>(fileNameString.data());

    // Concatenate 2 strings in 1. From now on use objectPathString
    String<ECSS_MAX_STRING_SIZE> objectPathString = "";
    objectPathString.append((const char *)repositoryStringChar);
    objectPathString.append((const char *)fileNameStringChar);

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&fs1, objectPathString.data(), infoStruct);

    if (infoStructFillStatus >= 0) {
        // Check if the object is a file or a directory
        switch (infoStruct->type) {
            case (LFS_TYPE_REG):

                // Object type is file
                return LFS_TYPE_REG;

            case (LFS_TYPE_DIR):

                // Object type is directory
                return LFS_TYPE_DIR;

                /*case (LFS_ERR_NOENT):

                    // There is no file in this path
                    return LFS_ERR_NOENT;
                */
            default:

                // Return error, invalid object type (in case the return is other than those above)
                return LFS_ERR_INVAL;
        }
    } else {
        // LittleFs generated error
        return -4;
    }

}

void FileManagementService::createFile(Message &message)
{
    // TC[23,1]
    // TODO implement fileLockedStatus but later as an extra feature
    message.assertTC(FileManagementService::ServiceType,
                     FileManagementService::MessageType::CreateFile);

    // Extract the repository path, which is the first string in this message
    String<ECSS_MAX_STRING_SIZE> repositoryPathString("");
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

    // Extract the file name, which is the second string in this message
    String<ECSS_MAX_STRING_SIZE> fileNameString("");
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

    // Extract the file size in bytes
    uint16_t fileSizeBytes = message.readUint32();

    // Check the repository name extraction status
    if (repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    // Check the file name extraction status
    else if (fileNameExtractionStatus != 0)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    //Check the validity of the request at service level
    else if (fileSizeBytes > MAX_FILE_SIZE_BYTES)
    {
        //Size of file out of bounds
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
    }

    // Check if the path is valid for creation of a file
    else
    {

        int32_t pathIsValidForCreationStatus = pathIsValidForCreation(repositoryPathString);

        // Check the status of the above function
        switch (pathIsValidForCreationStatus)
        {
            // Repository path leads to a repository, so proceed with the file creation
            case LFS_TYPE_DIR:
            {
                // Declare the lfs_file_t object
                lfs_file_t file;

                // Call littleFsCreateFile in order to create the file
                int32_t createFileStatus = littleFsCreateFile(&fs1, &file, repositoryPathString,
                                                              fileNameString,LFS_O_CREAT);
                // Check the status of the above function
                if (createFileStatus >= 0)
                {
                    // Calling lfs_file_close to release any allocated resources
                    if (lfs_file_close(&fs1, &file) >= 0)
                    {
                        // Successful file creation
                    }
                    else
                    {
                        // LittleFs lfs_file_close() generated error
                        ErrorHandler::reportError(message,
                                                  ErrorHandler::ExecutionCompletionErrorType::LittleFsFileCloseFailed);
                    }
                }
                else if(createFileStatus == -1)
                {
                    // Object's path size if greater that ECSS_MAX_STRING_SIZE
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
                }
                else if(createFileStatus == LFS_ERR_EXIST)
                {
                    // File already exists, so the creation was not successful
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::FileAlreadyExists);
                }
                else
                {
                    // LittleFs lfs_file_open() generated error
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::LittleFsFileOpenFailed);
                }
                break;
            }

            case LFS_TYPE_REG:

                // Repository path leads to a file, not a repository
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            case(-1):

                // The repository's path contains a wildcard
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
                break;

            case(-2):

                // The object's type is unexpected
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedFileType);
                break;

            default:

                // lfs_stat (called from pathIsValidForCreation() ) return an error code
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
                break;
        }
    }
}



void FileManagementService::deleteFile(Message &message)
{
    // TC[23,2]
    message.assertTC(FileManagementService::ServiceType,
                     FileManagementService::MessageType::DeleteFile);

    // Extract the repository path, which is the first string in this message
    String<ECSS_MAX_STRING_SIZE> repositoryPathString("");
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

    // Extract the file name, which is the second string in this message
    String<ECSS_MAX_STRING_SIZE> fileNameString("");
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

    // Check the repository name extraction status
    if (repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    // Check the file name extraction status
    else if (fileNameExtractionStatus != 0)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    // Check the validity of the object path
    else
    {
        // Check for wildcards, size and littleFs errors.
        int32_t pathIsValidForDeletionStatus = pathIsValidForDeletion(repositoryPathString,
                                                                      fileNameString);

        // Check the status of the above function
        switch (pathIsValidForDeletionStatus)
        {

            case LFS_TYPE_REG:
            {
                // The object is a file, so proceed with the deletion process
                int32_t littleFsDeleteFileStatus = littleFsDeleteFile(&fs1, repositoryPathString,
                                                                      fileNameString);

                // Check the status of the deletion
                if (littleFsDeleteFileStatus >= 0)
                {
                    // File was deleted successfully
                }
                else
                {
                    // LittleFs lfs_remove() failed
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::LittleFsRemoveFailed);
                }
                break;
            }

            case LFS_TYPE_DIR:

                // The object's type is a directory, so it can't be removed by this TC, which is only used for files
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            case(-1):

                // The repository's path contains a wildcard
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
                break;

            case(-2):

                // The file's name contains a wildcard
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
                break;

            case(-3):

                // The object's path exceeds ECSS_MAX_STRING_SIZE
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
                break;

            case(-4):

                // The object's type is unexpected
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedFileType);
                break;

            default:

                // lfs_stat (called from pathIsValidForDeletion() ) return an error code
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
                break;

        }
    }
}
/*
void FileManagementService::reportAttributes(Message &message) {
    // TC[23,3]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *) repositoryPath, repositoryPathSize);

    // Extract the file name, which is the second string in this message
    char fileName[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameSize = 0;
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileName, fileNameSize);
    String<ECSS_MAX_STRING_SIZE> fileNameString((uint8_t *) fileName, fileNameSize);


    // Extract, if any, wildcards in the repositoryString
    int32_t repositoyryStringWildcardStatus = checkForWildcard(repositoryPathString, repositoryPathSize);

    //  Extract, if any, wildcards in the fileNameString
    int32_t fileNameStringWildcardStatus = checkForWildcard(fileNameString, fileNameSize);

    // Check the repository name extraction status
    if (repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

        // Check the file name extraction status
    else if (fileNameExtractionStatus != 0)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

        // Check if there are any wildcard in the repository path
    else if (repositoyryStringWildcardStatus != -10)
    {
        // There is a wildcard in the repository path
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
    }

        // Check if there are any wildcard in the file name
    else if (fileNameStringWildcardStatus != -10)
    {
        // There is a wildcard in the file name
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
    }

        // Check if the concatenated size of the object path is valid
    else if ((repositoryPathSize + fileNameSize) > ECSS_MAX_STRING_SIZE)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    // Check the validity of the request at service level
    else
    {
        // Struct that will store the file information
        lfs_info infoStruct;

        // Fill infoStruct with the file's information using lfs_stat
        int32_t reportFileStatus = littleFsReportFile(repositoryPathString, repositoryPathSize, fileNameString,
                                                      fileNameSize, &infoStruct);

        // Handle each possible outcome
        switch (reportFileStatus) {

            case (-4):

                // There is no object with this path
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
                break;

            case (1):

                // Create a TM[23,4] fileAttributeReport
                fileAttributeReport(repositoryPathString, repositoryPathSize, fileNameString, fileNameSize, infoStruct);
                break;

            case (2):

                // Object is a directory
                // TODO : Define steps in reportProgressError (3rd argument). Logical wrong littlefs' response can be 3.
                ErrorHandler::reportProgressError(message,
                                          ErrorHandler::ExecutionProgressErrorType::UnknownExecutionProgressError, 3);
                break;

            default:

                // Unknown error
                // TODO : Possibly more error messages depending on littlefs return types
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
                break;
        }
    }
}

void FileManagementService::fileAttributeReport(String<ECSS_MAX_STRING_SIZE> repositoryString,
                                                uint8_t repositoryStringSize,
                                                String<ECSS_MAX_STRING_SIZE> fileNameString,
                                                uint8_t fileNameStringSize, lfs_info infoStruct) {
    //TM[23,4]
    Message report = createTM(MessageType::ReportAttributes);

    // Append the repository string and then the file name string
    report.appendString(repositoryString);
    //report.appendOctetString(repositoryString);
    report.appendString(fileNameString);
    //report.appendString(repositoryString);
    //report.appendString(fileNameString);

    // Append the size of the file
    report.appendSint16((int16_t)infoStruct.size);

    // Store the TM
    storeMessage(report);
}



*/
/*
void FileManagementService::findFile(Message &message) {
    // TC[23,7]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::FindFile);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *) repositoryPath, repositoryPathSize);

    // Extract the search pattern, which is the second string in this message
    char searchPattern[ECSS_MAX_STRING_SIZE];
    uint8_t searchPatternSize = 0;
    uint8_t searchPatternExtractionStatus = getStringUntilZeroTerminator(message, searchPattern, searchPatternSize);
    String<ECSS_MAX_STRING_SIZE> searchPatternString((uint8_t *) searchPattern, searchPatternSize);

    // Extract the wildcard position in the repository path if any.
    // If there is not a wildcard in the repository path, then return -10.
    int32_t repositoryPathWildcardPositionIndex = checkForWildcard(repositoryPathString, repositoryPathSize);

    // Extract the wildcard position in the search pattern if any.
    // If there is not a wildcard in the search pattern, then return -10.
    int32_t searchPatternWildcardPositionIndex = checkForWildcard(searchPatternString, searchPatternSize);

    // Check the repository name extraction status
    if (repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }
    // Check the search pattern extraction status
    else if (searchPatternExtractionStatus != 0)
    {
        // Size of search pattern is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }
    // Check if there is a wildcard in the repositoryPath
    else if (repositoryPathWildcardPositionIndex != -10)
    {
        // Report failed start of execution notification
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
    }
        // Check if there is a wildcard in the search pattern
    else if (searchPatternWildcardPositionIndex == -10)
    {
        // Info struct to store the files attributes
        lfs_info info_struct;

        // Report the requested object if it exists
        int32_t extractObjectFindStatus = littleFsReportFile(repositoryPathString, repositoryPathSize,
                                                             searchPatternString,
                                                             searchPatternSize, &info_struct);

        // Check the underlying file system response
        if (extractObjectFindStatus == LFS_ERR_NOENT)
        {
            // No file found in this path
            // TODO : What to do with report.numberOfFiles ?
            FoundFilesReportStruct report;
            strcpy(report.repositoryPath, reinterpret_cast<char *>(repositoryPathString.data()));
            strcpy(report.searchPattern, reinterpret_cast<char *>(searchPatternString.data()));
            strcpy(report.filePath, reinterpret_cast<char *>(0));
            report.numberOfFiles = 0;

            // Call TM[23,8]
            foundFileReport(report);
        }
        else if ((extractObjectFindStatus == LFS_TYPE_DIR) || (extractObjectFindStatus == LFS_TYPE_REG))
        {
            // An object found at this path
            FoundFilesReportStruct report;
            strcpy(report.repositoryPath, reinterpret_cast<char *>(repositoryPathString.data()));
            strcpy(report.searchPattern, reinterpret_cast<char *>(searchPatternString.data()));
            strcpy(report.filePath, reinterpret_cast<char *>(searchPatternString.data()));
            report.numberOfFiles = 1;

            // Call TM[23,8]
            foundFileReport(report);
        }
        else
        {
            // Report failed completion of execution
            ErrorHandler::reportError(message,
                                      ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        // Lfs struct for directory information
        lfs_dir info_directory;

        // Check if the repository exists
        int32_t repositoryExists = pathIsValidForARepository(repositoryPathString, repositoryPathSize);

        if (repositoryExists == 0)
        {
            // Open the requested directory
            int32_t lfsDirectoryOpenStatus = lfs_dir_open(&fs1, &info_directory,
                                                          reinterpret_cast<const char *>(repositoryPathString.data()));

            // Check the status of the above operation
            if (lfsDirectoryOpenStatus == 0)
            {
                // Info struct for every object found in the requested repository
                lfs_info info_struct;

                // Report struct
                FoundFilesReportStruct report;
                strcpy(report.repositoryPath, reinterpret_cast<const char *>(repositoryPathString.data()));
                strcpy(report.searchPattern, reinterpret_cast<const char *>(searchPatternString.data()));

                // String to be filled with filepaths
                String<ECSS_MAX_STRING_SIZE> filePaths((""));

                // Go through all the objects in the repository and store their information in the info_struct
                // TODO : Maybe an approach with a timeout instead of a while(1) is more appropriate
                while (1)
                {
                    // Read the next entry in the repository
                    int32_t lfsDirectoryReadStatus = lfs_dir_read(&fs1, &info_directory, &info_struct);

                    // Check if there was an error during the read operation
                    if (lfsDirectoryReadStatus < 0)
                    {
                        // Report failed completion of execution
                        ErrorHandler::reportError(message,
                                                  ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
                        break;
                    }
                        // Check if there are no other entries in the repository
                    else if (lfsDirectoryReadStatus == 0)
                    {
                        // Exit the for loop
                        break;
                    }
                        // For every entry found, match the name with the requested pattern
                    else
                    {
                        // Check if there is a match with a file
                        if (wildcardStringMatch(info_struct.name,
                                                reinterpret_cast<char *>(searchPatternString.data())) == 1)
                        {
                            // Find the actual size of the file's name
                            uint8_t fileNameSize = 0;
                            getNumberOfBytesUntillZeroTerminator(reinterpret_cast<char *>(info_struct.name),
                                                                 fileNameSize);

                            // TODO check for truncation

                            // Put it in the filePath character array
                            filePaths.append((const char *) info_struct.name, fileNameSize);
                            filePaths.append((const char *) "@", 1);

                            // Increment the numberOfFiles counter
                            report.numberOfFiles++;
                        }
                    }
                }
                // Append the filePaths to the report Struct
                strcpy(report.filePath, reinterpret_cast<const char *>(filePaths.data()));

                // Call TM[23,8]
                foundFileReport(report);
            }
            else
            {
                // Report failed start of execution due to inability to open the requested repository
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
            }
        }
        else
        {
            // Report failed start of execution due to invalid repository path
            ErrorHandler::reportError(message,
                                      ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
}

void FileManagementService::foundFileReport(FoundFilesReportStruct filesReport) {
    //TM [23,8]
    Message report = createTM(FoundFileReport);

    // Those can be avoided if the FoundFilesReportStruct's variables are converteed to Strings

    uint8_t repositoryPathSize = 0;
    uint8_t searchPatternSize = 0;
    uint8_t filePathsSize = 0;

    // TODO : Check initialization of the strings. After the actual message, there is a chance that garbage other than
    // TODO : 0 will appear, so the size of the string wiil be different.
    getNumberOfBytesUntillZeroTerminator(reinterpret_cast<char *>(filesReport.repositoryPath), repositoryPathSize);
    getNumberOfBytesUntillZeroTerminator(reinterpret_cast<char *>(filesReport.searchPattern), searchPatternSize);
    getNumberOfBytesUntillZeroTerminator(reinterpret_cast<char *>(filesReport.filePath), filePathsSize);

    String<ECSS_MAX_STRING_SIZE> repositoryPath(reinterpret_cast<uint8_t *>(filesReport.repositoryPath),
                                                repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> searchPattern(reinterpret_cast<uint8_t *>(filesReport.searchPattern),
                                               searchPatternSize);
    String<ECSS_MAX_STRING_SIZE> filePaths(reinterpret_cast<uint8_t *>(filesReport.filePath), filePathsSize);

    report.appendString(repositoryPath);
    report.appendUint8((uint8_t)variableStringTerminator);
    report.appendString(searchPattern);
    report.appendUint8((uint8_t)variableStringTerminator);
    report.appendUint32(filesReport.numberOfFiles);
    report.appendString(filePaths);
    report.appendUint8((uint8_t)variableStringTerminator);

}

void FileManagementService::createDirectory(Message &message) {
    // TC[23,9]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *) repositoryPath, repositoryPathSize);

    // Extract the directory name, which is the second string in this message
    char directoryName[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameSize = 0;
    uint8_t directoryNameExtractionStatus = getStringUntilZeroTerminator(message, directoryName, directoryNameSize);
    String<ECSS_MAX_STRING_SIZE> directoryNameString((uint8_t *) directoryName, directoryNameSize);

    // Check if the repository path is valid
    int32_t repositoryPathIsValid = pathIsValidForARepository(repositoryPathString, repositoryPathSize);

    // Check if the directory name has a wildcard in it
    int32_t directoryNameIsValid = FileManagementService::checkForWildcard(directoryNameString, directoryNameSize);

    // Check if the size of the object path (repository + directory path) is less than ECSS_MAX_STRING_SIZE
    uint8_t objectPathSizeOverflowStatus;

    if ((uint16_t) repositoryPathSize + (uint16_t) directoryNameSize < ECSS_MAX_STRING_SIZE)
    {
        objectPathSizeOverflowStatus = 0;
    }
    else
    {
        objectPathSizeOverflowStatus = 1;
    }

    // Check if both the repository path is valid AND the directory name has not a wildcard in it AND the object path
    // does not overflow ECSS_MAX_STRING_SIZE
    if ((repositoryPathIsValid == 0) && (directoryNameIsValid == -10) && (objectPathSizeOverflowStatus == 0)) {
        // Concatenate repository and directory String to repository string and use it from now on as the object path
        // TODO maybe we should check for slashes "/" and handle all the possible cases (ex "/" is missing, too many etc)
        repositoryPathString.append(directoryNameString);

        // Create the directory
        int32_t mkridReturnCode = lfs_mkdir(&fs1, reinterpret_cast<const char *>(repositoryPathString.data()));

        // Check if the Directory is created successfully
        if (mkridReturnCode < 0) {
            // Create failed completion of execution error
            ErrorHandler::reportError(message,
                                      ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        // Create failed start of execution error
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
}

void FileManagementService::deleteDirectory(Message &message) {
    //TC[23,10]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *) repositoryPath, repositoryPathSize);

    // Extract the directory name, which is the second string in this message
    char directoryName[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameSize = 0;
    uint8_t directoryNameExtractionStatus = getStringUntilZeroTerminator(message, directoryName, directoryNameSize);
    String<ECSS_MAX_STRING_SIZE> directoryNameString((uint8_t *) directoryName, directoryNameSize);

    // Check if the repository path is valid
    int32_t repositoryPathIsValid = pathIsValidForARepository(repositoryPathString, repositoryPathSize);

    // Check if the directory name has a wildcard in it
    int32_t directoryNameIsValid = FileManagementService::checkForWildcard(directoryNameString, directoryNameSize);

    // Check if the size of the object path (repository + directory path) is less than ECSS_MAX_STRING_SIZE
    uint8_t objectPathSizeOverflowStatus;

    if (repositoryPathSize + directoryNameSize < ECSS_MAX_STRING_SIZE)
    {
        objectPathSizeOverflowStatus = 0;
    }
    else
    {
        objectPathSizeOverflowStatus = 1;
    }

    // Info struct to store the repository's attributes
    lfs_dir info_directory;

    // Check repository path is valid AND directory name is valid AND object path does not overflow
    if ((repositoryPathIsValid == 0) && (directoryNameIsValid == 0) && (objectPathSizeOverflowStatus == 0))
    {
        // Open the requested directory
        int32_t lfsDirectoryOpenStatus = lfs_dir_open(&fs1, &info_directory,
                                                      reinterpret_cast<const char *>(repositoryPathString.data()));

        // Check the status of the above operation
        if (lfsDirectoryOpenStatus == 0)
        {
            // Info struct, just for argument for lfs_dir_open. No usage otherwise.
            lfs_info info_struct;

            // Check if the directory is empty
            // TODO : Maybe an approach with a timeout instead of a while(1) is more appropriate
            int32_t lfsDirectoryReadStatus = lfs_dir_read(&fs1, &info_directory, &info_struct);

            // If it is indeed empty
            if(lfsDirectoryReadStatus == 0)
            {
                // Delete the directory
                int32_t removeDirStatus = lfs_remove(&fs1, reinterpret_cast<const char *>(repositoryPathString.data()));

                // Check if the Directory is deleted successfully
                if (removeDirStatus < 0)
                {
                    // Create failed completion of execution error
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
                }
            }
            // Directory is not empty
            else if(lfsDirectoryReadStatus > 0)
            {
                // The directory which is requested to be deleted is not empty, hence can't be deleted
                ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
            }
            // The lfs_dir_read function failed
            else
            {
                // The lfs_dir_read couldn't be processed correctly
                // TODO : More than 1 reason's this can happen. Possibly out of scope so an error handler is an overkill
                // TODO : for this one
                ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
            }
        }
        else
        {
            // Report failed start of execution due to inability to open the requested repository
            ErrorHandler::reportError(message,
                                      ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        // Report failed start of execution due to inability to open the requested repository
        // TODO : There are many possible reasons why this can happen, needs addressing with an error handler (?) for
        // TODO : every one of the possible outcomes
        ErrorHandler::reportError(message,
                                  ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
    }
}

void FileManagementService::renameDirectory(Message &message) {
    //TC[23,11]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::RenameDirectory);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *) repositoryPath, repositoryPathSize);

    // Extract the old directory name, which is the second string in this message
    char oldDirectoryName[ECSS_MAX_STRING_SIZE];
    uint8_t oldDirectoryNameSize = 0;
    uint8_t oldDirectoryExtractionStatus = getStringUntilZeroTerminator(message, oldDirectoryName,
                                                                        oldDirectoryNameSize);
    String<ECSS_MAX_STRING_SIZE> oldDirectoryString((uint8_t *) oldDirectoryName, oldDirectoryNameSize);

    // Extract the new directory name, which is the third string in this message
    char newDirectoryName[ECSS_MAX_STRING_SIZE];
    uint8_t newDirectoryNameSize = 0;
    uint8_t newDirectoryExtractionStatus = getStringUntilZeroTerminator(message, newDirectoryName,
                                                                        newDirectoryNameSize);
    String<ECSS_MAX_STRING_SIZE> newDirectoryString((uint8_t *) newDirectoryName, newDirectoryNameSize);

    // Check if the repository path is valid
    int32_t repositoryPartIsValid = pathIsValidForARepository(repositoryPathString, repositoryPathSize);

    // Check if the old directory exists and if it is actually a repository
    // TODO maybe a check for wildcard can be added, but the presence of this check in creation and deletion actually
    // TODO guarantees that littleFsReportFile will fail even when the oldDirectoryString has a wildcard
    lfs_info oldDirectoryStruct;
    int32_t oldDirectoryStatus = littleFsReportFile(repositoryPathString, repositoryPathSize, oldDirectoryString,
                                                    oldDirectoryNameSize, &oldDirectoryStruct);

    // Check if the new directory name has no wildcards in it
    int32_t newDirectoryStatus = checkForWildcard(newDirectoryString, newDirectoryNameSize);

    // Check if the new repository + directory name size is less than ECSS_MAX_STRING_SIZE
    uint8_t objectPathNameSizeStatus;

    if ((uint16_t) repositoryPathSize + (uint16_t) newDirectoryNameSize < ECSS_MAX_STRING_SIZE)
    {
        objectPathNameSizeStatus = 0;
    }
    else
    {
        objectPathNameSizeStatus = 1;
    }

    // Check if all the conditions are matched, else generate a failed start of execution error
    if ((repositoryPartIsValid == 0) && (oldDirectoryStatus == 2) && (newDirectoryStatus == -10) &&
        (objectPathNameSizeStatus == 0))
    {
        // Create an new string to house the new directory string, with the repository prefix
        String<ECSS_MAX_STRING_SIZE> newDirectoryPathString(repositoryPathString);

        // TODO : Same addressing here must be needed for the concatenation of the strings (missing "/" etc)
        // Append the old path to the repository prefix in order to pass the into the lfs_remove function
        repositoryPathString.append(oldDirectoryString);

        // Append the new path to the repository prefix in order to pass the into the lfs_remove function
        newDirectoryPathString.append(newDirectoryString);

        // Call litleFs directory renaming function
        int32_t lfsRenameStatus = lfs_rename(&fs1, reinterpret_cast<const char *>(repositoryPathString.data()),
                                             reinterpret_cast<const char *>(newDirectoryPathString.data()));

        // Check if the littleFs function executed without any errors
        if (lfsRenameStatus < 0)
        {
            // An error was detected, create a failed completion of execution error
            // TODO : More than 1 reason's this can happen. Possibly out of scope so an error handler is an overkill
            // TODO : for this one
            ErrorHandler::reportError(message,
                                      ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        // TODO we can mask all the possible failures at this point an create different error reports
        // Create failed start of execution error
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
}


*/
/*
void FileManagementService::reportSummaryDirectory(Message &message)
{
    //TC[23,12]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportSummaryDirectory);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Check if the directory path is valid
    if (checkIfPathIsValid(repositoryPathString, repositoryPathCounter) == 1)
    {
        //Extract (if any) wildcards in the message
        wildcardPositionIndex = extractWildcardPositionIndex(repositoryPathString, repositoryPathCounter, wildcard);

        //Check if the wildcard is in a logical place (between the first and the last character)
        if((wildcardPositionIndex <= (repositoryPathCounter - 1) &&  (wildcardPositionIndex >= -1))
        {
            //Check if there are any wildcards in the directory path
            if (wildcardPositionIndex == -1)
            {
                //Fill the objectVector with information about the objects that are located under the regarding repository
                etl::vector<ReportSummaryDirectoryStruct, REPOSITORY_SUMMARY_REPORT_MAX_OBJECTS> objectVector;

                //Check if the filling is done successfully
                if(fillObjectVector(repositoryPath, &objectVector) == 1)
                {
                    summaryDirectoryReport(repositoryPath, objectVector);
                }
                else
                {
                    //Error occurred in the process
                    ErrorHandler::reportError(message,ErrorHandler::ExecutionProgressErrorType::UnknownExecutionProgressError);
                }

                //Pass the reportMessage to TM[23,13]
                summaryDirectoryReport(objectVector);
            }
            else
            {
                //There is a wildcard in the directory path
                //Create a failed start of execution error
                ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
            }
        }
        else
        {
            //Wildcard is out of bounds, create an execution progress error
            ErrorHandler::reportError(message,ErrorHandler::ExecutionProgressErrorType::UnknownExecutionProgressError);
        }
    }
    else
    {
        //Directory path in not valid
        //Create a failed start of execution error
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
    }

}

void FileManagementService::summaryDirectoryReport(char repositoryPath[ECSS_MAX_STRING_SIZE], etl::vector<ReportSummaryDirectoryStruct, REPOSITORY_SUMMARY_REPORT_MAX_OBJECTS> objectVector)
{
    //TM[23,13]
    Message report = createTM(FileManagementService::MessageType::SummaryDirectoryReport);

    // Get the number of objects
    uint8_t reportedObjectsSize = objectVector.size();

    // Put repositoryPath in the message
    // Here it should parse the char byte by byte and append them in the report
    report.appendByte();

    // Put reportedObjectsSize in the message
    report.appendUint8(reportedObjectsSize);

    // Loop through every object to get its type and name
    for(int vectorIndex = 0; vectorIndex < reportedObjectsSize; vectorIndex++)
    {
        // Append object type
        report.appendByte(objectVector[i].objectType);

        // Append object name
        // Here it should parse the char byte by byte and append them in the report
        report.appendByte();
    }
}

void FileManagementService::copyFile(Message& message)
{

    //TC[23,14]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile);

    // Extract operation id
    uint32_t operationId = message.readUint32();

    // Extract the repository path of the source file
    char SourceRepositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t SourceRepositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(SourceRepositoryPathString, SourceRepositoryPathCounter);

    // Extract the file name of the source file
    char SourceFileNameString[ECSS_MAX_STRING_SIZE];
    uint8_t SourceFileNameCounter = getStringUntilZeroTerminator(message);
    message.readString(SourceFileNameString, SourceFileNameCounter);

    // Extract the repository path of the target file
    char TargetRepositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t TargetRepositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(TargetRepositoryPathString, TargetRepositoryPathCounter);

    // Extract the file name of the target file
    char TargetFileNameString[ECSS_MAX_STRING_SIZE];
    uint8_t TargetFileNameCounter = getStringUntilZeroTerminator(message);
    message.readString(TargetFileNameString, TargetFileNameCounter);



}

*/
