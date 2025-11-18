#ifndef ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP

#include "Helpers/Filesystem.hpp"
#include "Service.hpp"
#include "etl/string_utilities.h"

/**
 * Implementation of ST[23] file management service
 *
 * ECSS 6.23 & 8.23
 *
 * @ingroup Services
 *
 * The filesystem of the satellite will be structured, meaning files are stored in within directories. For more info
 * check DDJF_OBSW.
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
	 * The maximum possible size of a file, in bytes.
	 */
	inline static constexpr size_t MaxPossibleFileSizeBytes = 4096;

	// File Copy Subservice Configuration Constants
	static constexpr uint8_t MaxConcurrentFileCopyOperations = 10;
	static constexpr uint16_t DefaultFileCopyReportingIntervalMs = 1000;
	static constexpr uint16_t MinFileCopyReportingIntervalMs = 1000;
	static constexpr uint16_t MaxFileCopyReportingIntervalMs = 60000;
	static constexpr bool InitialPeriodicReportingState = false;
	static constexpr uint8_t InitialOperationCount = 0;
	static constexpr uint16_t InvalidOperationId = 0;

	enum MessageType : uint8_t {
		// File handling subservice (ECSS 6.23.4)
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
		// File copy subservice (ECSS 6.23.5)
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

	struct FileCopyOperation {

		enum class State : uint8_t {
			IDLE = 0,
			PENDING = 1,
			IN_PROGRESS = 2,
			ON_HOLD = 3,
			COMPLETED = 4,
			FAILED = 5
		};

		enum class Type : uint8_t {
			COPY = 0,
			MOVE = 1
		};

		uint16_t operationId = InvalidOperationId;
		State state = State::IDLE;
		Type type = Type::COPY;
		Filesystem::ObjectPath sourcePath;
		Filesystem::ObjectPath targetPath;
		uint32_t bytesTransferred = 0;
		uint32_t totalBytes = 0;
		uint32_t startTime = 0;
		Message requestMessage;

		FileCopyOperation() : sourcePath(""), targetPath("") {}

		void initialize(const uint16_t id, const Filesystem::ObjectPath& srcPath,
			const Filesystem::ObjectPath& dstPath, const Type opType, const Message& message) {
			operationId = id;
			sourcePath = srcPath;
			targetPath = dstPath;
			type = opType;
			state = State::PENDING;
			bytesTransferred = 0;
			totalBytes = 0;
			startTime = 0;
			requestMessage = message;
		}

		void reset() {
			operationId = InvalidOperationId;
			state = State::IDLE;
			type = Type::COPY;
			sourcePath.clear();
			targetPath.clear();
			bytesTransferred = 0;
			totalBytes = 0;
			startTime = 0;
			requestMessage = Message();
		}

		[[nodiscard]] State getState() const { return state; }
		[[nodiscard]] uint16_t getId() const { return operationId; }
		[[nodiscard]] Filesystem::ObjectPath getSourcePath() const { return sourcePath; }
		[[nodiscard]] Filesystem::ObjectPath getTargetPath() const { return targetPath; }
		void setState(const State newState) {
			if (isValidStateTransition(state, newState)) {
				state = newState;
			}
		}

		[[nodiscard]] uint8_t getProgressPercentage() const {
			if (totalBytes == 0) return 0;
			return static_cast<uint8_t>((bytesTransferred * 100ULL) / totalBytes);
		}

		[[nodiscard]] bool isActive() const {
			return state != State::IDLE && state != State::COMPLETED && state != State::FAILED;
		}

		[[nodiscard]] bool involvesRepositoryPath(const Filesystem::ObjectPath& repositoryPath) const {
			return sourcePath.find(repositoryPath.c_str()) == 0 || targetPath.find(repositoryPath.c_str()) == 0;
		}

		static bool isValidStateTransition(const State from, const State to) {
			switch (from) {
				case State::PENDING:
					return to == State::IN_PROGRESS || to == State::FAILED;
				case State::IN_PROGRESS:
					return to == State::ON_HOLD || to == State::COMPLETED || to == State::FAILED;
				case State::ON_HOLD:
					return to == State::IN_PROGRESS || to == State::FAILED;
				default:
					return false;
			}
		}
	};

	[[nodiscard]] bool isPeriodicFileCopyReportingEnabled() const {
		return periodicFileCopyReportingEnabled;
	}

	[[nodiscard]] uint16_t getFileCopyReportingIntervalMs() const {
		return fileCopyReportingIntervalMs;
	}

	FileCopyOperation* findFileCopyOperation(uint16_t operationId);
	bool addFileCopyOperation(uint16_t operationId,
							 const Filesystem::ObjectPath& sourcePath,
							 const Filesystem::ObjectPath& targetPath,
							 FileCopyOperation::Type type,
							 const Message& message);
	void removeFileCopyOperation(uint16_t operationId);
	[[nodiscard]] bool isOperationSuspended(uint16_t operationId) const;
	bool setOperationState(uint16_t operationId, FileCopyOperation::State newState);
	void updateOperationProgress(uint16_t operationId, uint32_t bytesTransferred, uint32_t totalBytes);
	void notifyOperationSuccess(uint16_t operationId);
	void notifyOperationFailure(uint16_t operationId, Filesystem::FileCopyError errorType);

	/**
     * TC[23,1] Create a file at the provided repository path, give it the provided file name and file size
     * Checks done prior to creating a file:
     * - The size of the file is below the maximum allowed file size
     * - The path is valid, meaning it leads to an existing repository
     * - The repository's path and file's name do not contain a wildcard
     * - The file does not already exist
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_DIR)
     * - The object path size is less than ECSSMaxStringSize
     *
     * @note Apart from the above checks, the _maximum file size_ telecommand argument is currently ignored.
     */
	void createFile(Message& message);

	/**
     * TC[23,2] Delete the file at the provided repository path, with the provided file name
     * Checks done prior to deleting a file:
     * - The path is valid, meaning it leads to an existing file
     * - The repository's path and file's name do not contain a wildcard
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_REG)
     * - The object path size is less than ECSSMaxStringSize
     */
	void deleteFile(Message& message);

	/**
     * TC[23,3] Report attributes of a file at the provided repository path and file name
     * Checks done prior to reporting a file:
     * - The path is valid, meaning it leads to an existing file
     * - The repository's path and file's name do not contain a wildcard
     * - The object type at the repository path is nothing but a directory (LFS_TYPE_REG)
     * - The object path size is less than ECSSMaxStringSize
     */
	void reportAttributes(Message& message);

	/**
     * TM[23,4] Create a report with the attributes from a file at the provided object path
     */
	void fileAttributeReport(const Filesystem::ObjectPath& repositoryPath, const Filesystem::ObjectPath& fileName, const Filesystem::Attributes& attributes);

	/**
	 * TM[23,5] Lock a file at the provided repository path and file name.
	 * Checks done prior to locking a file:
	 * - The path is valid, meaning it leads to an existing file
	 * - The repository's path and file's name do not contain a wildcard
	 */
	void lockFile(Message& message);

	/**
	 * TM[23,6] Unlock a file at the provided repository path and file name.
	 * Checks done prior to unlocking a file:
	 * - The path is valid, meaning it leads to an existing file
	 * - The repository's path and file's name do not contain a wildcard
	 */
	void unlockFile(Message& message);

	/**
     * TC[23,9] Create a directory on the filesystem
     */
	void createDirectory(Message& message);

	/**
     * TC[23,10] Delete a directory from the filesystem
     */
	void deleteDirectory(Message& message);

	/**
	 * TC[23,14] Copy a file in the requested path.
	 */
	void copyFile(Message& message);

	/**
	 * TC[23,15] Move a file to the requested path.
	 */
	void moveFile(Message& message);

	/**
	 * TC[23,16] Suspend file copy operations
	 */
	void suspendFileCopyOperations(Message& message);

	/**
	 * TC[23,17] Resume file copy operations
	 */
	void resumeFileCopyOperations(Message& message);

	/**
	 * TC[23,18] Abort file copy operations
	 */
	void abortFileCopyOperations(Message& message);

	/**
	 * TC[23,19] Suspend all file copy operations involving a repository path
	 */
	void suspendFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,20] Resume all file copy operations involving a repository path
	 */
	void resumeFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,21] Abort all file copy operations involving a repository path
	 */
	void abortFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,22] Enable the periodic reporting of the file copy status
	 */
	void enablePeriodicFileCopyStatusReporting(Message& message);

	/**
	 * TM[23,23] File copy status report
	 */
	void generateFileCopyStatusReport();

	/**
	 * TC[23,24] Disable the periodic reporting of the file copy status
	 */
	void disablePeriodicFileCopyStatusReporting(const Message& message);

	/**
	 * Ask the FS for the available unallocated memory and return it.
	 *
	 * @return uint32_t The bytes of available unallocated memory
	 */
	uint32_t getUnallocatedMemory();

	/**
	 * It is responsible to call the suitable function that executes a tele-command packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);

private:
	using ObjectPath = Filesystem::ObjectPath;
	using Path = Filesystem::Path;

	/**
	 * Returns the full filesystem path for an object given the repository path and the file path
	 * @param repositoryPath The repository path
	 * @param filePath The file path
	 * @return The full path, where the repository path and file path are separated by a single '/' (slash)
	 *
	 * @note All leading and trailing slashes are removed from the repositoryPath and filePath objects.
	 */
	inline static Path getFullPath(ObjectPath& repositoryPath, ObjectPath& filePath) {
		etl::trim_from_left(repositoryPath, "/");
		etl::trim_from_right(repositoryPath, "/");

		etl::trim_from_left(filePath, "/");
		etl::trim_from_right(filePath, "/");

		Path fullPath = ("");
		fullPath.append(repositoryPath);
		fullPath.append("/");
		fullPath.append(filePath);
		return fullPath;
	}

	struct FileCopyRequest {
		uint16_t operationId = 0;
   		Filesystem::Path sourceFullPath = "";
   		Filesystem::Path targetFullPath = "";
	};

	bool validateFileCopyOperationRegistration(
	    const Message& message,
	    uint16_t operationId,
	    const Filesystem::Path& sourceFullPath,
	    const Filesystem::Path& targetFullPath,
	    FileCopyOperation::Type operationType);

	static FileCopyRequest parseFileCopyRequest(Message& message);

	// File Copy Subservice State
	std::array<FileCopyOperation, MaxConcurrentFileCopyOperations> fileCopyOperations{};
	uint8_t activeFileCopyOperationCount = InitialOperationCount;
	bool periodicFileCopyReportingEnabled = InitialPeriodicReportingState;
	uint16_t fileCopyReportingIntervalMs = DefaultFileCopyReportingIntervalMs;

	struct OperationIdManager {
		std::array<uint16_t, MaxConcurrentFileCopyOperations> activeIds{};
		uint8_t activeCount = 0;

		bool reserveId(const uint16_t id) {
			if (isInUse(id) || activeCount >= MaxConcurrentFileCopyOperations) {
				return false;
			}
			activeIds[activeCount++] = id;
			return true;
		}

		[[nodiscard]] bool isInUse(const uint16_t id) const {
			for (uint8_t i = 0; i < activeCount; i++) {
				if (activeIds[i] == id) return true;
			}
			return false;
		}

		void releaseId(const uint16_t id) {
			for (uint8_t i = 0; i < activeCount; i++) {
				if (activeIds[i] == id) {
					for (uint8_t j = i; j < activeCount - 1; j++) {
						activeIds[j] = activeIds[j + 1];
					}
					activeCount--;
					break;
				}
			}
		}

		[[nodiscard]] size_t getActiveCount() const {
			return activeCount;
		}
	};

	OperationIdManager operationIdManager{};

};

#endif //ECSS_SERVICES_FILEMANAGEMENTSERVICE_HPP
