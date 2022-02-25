#ifndef ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP

#include <Service.hpp>

extern "C" {
#include "Helpers/lfs_stub.h"
};
#define MAX_FILE_SIZE_BYTES 4096
#define MAX_FILE_COPY_OPERATIONS 10
#define MAX_FILE_NAME_SIZE 256
#define MAX_OPERATION_IDENTIFIERS 256
#define REPOSITORY_SUMMARY_REPORT_MAX_OBJECTS 4096

/**
 * Implementation of ST[23] file management service
 *
 * @author Christos Ioannidis <xristosioan@gmail.com>
 *
 * ECSS 6.23 & 8.23
 *
 * The filesystem of the satellite will be structured, meaning files are stored in within directories. For more info
 * check DDJF_OBSW.
 * @todo define the number of filesystems (obc, su, ?)
 * @todo specify the set of file attributes (atleast the size in bytes of any file)
 * @todo define the MAX_SIZE_OF_SILE_BYTE
 * @todo define the locking status
 * @todo define the searching status (possibly will be supported)
 * @todo define the summary report support
 * @todo do we need to move files ?
 * @todo do we need all these operation handling (suspend, abort, report, periodic report)
 * @todo define max number of file copy operations (what is the meaning of having one ?)
 * @todo moving files is allowed ? does littlefs have multiple file transfer handlers
 * @note The wildcard character is set to be '*'.
 * @note The "null terminator" between two variables with dynamic length is '@'
 * @note LittleFS permits only the deletion of empty directories
 *
 * @ingroup services
 *
 */


class FileManagementService : public Service {
private:

    bool lockFileSupport = false;
    bool searchFileSupport = false;
    bool summaryReportSupport = false;
    bool moveFileSupport = true;
    bool suspendFileCopyOperationSupport = false;
    bool suspendAllFileCopyOperationSupport = false;
    bool abortFileCopyOperationSupport = false;
    bool abortAllFileCopyOperationSupport = false;
    bool reportProgressOfCopyOperationsSupport = false;
    bool enablePeriodicReportingOfFileCopyStatusSupport = false;

    /**
     * LittleFs file system object.
     * @todo Is one file system enough, do we need more (maybe yes, for su, obc, etc) ?
     */
    lfs_t onBoardFileSystemObject;


    //------------------------------------------------------------------------

    /**
     * The purpose of this function is to check if there is a wildcard in a given string
     * @param messageString : The message passed as a String
     * @return status of execution (1: Message does not contain any wildcards, -1: Message contains at least one wildcard)
     */
    static int8_t checkForWildcard(String<ECSSMaxStringSize> messageString);

    /**
     * The purpose of this function is to take care of the extraction process for the object path variable
     * Parses the message until a '@' is found. Then returns the actual string, excluding the '@' char
     * @param message : The message that we want to parse
     * @param extractedString : pointer to a String<ECSSMaxStringSize> that will house the extracted string
     * @return status of execution (0: Successful completion, 1: Error occurred)
     */
    static uint8_t getStringUntilZeroTerminator(Message &message, String<ECSSMaxStringSize> &extractedString);

    /**
     * The purpose of this function is to check if the object path is valid for creation
     * Checks if there is an object at this path and returns its type.
     * @param repositoryString : Pointer to the repository path
     * @return status of execution (2: Object is a directory, 1: Object is a file, -1: Repository path contains a wildcard
     *                             -2: Invalid type of object, Negative LittleFS error code: lfs_stat() returned an error code)
     */
    int32_t pathIsValidForCreation(String<ECSSMaxStringSize> repositoryString);

    /**
     * The purpose of this function is to initiate a creation of a file using littleFs
     * Checks if there is already a file with this name
     * @param fileSystem : Pointer to the file system struct
     * @param file : Pointer to the file struct
     * @param repositoryPath : The repository path
     * @param fileName : The file name
     * @param flags : Input flags that determines the creation status
     * @return status of execution (-1: File's object path name is too large, -2: there is a wildcard in the repository's path string,
     *                              lfs_open_file status: Status of the lfs function that creates a file)
     */
    int32_t littleFsCreateFile(lfs_t *fileSystem,
                               lfs_file_t *file,
                               String<ECSSMaxStringSize> repositoryPath,
                               String<ECSSMaxStringSize> fileName,
                               int32_t flags);

