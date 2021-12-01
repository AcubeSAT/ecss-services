

#include "Services/FileManagementService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"

#define REPOSITORY_SUMMARY_REPORT_MAX_OBJECTS 4096

uint8_t FileManagementService::getStringUntilZeroTerminator(Message &message, char extractedString[ECSS_MAX_STRING_SIZE], uint8_t &stringSize)
{

    char currentChar[ECSS_MAX_STRING_SIZE];
    uint8_t charCounter = 0;
    currentChar[charCounter] = (char) message.readByte();

    // Increment the counter until '\0' is reached
    while(currentChar[charCounter] != '\0')
    {
        charCounter++;

        // Check if size is below the maximum allowed
        if(charCounter == ECSS_MAX_STRING_SIZE)
        {
            ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
            return 1; // This should be interpreted as error code
        }

        // Pass the char byte and increment the size of the string
        extractedString[stringSize] = currentChar[charCounter];
        stringSize++;

        // Increment the char pointer and read next byte
        currentChar[charCounter] = message.readByte();
    }

    return 0; // This should be interpreted as ok code
}

int32_t FileManagementService::pathIsValidForCreation(String<ECSS_MAX_STRING_SIZE> repositoryString, uint8_t repositoryStringSize)
{
    lfs_info *pInfo;
    lfs_info infoStruct;
    pInfo = &infoStruct;

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto* repositoryStringChar = reinterpret_cast<uint8_t*>(repositoryString.data());

    // Call lfs_stat in order to fill the info_struct with data
    int32_t infoStructFillStatus = lfs_stat(&fs1, repositoryStringChar, pInfo);

    // Check if the repository exists and no errors are produced during the lfs_stat() execution
    if(infoStructFillStatus >= 0)
    {
        // Check if the object at the end of the path is a file or a repository
        if(infoStruct->type == LFS_TYPE_DIR)
        {
            // TODO maybe check for the size of the file ?
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
        // Return NO_OBJECT_AT_THIS_PATH
        return 2;
    }
}

int32_t littleFsCreateFile(lfs_t &fs, lfs_file_t &file, String<ECSS_MAX_STRING_SIZE> repositoryPath, uint8_t repositoryPathSize,
                           String<ECSS_MAX_STRING_SIZE> fileName, uint8_t fileNameSize, int flags)
{
    // Copy the repositoryPath to a char array, in order to use it in lfs_stat
    auto* repositoryPathChar = reinterpret_cast<uint8_t*>(repositoryPath.data());

    // Copy the repositoryPath to a char array, in order to use it in lfs_stat
    auto* fileNameChar = reinterpret_cast<uint8_t*>(fileName.data());

    // Concatenate 2 strings in 1. From now on use objectPath
    char objectPath [sizeof(repositoryPathSize + fileNameSize)];
    strcat(objectPath, reinterpret_cast<const char *>(repositoryPathChar));
    strcat(objectPath, reinterpret_cast<const char *>(fileNameChar));

    // Check the size of the object path
    if((repositoryPathSize + fileNameSize) > ECSS_MAX_STRING_SIZE)
    {
        // Return error code that indicates too large object path
        return -1;
    }

    // Create the file using lfs_file_open with the appropriate flags
    int32_t lfsCreateFileStatus = lfs_file_open(fs, file, objectPath, flags);

    return lfsCreateFileStatus;
}

int32_t pathIsValidForDeletion(String<ECSS_MAX_STRING_SIZE> repositoryString, uint8_t repositoryStringSize,
                               String<ECSS_MAX_STRING_SIZE> fileNameString, uint8_t fileNameStringSize)
{
    lfs_info *pInfo;
    lfs_info infoStruct;
    pInfo = &infoStruct;

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto* repositoryStringChar = reinterpret_cast<uint8_t*>(repositoryString.data());

    // Check for wildcards in repositoryStringChar
    for(uint8_t currentChar = 0; currentChar < repositoryStringSize; currentChar++)
    {
        // Iterate over the string
        if(repositoryStringChar[currentChar] == FileManagementService::wildcard)
        {
            // Return error code which indicates the existence of a wildcard
            return -1;
        }
    }

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto* fileNameStringChar = reinterpret_cast<uint8_t*>(fileNameString.data());

    // Check for wildcards in fileNameChar
    for(uint8_t currentChar = 0; currentChar < fileNameStringSize; currentChar++)
    {
        // Iterate over the string
        if(fileNameStringChar[currentChar] == FileManagementService::wildcard)
        {
            // Return error code which indicates the existence of a wildcard
            return -1;
        }
    }

    // Concatenate 2 strings in 1. From now on use objectPath
    char objectPath [sizeof(repositoryStringSize + fileNameStringSize)];
    strcat(objectPath, reinterpret_cast<const char *>(repositoryStringChar));
    strcat(objectPath, reinterpret_cast<const char *>(fileNameStringChar));

    // Check for objectPath size
    if((repositoryStringSize + fileNameStringSize) > ECSS_MAX_STRING_SIZE)
    {
        // Return error code which indicates that the object path size is too large
        return -2;
    }

    // Call lfs_stat to fill the infoStruct with information about the object
    int32_t infoStructFillStatus = lfs_stat(&fls, objectPath, pInfo);

    // Check if the lfs_stat is completed successfully
    if(infoStructFillStatus >= 0)
    {
        // Check what type of object is found
        if(infoStruct->type == LFS_TYPE_DIR)
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

int32_t littleFsDeleteFile(lfs_t &fs, String<ECSS_MAX_STRING_SIZE> repositoryPath, uint8_t repositoryPathSize,
                           String<ECSS_MAX_STRING_SIZE> fileName, uint8_t fileNameSize)
{
    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto* repositoryStringChar = reinterpret_cast<uint8_t*>(repositoryPath.data());

    // Copy the repositoryString to a char array, in order to use it in lfs_stat
    auto* fileNameStringChar = reinterpret_cast<uint8_t*>(fileName.data());

    // Concatenate 2 strings in 1. From now on use objectPath
    char objectPath [sizeof(repositoryPathSize + fileNameSize)];
    strcat(objectPath, reinterpret_cast<const char *>(repositoryStringChar));
    strcat(objectPath, reinterpret_cast<const char *>(fileNameStringChar));

    // Call lfs_remove
    int32_t lfsDeleteFileStatus = lfs_remove(&fls, objectPath);

    // Return status of deletion for the lfs function
    return lfsDeleteFileStatus;

}

void FileManagementService::createFile(Message& message)
{
    // TC[23,1]
    // TODO should i implement fileLockedStatus and additionalFileAttributes ?
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile);

    //Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize = 0;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *)repositoryPath, repositoryPathSize);


    //Extract the file name, which is the second string in this message
    char fileName[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameSize = 0;
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileName, fileNameSize);
    String<ECSS_MAX_STRING_SIZE> fileNameString((uint8_t *)fileName, fileNameSize);

    //Extract the file size in bytes
    uint16_t fileSizeBytes = message.readUint16();

    // Check the repository name extraction status
    if(repositoryPathExtractionStatus != 0)
    {
        // Size of repository path is too large
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    // Check the file name extraction status
    else if(fileNameExtractionStatus != 0)
    {
        // Size of file name is too large
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::SizeOfStringIsOutOfBounds);
    }

    //Check the validity of the request at service level
    else if(fileSizeBytes > MAX_FILE_SIZE_BYTES)
    {
        //Size of file out of bounds
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::SizeOfFileIsOutOfBounds);
    }

    // Check if the path is valid for creation of a file
    else if(pathIsValidForCreation(repositoryPathString, repositoryPathSize) != 1)
    {
        //Invalid path
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }

    // Proceed with the creation
    else
    {
        // Declare the lfs_file_t object
        lfs_file_t file;

        // Call lfs_file_open to create the file
        if(littleFsCreateFile(&fs, &file, repositoryPathString, repositoryPathSize, fileNameString, fileNameSize, 0b1100000011) >= 0)
        {
            //Calling lfs_file_close to release any allocated resources
            if(lfs_file_close(&lfs, &file) >= 0)
            {
                //Successful file creation
            }
            else
            {
                //LittleFs generated error
                //TODO The error codes that littlefs will produce are documented, so we could integrate them but later
                ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
            }
        }
        else
        {
            //LittleFs generated error
            //TODO The error codes that littlefs will produce are documented, so we could integrate them but later
            ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
}

void FileManagementService::deleteFile(Message& message)
{
    // TC[23,2]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile);

    // Extract the repository path, which is the first string in this message
    char repositoryPath[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathSize;
    uint8_t repositoryPathExtractionStatus = getStringUntilZeroTerminator(message, repositoryPath, repositoryPathSize);
    String<ECSS_MAX_STRING_SIZE> repositoryPathString((uint8_t *)repositoryPath, repositoryPathSize);

    // Extract the file name, which is the second string in this message
    char fileName[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameSize = 0;
    uint8_t fileNameExtractionStatus = getStringUntilZeroTerminator(message, fileName, fileNameSize);
    String<ECSS_MAX_STRING_SIZE> fileNameString((uint8_t *)fileName, fileNameSize);

    // Extract the file size in bytes
    uint16_t fileSizeBytes = message.readUint16();

    // If clause must be added if lockFileSupport = true.

    // Check the validity of the request at service level
    if(pathIsValidForDeletion(repositoryPathString, repositoryPathSize, fileNameString, fileNameSize) != 1)
    {
        // Invalid path
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
    else
    {
        // Call lfs_remove in order to delete the file
        if(littleFsDeleteFile(&fls, repositoryPathString, repositoryPathSize, fileNameString, fileNameSize) >= 0)
        {
            //Successful Deletion
        }
        else
        {
            //LittleFs generated error
            ErrorHandler::reportError(message, ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
}

void FileManagementService::reportAttributes(Message& message)
{
    // TC[23,3]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract the file name, which is the second string in this message
    char fileNameString[ECSS_MAX_STRING_SIZE];
    uint8_t fileNameCounter = getStringUntilZeroTerminator(message);
    message.readString(fileNameString, fileNameCounter);

    //Extract the file size in bytes
    uint16_t fileSizeBytes = message.readUint16();

    //Check the validity of the request at service level
    if(pathIsValidForReport(repositoryPathString, fileNameString) == 1)
    {
        //Invalid path
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
    else
    {
        //Valid path
        // TODO implement littleFsReportFile using littleFs (lf_stat)

        //Struct that will store the file information
        lfs_info infoStruct;
        if(littleFsReportFile(repositoryPathString, fileNameString, fileSizeBytes, infoStruct) != 1)
        {
            //LittleFs generated error
            ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
        else
        {
            //Command the creation of a TM[23,4] file attribute report
            fileAttributeReport(repositoryPathString, infoStruct);

        }
    }
}

void FileManagementService::fileAttributeReport(char *repositoryPathName,
                                                uint16_t repositoryPathLength,
                                                lfs_info infoStruct)
{
    //TM[23,4]
    Message report = createTM(MessageType::CreateAttributesReport);

    //Append repository name
    for (int repositoryPathIndex = 0; repositoryPathIndex < repositoryPathLength; ++repositoryPathIndex)
    {
        report.appendByte(repositoryPathName[repositoryPathIndex]);
    }

    //Append file name. LittleFs stores the name with \0 at the end of the string.
    uint8_t filePathIndex = 0;
    while (infoStruct->name != NULL && infoStruct->name[filePathIndex] != '\0')
    {
        report.appendByte(infoStruct->name[filePathIndex]);
        filePathIndex++;
    }

    //Append the size of the file
    report.appendSint32(infoStruct.size);
}

void FileManagementService::findFile(Message &message)
{
    // TC[23,7]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::FindFile);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract the search pattern, which is the second string in this message
    char searchPattern[ECSS_MAX_STRING_SIZE];
    uint8_t searchPatternCounter = getStringUntilZeroTerminator(message);
    message.readString(searchPattern, searchPatternCounter);

    //Extract the wildcard position in the search pattern if any.
    //If there is not a wildcard in the search pattern, then return -1.
    wildcardPositionIndex = extractWildcardPositionIndex(searchPattern, searchPatternCounter, wildcard);

    //Sanity check for the wildcardPositionIndex (the wildcard position is smaller than the absolute maximum of message length )
    if((wildcardPositionIndex <= (searchPatternCounter - 1)) && (wildcardPositionIndex >= -1))
    {
        //Check if the wildcard is used as a prefix, as a suffix, in the middle of the pattern or not used at all.
        //No wildcard is detected
        if (wildcardPositionIndex == -1)
        {

        }
        //Wildcard is used as a prefix
        else if (wildcardPositionIndex == 0)
        {

        }
        //Wildcard is used as a suffix
        else if (wildcardPositionIndex == (searchPatternCounter - 1))
        {

        }
        //Wildcard is used in the middle of the pattern
        else
        {

        }
    }
    else
    {
        //Create failed completion of execution error
        ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
    }

}

void FileManagementService::foundFileReport(char repositoryPath[ECSS_MAX_STRING_SIZE], uint8_t repositoryPathSize, char fileName[ECSS_MAX_STRING_SIZE], uint8_t fileNameSize)
{
    //TM [23,8]
    Message report = createTM(FoundFileReport);

    // Append the repository path
    for (int repositoryPathStringIndex = 0;
         repositoryPathStringIndex < repositoryPathSize - 1; repositoryPathStringIndex++)
    {
        report.appendByte((uint8_t)repositoryPath[repositoryPathStringIndex]);
    }

    // Append the fileName
    for (int fileNameStringIndex = 0;
         fileNameStringIndex < fileNameSize - 1; fileNameStringIndex++)
    {
        report.appendByte((uint8_t)fileName[fileNameStringIndex]);
    }

}

void FileManagementService::createDirectory(Message &message)
{
    // TC[23,9]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract the directory name, which is the second string in this message
    char directoryNameString[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameCounter = getStringUntilZeroTerminator(message);
    message.readString(directoryNameString, directoryNameCounter);

    //Check if the path is valid
    if(checkIfPathIsValid(repositoryPathString, repositoryPathCounter, directoryNameString, directoryNameCounter) == 1)
    {
        //Create the directory
        int8_t mkridReturnCode = lfs_mkdir();

        //Check if the Directory is created successfully
        if (mkridReturnCode != 0)
        {
            //Create failed completion of execution error
            ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        //Create failed start of execution error
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }
}

void FileManagementService::deleteDirectory(Message &message)
{
    //TC[23,10]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract the directory name, which is the second string in this message
    char directoryNameString[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameCounter = getStringUntilZeroTerminator(message);
    message.readString(directoryNameString, directoryNameCounter);

    //Check if the directory path is valid
    if (checkIfPathIsValid(repositoryPathString, repositoryPathCounter, directoryNameString, directoryNameCounter) == 1)
    {
        //Extract (if any) wildcards in the message
        wildcardPositionIndex = extractWildcardPositionIndex(repositoryPathString, repositoryPathCounter, wildcard);

        //Check if the wildcard is in a logical place (between the first and the last character)
        if((wildcardPositionIndex <= (repositoryPathCounter - 1) &&  (wildcardPositionIndex >= -1))
        {
            //Check if there are any wildcards in the directory path
            if (wildcardPositionIndex == -1)
            {
                //There are not any wildcards, delete the directory
                int8_t removeReturnCode = lfs_remove();

                //Check the status of lfs_remove()
                if (removeReturnCode != 0)
                {
                    //Create failed completion of execution error
                    ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
                }
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

void FileManagementService::renameDirectory(Message &message)
{
    //TC[23,11]
    message.assertTC(FileManagementService::ServiceType, FileManagementService::MessageType::RenameDirectory);

    //Extract the repository path, which is the first string in this message
    char repositoryPathString[ECSS_MAX_STRING_SIZE];
    uint8_t repositoryPathCounter = getStringUntilZeroTerminator(message);
    message.readString(repositoryPathString, repositoryPathCounter);

    //Extract the old directory name, which is the second string in this message
    char directoryNameStringOld[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameCounterOld = getStringUntilZeroTerminator(message);
    message.readString(directoryNameStringOld, directoryNameCounterOld);

    //Extract the new directory name, which is the third string in this message
    char directoryNameStringNew[ECSS_MAX_STRING_SIZE];
    uint8_t directoryNameCounterNew = getStringUntilZeroTerminator(message);
    message.readString(directoryNameStringNew, directoryNameCounterNew);

    //Check if both the old and the new path are valid
    if ((checkIfPathIsValid(repositoryPathString, repositoryPathCounter, directoryNameStringOld, directoryNameCounterOld) == 1)
       && (checkIfPathIsValid(repositoryPathString, repositoryPathCounter, directoryNameStringNew, directoryNameCounterNew) == 1))
    {
        //Rename the directory
        int8_t renameDirectoryReturnCode = lfs_rename();

        //Check the status of lfs_rename();
        if (renameDirectorySupport != 0)
        {
            //Create failed completion of execution error
            ErrorHandler::reportError(message,ErrorHandler::ExecutionCompletionErrorType::UnknownExecutionCompletionError);
        }
    }
    else
    {
        //Create failed start of execution error
        ErrorHandler::reportError(message,ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid);
    }

}

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

