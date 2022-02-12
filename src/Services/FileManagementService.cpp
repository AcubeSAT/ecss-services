#include <stdint.h>
#include "Services/FileManagementService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "../../lib/littlefs/lfs.h"

int8_t FileManagementService::checkForWildcard(String<ECSSMaxStringSize> messageString)
{
    // Copy the repositoryString to a char pointer, in order to use it in lfs_stat
    auto *messageStringChar = reinterpret_cast<uint8_t *>(messageString.data());

    // Check for wildcards in repositoryStringChar
    for (uint64_t currentChar = 0; currentChar < messageString.size(); currentChar++)
    {
        // Iterate over the string
        if (messageStringChar[currentChar] == FileManagementService::wildcard)
        {
            // Return the position of the index
            return currentChar;
        }
    }

    // Return status code -1 if there is no wildcard
    return -1;
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
        } else if (wildcard != 0U) {
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
    } while (*line != 0);

    // Check if the whole pattern has been scanned
    if (*pattern == '\0')
    {
        return 1;
    }

    // If not, return 0
    return 0;

}

uint8_t FileManagementService::getStringUntilZeroTerminator(Message                   &message,
                                                            String<ECSSMaxStringSize> &extractedString)
{
    // Counter which counts the size of the string
    uint8_t charCounter = 0;

    // Current character
    char currentChar = static_cast<char>(message.readByte());

    // Increment the counter until '@' is reached
    while (currentChar != FileManagementService::variableStringTerminator)
    {

        // Check if size is below the maximum allowed
        if (charCounter == ECSSMaxStringSize - 1)
        {
            // No string terminator found
            return 1;
        }

        // Pass the char byte and increment the size of the string
        extractedString.append(1, currentChar);
        charCounter++;

        // Increment the char pointer and read next byte
        currentChar = message.readByte();
    }

    return 0; // This should be interpreted as ok code
}

int32_t FileManagementService::pathIsValidForCreation(String<ECSSMaxStringSize> repositoryString)
{
    // Info struct that will house the necessary information for the repository
    lfs_info infoStruct;

    // Check if there are any wildcards in the repositoryString
    int8_t repositoryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);

    if (repositoryStringWildcardStatus != -1)
    {
        // Return error code which indicates that there is a wildcard in the repository's path string
        return -1;
    }

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto *repositoryStringChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Call lfs_stat in order to fill the info_struct with data
    int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject,
                                            reinterpret_cast<const char *>(repositoryStringChar),
                                            &infoStruct);

    // Check if the repository exists and no errors are produced during the lfs_stat() execution
    if (infoStructFillStatus >= 0)
    {
        // Check if the object at the end of the path is a file or a repository
        switch (infoStruct.type)
        {
            case LFS_TYPE_DIR:
                // The object is a directory
                return LFS_TYPE_DIR;
                break;

            case LFS_TYPE_REG:
                // The object is a file
                return LFS_TYPE_REG;
                break;

            default:
                // Info type is invalid
                return -2;
                break;
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
    while (*characterArray != '@')
    {
        characterArray++;
        size++;
    }
}

void FileManagementService::checkForSlashes(String<ECSSMaxStringSize> &objectPathString, uint8_t *&fileNameChar)
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
            fileNameChar = fileNameChar + 1;
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

int32_t FileManagementService::littleFsCreateFile(lfs_t                     *fileSystem,
                                                  lfs_file_t                *file,
                                                  String<ECSSMaxStringSize> repositoryPath,
                                                  String<ECSSMaxStringSize> fileName,
                                                  int32_t                   flags)
{

    // Check the size of the object path
    if ((repositoryPath.size() + fileName.size()) > ECSSMaxStringSize)
    {
        // Return error code that indicates too large object path
        return -1;
    }

    // Check if there are any wildcards in the file name
    int8_t fileNameWildcardStatus = FileManagementService::checkForWildcard(fileName);

    if (fileNameWildcardStatus != -1)
    {
        // Return error code which indicates that there is a wildcard in the repository's path string
        return -2;
    }

    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryPath.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileName.data());

    // String that will house the object path (repository path + file name)
    String<ECSSMaxStringSize> objectPathString = "";

    // Append the repository path
    objectPathString.append(reinterpret_cast<const char *>(repositoryPathChar));

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append(reinterpret_cast<const char *>(fileNameChar));

    // Create the file using lfs_file_open with the appropriate flags
    int32_t lfsCreateFileStatus = lfs_file_open(&onBoardFileSystemObject, file, objectPathString.data(), flags);

    return lfsCreateFileStatus;
}

