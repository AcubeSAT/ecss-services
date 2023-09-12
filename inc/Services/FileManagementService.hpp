#ifndef ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP

#include "Helpers/Filesystem.hpp"
#include "Service.hpp"

/**
 * Implementation of ST[23] file management service
 *
 * ECSS 6.23 & 8.23
 *
 * @ingroup Services
 *
 * The filesystem of the satellite will be structured, meaning files are stored in within directories. For more info
 * check DDJF_OBSW.
 * @todo define the number of filesystems (obc, su, ?)
 * @todo define the MAX_SIZE_OF_SILE_BYTE
 * @todo define the locking status
 * @todo define the summary report support
 * @todo do we need to move files ?
 * @todo do we need all these operation handling (suspend, abort, report, periodic report)
 * @todo define max number of file copy operations (what is the meaning of having one ?)
 * @note The wildcard character is set to be '*'.
 * @note The "null terminator" between two variables with dynamic length is '0'
 *
 * @ingroup services
 *
 */
class FileManagementService : public Service {
public:
	inline static constexpr uint8_t ServiceType = 23;

	/**
     * The wildcard character accepted by the service
     */
	inline static constexpr char Wildcard = '*';

	/**
     * Character which denotes the end of a string and the beginning of the next (if there is any)
     */
	inline static constexpr char VariableStringTerminator = '\0';

	/**
	 * The maximum possible size of a file, in bytes.
	 */
	inline static constexpr size_t MaxPossibleFileSizeBytes = 4096;

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
     * Checks done prior to creating a file :
     * - The size of the file is below the maximum allowed file size
     * - The path is valid, meaning it leads to an existing repository
     * - The repository's path and file's name do not contain a wildcard
     * - The file does not already exist
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_DIR)
     * - The object path size is less than ECSSMaxStringSize
     */
	void createFile(Message& message);

	/**
     * TC[23,2] Delete the file at the provided repository path, with the provided file name
     * Checks done prior to deleting a file :
     * - The path is valid, meaning it leads to an existing file
     * - The repository's path and file's name do not contain a wildcard
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_REG)
     * - The object path size is less than ECSSMaxStringSize
     */
	void deleteFile(Message& message);

	/**
     * TC[23,3] Report attributes of a file at the provided repository path and file name
     * Checks done prior to reporting a file :
     * - The path is valid, meaning it leads to an existing file
     * - The repository's path and file's name do not contain a wildcard
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_REG)
     * - The object path size is less than ECSSMaxStringSize
     */
	void reportAttributes(Message& message);

	/**
     * TM[23,4] Create a report with the attributes from a file at the provided object path
     */
	void fileAttributeReport(const Filesystem::Path& repositoryPath, const Filesystem::Path& fileName, const Filesystem::Attributes& attributes);

	/**
     * TC[23,9] Create a directory on the filesystem
     */
	void createDirectory(Message& message){};

	/**
     * TC[23,10] Delete a directory from the filesystem
     */
	void deleteDirectory(Message& message){};

	/**
	 * It is responsible to call the suitable function that executes a tele-command packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);

private:
	inline static constexpr bool LockFileSupport = false;
	inline static constexpr bool SearchFileSupport = false;
	inline static constexpr bool SummaryReportSupport = false;
	inline static constexpr bool MoveFileSupport = true;
	inline static constexpr bool SuspendFileCopyOperationSupport = false;
	inline static constexpr bool SuspendAllFileCopyOperationSupport = false;
	inline static constexpr bool AbortFileCopyOperationSupport = false;
	inline static constexpr bool AbortAllFileCopyOperationSupport = false;
	inline static constexpr bool ReportProgressOfCopyOperationsSupport = false;
	inline static constexpr bool EnablePeriodicReportingOfFileCopyStatusSupport = false;

	using Path = Filesystem::Path;

	inline static Path getFullPath(const Path& repositoryPath, const Path& filePath) {
		Path fullPath = repositoryPath;
		fullPath.append("/");
		fullPath.append(filePath);

		return fullPath;
	}
};

#endif //ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