    /**
     * The purpose of this function is to check if the the strings that compose the object path (repository string and
     * file name string) are seperated with a slash "/" between them. If they are not seperated by one and only one
     * slash, then it modifies the object path accordingly.
     * @param objectPathString : String that will house the complete object path
     * @param fileNameString : String with the file name
     * @return -
     */
    void checkForSlashes(String<ECSSMaxStringSize> &objectPathString, uint8_t *&fileNameChar);

    /**
     * The purpose of this function is to check if the object path is valid for deletion
     * Checks if there is an object at this path, if it is a file and does not contain any wildcards
     * @param repositoryString : String with the repository name
     * @param fileNameString : String with the file name
     * @return status of execution (2: Object is a directory, 1: Object is a file,
     *                             -1: If there is a wildcard in the repository's path string
     *                             -2: If there is a wildcard in the file's name string
     *                             -3: Object path size is too large
     *                             -4: Invalid object type
     *                             Other negative code: lfs_stat returned error code)
     */
    int32_t pathIsValidForDeletion(String<ECSSMaxStringSize> repositoryString,
                                   String<ECSSMaxStringSize> fileNameString);

    /**
     * The purpose of this function is to initiate the deletion of a file using littleFs
     * Checks if there is already a file with this name and if there is a wildcard in the object path
     * @param fs : Pointer to the file system struct
     * @param repositoryPath : The repository path
     * @param fileName : The file name
     * @return lfs_remove status of execution
     */
    int32_t littleFsDeleteFile(lfs_t *fs,
                               String<ECSSMaxStringSize> repositoryPath,
                               String<ECSSMaxStringSize> fileName);

    /**
     * The purpose of this function is to initiate the lfs_stat function, which will fill the info struct with all
     * the necessary information about a files report.
     * Checks if there is an object at this path, if it is a file and does not contain any wildcards,
     * @param repositoryString : String with the repository name
     * @param fileNameString : String with the file name
     * @param infoStruct : lfs_info which will house the file's attributes
     * @return status of execution (-1 invalid object type,
     *                               1: Object is a file,
     *                               2: Object is a directory
     *                               Any error code that lfs_stat might return)
     */
    int32_t littleFsReportFile(String<ECSSMaxStringSize> repositoryString,
                               String<ECSSMaxStringSize> fileNameString,
                               lfs_info *infoStruct);

public:

    inline static const uint8_t ServiceType = 23;

    /**
     * The wildcard character accepted by the service
     */
    static const char wildcard = '*';

    /**
     * Character which denotes the end of a string and the beginning of the next (if there is any)
     */
    static const char variableStringTerminator = '@';

    enum MessageType : uint8_t {
        CreateFile = 1,
        DeleteFile = 2,
        ReportAttributes = 3,
        CreateAttributesReport = 4,
        LockFile = 5,
        UnlockFile = 6,
        FindFile = 7,
        FoundFileReport = 8,
        CreateDirectory = 9,
        DeleteDirectory = 10,
        RenameDirectory = 11,
        ReportSummaryDirectory = 12,
        SummaryDirectoryReport = 13,
        CopyFile = 14,
        MoveFile = 15,
        SuspendFileCopyOperation = 16,
        ResumeFileCopyOperation = 17,
        AbortFileCopyOperation = 18,
        SuspendFileCopyOperationInPath = 19,
        ResumeFileCopyOperationInPath = 20,
        AbortFileCopyOperationInPath = 21,
        EnablePeriodicReportingOfFileCopy = 22,
        FileCopyStatusReport = 23,
        DisablePeriodicReportingOfFileCopy = 24
    };

    /**
     * TC[23,1] Create a file at the provided repository path, give it the provided file name and file size
     */
    void createFile(Message &message);

    /**
     * TC[23,2] Delete the file at the provided repository path, with the provided file name
     */
    void deleteFile(Message &message);

    /**
     * TC[23,3] Report attributes of a file at the provided repository path and file name
     */
    void reportAttributes(Message &message);

    /**
     * TM[23,4] Create a report with the attributes from a file at the provided object path
     */
    void fileAttributeReport(const String<ECSSMaxStringSize> &repositoryString,
                             const String<ECSSMaxStringSize> &fileNameString,
                             uint32_t fileSize);

    /**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
    void execute(Message &message);

};


#endif //ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