int32_t FileManagementService::pathIsValidForDeletion(String<ECSSMaxStringSize> repositoryString,
                                                      String<ECSSMaxStringSize> fileNameString)
{
    // Check if there are any wildcards in the repositoryString
    int8_t repositoyryStringWildcardStatus = FileManagementService::checkForWildcard(repositoryString);

    if (repositoyryStringWildcardStatus != -1) {
        // Return error code which indicates that there is a wildcard in the repository's path string
        return -1;
    }

    // Check if there are any wildcards in the fileNameString
    int8_t fileNameStringWildcardStatus = FileManagementService::checkForWildcard(fileNameString);

    if (fileNameStringWildcardStatus != -1)
    {
        // Return error code which indicates that there is a wildcard in the file's name string
        return -2;
    }

    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileNameString.data());

    // String that will house the object path (repository path + file name)
    String<ECSSMaxStringSize> objectPathString = "";

    // Append the repository path
    objectPathString.append(reinterpret_cast<const char *>(repositoryPathChar));

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append(reinterpret_cast<const char *>(fileNameChar));

    // Check for objectPath size
    if (objectPathString.size() > ECSSMaxStringSize )
    {
        // Return error code which indicates that the object path size is too large
        return -3;
    }

    // Info struct, that will house the type of the requested object
    lfs_info infoStruct;

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), &infoStruct);

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

int32_t FileManagementService::littleFsDeleteFile(lfs_t *fs, String<ECSSMaxStringSize> repositoryPath,
                                                  String<ECSSMaxStringSize> fileName)
{
    // Pointer to repository path string data
    auto *repositoryPathChar = reinterpret_cast<uint8_t *>(repositoryPath.data());

    // Pointer to file name string data
    auto *fileNameChar = reinterpret_cast<uint8_t *>(fileName.data());

    // String that will house the object path (repository path + file name)
    String<ECSSMaxStringSize> objectPathString = "";

    // Append the repository path
    objectPathString.append(reinterpret_cast<const char *>(repositoryPathChar));

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameChar);

    // Append the file name
    objectPathString.append(reinterpret_cast<const char *>(fileNameChar));

    // Call lfs_remove
    int32_t lfsDeleteFileStatus = lfs_remove(fs, objectPathString.data());

    // Return status of deletion for the lfs function
    return lfsDeleteFileStatus;

}

int32_t FileManagementService::pathIsValidForARepository(String<ECSSMaxStringSize> repositoryString,
                                                         uint8_t                   repositoryStringSize)
{
    lfs_info *pInfo;
    lfs_info infoStruct;
    pInfo = &infoStruct;

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto *repositoryStringChar = reinterpret_cast<const char *>(repositoryString.data());

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, repositoryStringChar, pInfo);

    // Check if the lfs_stat is completed successfully
    if (infoStructFillStatus >= 0)
    {
        // Check what type of object is found
        switch (infoStruct.type)
        {
            case LFS_TYPE_DIR:
                // The object is a directory
                return 0;
                break;

            case LFS_TYPE_REG:
                // The object is a file
                return 1;
                break;

            default:
                // Invalid object type
                return 2;
                break;
        }
    }
    else
    {
        // Return error code that there is no object
        return 2;
    }

}

