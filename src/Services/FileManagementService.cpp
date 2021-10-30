

#include "Services/FileManagementService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

uint8_t FileManagementService::getStringUntilZeroTerminator(Message &message)
{
    char currentChar;
    uint8_t charCounter = 0;
    currentChar = (char) message.readByte();

    //Increment the counter until '\0' is reached
    while(currentChar != '\0')
    {
        charCounter++;

        //Check if size is below the maximum allowed
        if(charCounter == ECSS_MAX_STRING_SIZE)
        {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
        }

        //Read next byte
        currentChar = message.readByte();
    }
    return charCounter;
}

void FileManagementService::createFile(Message &message)
{
    // TODO should i implement fileLockedStatus and additionalFileAttributes ?
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile);

    //Extract repository path which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract file name which is the second string in this message
    char fileNameString[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameCounter = getStringUntilZeroTerminator(message);
    message.readString(fileNameString, fileNameCounter);

    //Extract file size in bytes
    uint16_t fileSizeBytes = message.readUint16();

    //Check the validity of the request at service level
    if(fileSizeBytes > MAX_FILE_SIZE_BYTES)
    {
        //Size of file out of bounds
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
    }
    // TODO implement pathIsValid (lfs_stat)
    else if(pathIsValidForCreation(repositoryPathString, fileNameString) == 1)
    {
        //Invalid path
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
    else
    {
        // TODO implement littleFsCreateFile using littleFs
        if(littleFsCreateFile(repositoryPathString, fileNameString, fileSizeBytes) != 1)
        {
            //LittleFs generated error
            //TODO The error codes that littlefs will produce are documented, so we could integrate them but later
            ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
        else
        {
            //Successful creation
        }
    }
}

void FileManagementService::deleteFile(Message &message)
{
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile);

    //Extract repository path which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract file name which is the second string in this message
    char fileNameString[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameCounter = getStringUntilZeroTerminator(message);
    message.readString(fileNameString, fileNameCounter);

    //Check the validity of the request at service level
    if(pathIsValidForDeletion(repositoryPathString, fileNameString) == 1)
    {
        //Invalid path
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
    else
    {
        //Valid path
        // TODO implement littleFsCreateFile using littleFs
        if(littleFsCreateFile(repositoryPathString, fileNameString, fileSizeBytes) != 1)
        {
            //LittleFs generated error
            ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
        else
        {
            //Successful Deletion
        }
    }
}