int32_t FileManagementService::littleFsReportFile(String<ECSSMaxStringSize> repositoryString,
                                                  String<ECSSMaxStringSize> fileNameString,
                                                  lfs_info                  *infoStruct)
{

    // Copy the repositoryString to a char array, in order to concatenate in one string
    auto *repositoryStringChar = reinterpret_cast<uint8_t *>(repositoryString.data());

    // Copy the repositoryString to a char array, in order to concatenate in one string
    auto *fileNameStringChar = reinterpret_cast<uint8_t *>(fileNameString.data());

    // Concatenate 2 strings in 1. From now on use objectPathString
    String<ECSSMaxStringSize> objectPathString = "";

    // Append the repository path string
    objectPathString.append(reinterpret_cast<const char *>(repositoryStringChar));

    // Check for the existence of slashes and adapt accordingly
    checkForSlashes(objectPathString, fileNameStringChar);

    // Append the file name string
    objectPathString.append(reinterpret_cast<const char *>(fileNameStringChar));

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&onBoardFileSystemObject, objectPathString.data(), infoStruct);

    if (infoStructFillStatus >= 0)
    {
        // Check if the object is a file or a directory
        switch (infoStruct->type)
        {
            case (LFS_TYPE_REG):

                // Object type is file
                return LFS_TYPE_REG;

            case (LFS_TYPE_DIR):

                // Object type is directory
                return LFS_TYPE_DIR;

            default:

                // Return error, invalid object type (in case the return is other than those above)
                return -1;
        }
    }
    else
    {
        // LittleFs generated error
        return infoStructFillStatus;
    }

}

void FileManagementService::createFile(Message &message)
{
    // TC[23,1]
    // TODO implement fileLockedStatus but later as an extra feature
    message.assertTC(FileManagementService::ServiceType,
                     FileManagementService::MessageType::CreateFile);

    // Extract the repository path, which is the first string in this message
    String<ECSSMaxStringSize> repositoryPathString("");
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

    // Extract the file name, which is the second string in this message
    String<ECSSMaxStringSize> fileNameString("");
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);

    // Extract the file size in bytes
    uint16_t fileSizeBytes = message.readUint32();

    // Check the repository name extraction status
    if (repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
        return;
    }

    // Check the file name extraction status
    if (fileNameExtractionStatus != 0)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
        return;
    }

    //Check the validity of the request at service level
    if (fileSizeBytes > MAX_FILE_SIZE_BYTES)
    {
        //Size of file out of bounds
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
        return;
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
                int32_t createFileStatus = littleFsCreateFile(&onBoardFileSystemObject,
                                                              &file,
                                                              repositoryPathString,
                                                              fileNameString,
                                                              LFS_O_CREAT);
                // Check the status of the above function
                if (createFileStatus >= 0)
                {
                    // Calling lfs_file_close to release any allocated resources
                    if (lfs_file_close(&onBoardFileSystemObject, &file) >= 0)
                    {
                        // Successful file creation
                        return;
                    }
                    else
                    {
                        // LittleFs lfs_file_close() generated error, during the synchronization of the file system
                        ErrorHandler::reportError(message,
                                                  ErrorHandler::ExecutionCompletionErrorType::LittleFsFileCloseFailed);
                        return;
                    }
                }

                // Object's path size is greater than ECSSMaxStringSize
                if(createFileStatus == -1)
                {
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
                    return;
                }

                // File's name contains a wildcard
                if(createFileStatus == -2)
                {
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
                    return;
                }

                // File already exists, so the creation was not successful
                if(createFileStatus == LFS_ERR_EXIST)
                {
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::FileAlreadyExists);
                    return;
                }
                // LittleFs lfs_file_open() generated error, during the creation of the file
                if(createFileStatus < 0)
                {
                    ErrorHandler::reportError(message,
                                              ErrorHandler::ExecutionCompletionErrorType::LittleFsFileOpenFailed);
                    return;
                }
                break;
            }

            case LFS_TYPE_REG:

                // Repository path leads to a file, not a repository
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionStartErrorType::RepositoryPathLeadsToFile);
                break;

            case(-1):

                // The repository's path contains a wildcard
                ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
                break;

            case(-2):

                // The object's type is unexpected
                ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            default:

                // lfs_stat (called from pathIsValidForCreation()) return an error code
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
    String<ECSSMaxStringSize> repositoryPathString("");
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

    // Extract the file name, which is the second string in this message
    String<ECSSMaxStringSize> fileNameString("");
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
                int32_t littleFsDeleteFileStatus = littleFsDeleteFile(&onBoardFileSystemObject, repositoryPathString,
                                                                      fileNameString);

                // Check the status of the deletion
                if (littleFsDeleteFileStatus >= 0)
                {
                    // File was deleted successfully
                }
                else
                {
                    // LittleFs lfs_remove() failed, during the deletion of this file
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
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
                break;

            case(-4):

                // The object's type is unexpected
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            default:

                // lfs_stat (called from pathIsValidForDeletion()) return an error code, during the search for the
                // repository, that was sent by the TC
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
                break;

        }
    }
}

void FileManagementService::reportAttributes(Message &message) {
    // TC[23,3]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

    // Extract the repository path, which is the first string in this message
    String<ECSSMaxStringSize> repositoryPathString("");
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPathString);

    // Extract the file name, which is the second string in this message
    String<ECSSMaxStringSize> fileNameString("");
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileNameString);


    // Extract, if any, wildcards in the repositoryString
    int8_t repositoyryStringWildcardStatus = checkForWildcard(repositoryPathString);

    //  Extract, if any, wildcards in the fileNameString
    int8_t fileNameStringWildcardStatus = checkForWildcard(fileNameString);

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
    else if (repositoyryStringWildcardStatus != -1)
    {
        // There is a wildcard in the repository path
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
    }

        // Check if there are any wildcard in the file name
    else if (fileNameStringWildcardStatus != -1)
    {
        // There is a wildcard in the file name
        ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UnexpectedWildcard);
    }

        // Check if the concatenated size of the object path is valid
    else if ((repositoryPathString.size() + fileNameString.size()) > ECSSMaxStringSize)
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
        int32_t reportFileStatus = littleFsReportFile(repositoryPathString, fileNameString,
                                                      &infoStruct);

        // Handle each possible outcome
        switch (reportFileStatus)
        {

            case (-1):

                // The object type is invalid
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            case (LFS_TYPE_REG):

                // Create a TM[23,4] fileAttributeReport
                fileAttributeReport(repositoryPathString, fileNameString, infoStruct.size);
                break;

            case (LFS_TYPE_DIR):

                // Object is a directory
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsInvalidObjectType);
                break;

            default:

                // Unknown error
                // TODO : Possibly more error messages depending on littlefs return types
                ErrorHandler::reportError(message,
                                          ErrorHandler::ExecutionCompletionErrorType::LittleFsStatFailed);
                break;
        }
    }
}

void FileManagementService::fileAttributeReport(const String<ECSSMaxStringSize>& repositoryString,
                                                const String<ECSSMaxStringSize>& fileNameString,
                                                uint32_t fileSize)
{
    //TM[23,4]
    Message report = createTM(MessageType::ReportAttributes);

    // Append the repository string
    report.appendString(repositoryString);

    // Append @ in order to separate them
    report.appendUint8('@');

    // Append the file name string
    report.appendString(fileNameString);

    // Append the size of the file
    report.appendUint32(fileSize);

    // Store the TM
    storeMessage(report);
}

void FileManagementService::execute(Message& message)
{

    switch (message.messageType)
    {
        case 1: createFile(message); // TC[23,1]
            break;

        case 2: deleteFile(message); // TC[23,2]
            break;

        case 3: reportAttributes(message); // TC[23,3]
            break;

        default:
            ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
    }
}