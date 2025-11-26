#pragma once

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

		enum class SuspensionStatus : uint8_t {
			SUSPENDED = 0,
			NOT_SUSPENDED = 1,
			NOT_FOUND = 255
		};

		enum class OperationRegistrationError : uint8_t {
			MAXIMUM_CONCURRENT_OPERATIONS_REACHED = 0,
			OPERATION_ID_ALREADY_EXISTS = 1,
			UNKNOWN_ERROR = 255
		};

		enum class OperationProgressUpdateError : uint8_t {
			FILE_COPY_OPERATION_NOT_FOUND = 0,
			INVALID_OPERATION_STATE_TRANSITION = 1,
			UNKNOWN_ERROR = 255
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

		/**
		 * Initializes the FileCopyOperation, so it can be picked up by the file transfer handler layer.
		 */
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

		/**
		 * Used to clear everything regarding a FileCopyOperation, so it can be removed safely.
		 */
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

		bool setState(const State newState) {
			if (isValidStateTransition(state, newState)) {
				state = newState;
				return true;
			}
			return false;
		}

		/**
		 * Checks if the file copy operation involves the given repository path and the state transition is valid, and
		 * successful.
		 * @tparam From The current state of the file copy operation.
		 * @tparam To The state the caller wants the file copy operation to transition to.
		 * @param repositoryPath The repository path that must be involved with the file copy operation.
		 * @return True if the operation state is changed or does not involve the given path, false otherwise.
		 */
		template <State From, State To>
		bool updateOperationStateIfMatchesPath(const Filesystem::ObjectPath& repositoryPath) {
			if (state == From && involvesRepositoryPath(repositoryPath)) {
				return setState(To);
			}
			return true;
		}

		[[nodiscard]] uint8_t getProgressPercentage() const {
			if (totalBytes == 0) {
				return 0;
			}
			return static_cast<uint8_t>((bytesTransferred * 100ULL) / totalBytes); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
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

	/**
	 * @brief Find an existing file copy operation by its identifier.
	 *
	 * Performs a linear search over the internal array of file copy operations
	 * and returns a pointer to the matching entry if it exists.
	 *
	 * @param operationId Unique identifier of the file copy operation to search for.
	 * @return Pointer to the matching FileCopyOperation if found, or nullptr if no such operation is currently registered.
	 *
	 */
	FileCopyOperation* findFileCopyOperation(uint16_t operationId);

	/**
	 * @brief Register and initialize a new file copy operation.
	 *
	 * Attempts to reserve the given operation identifier and allocate a free slot in the internal file copy operation
	 * table, then initializes that slot with the provided source path, target path, operation type, and associated request message.
	 * If the maximum number of concurrent operations has already been reached, if the operation identifier is already
	 * in use, or if no idle slot can be found despite available capacity, the function returns an appropriate registration error.
	 *
	 * @param operationId Unique identifier to associate with the new file copy operation.
	 * @param sourcePath  Path of the file or directory to be copied.
	 * @param targetPath  Destination path for the file or directory copy.
	 * @param type        Type of copy operation (e.g. copy or move).
	 * @param message     Request message associated with this operation, used for later
	 *                    completion or error reporting.
	 *
	 * @return etl::expected<void, FileCopyOperation::OperationRegistrationError>
	 *         An engaged expected on successful registration, or an error indicating why
	 *         the operation could not be registered.
	 */
	etl::expected<void, FileCopyOperation::OperationRegistrationError> addFileCopyOperation(uint16_t operationId,
							 const Filesystem::ObjectPath& sourcePath,
							 const Filesystem::ObjectPath& targetPath,
							 FileCopyOperation::Type type,
							 const Message& message);

	/**
	 * @brief Remove and reset a completed or canceled file copy operation.
	 *
	 * Looks up the operation with the given identifier, resets its internal state, releases the reserved identifier,
	 * and decrements the count of active operations. If no matching active operation exists, the call has no effect.
	 *
	 * @param operationId Identifier of the file copy operation to remove.
	 */
	void removeFileCopyOperation(uint16_t operationId);

	/**
	 * @brief Check the suspension status of a file copy operation.
	 *
	 * Inspects the state of the operation associated with the given identifier to determine if it is currently
	 * suspended (on hold), active, or if the operation ID is invalid.
	 *
	 * @param operationId Identifier of the operation to query.
	 * @retval FileCopyOperation::SuspensionStatus::SUSPENDED If the operation exists and its state is ON_HOLD.
	 * @retval FileCopyOperation::SuspensionStatus::NOT_SUSPENDED If the operation exists but is not currently on hold.
	 * @retval FileCopyOperation::SuspensionStatus::NOT_FOUND If no operation with the given identifier exists.
	 */
	[[nodiscard]] FileCopyOperation::SuspensionStatus getOperationSuspensionStatus(uint16_t operationId) const;

	/**
	 * @brief Change the state of an existing file copy operation.
	 *
	 * Finds the operation associated with the given identifier and updates its internal state to the provided value.
	 * If the operation cannot be found, the state remains unchanged.
	 *
	 * @param operationId Identifier of the operation whose state is to be updated.
	 * @param newState    New state to assign to the file copy operation.
	 *
	 * @return true  If the operation was found and its state was updated.
	 * @return false If no operation with the given identifier exists.
	 */
	bool setOperationState(uint16_t operationId, FileCopyOperation::State newState);

	/**
	 * @brief Update progress information for a file copy operation.
	 *
	 * Updates the number of bytes transferred and the total number of bytes for the specified operation, and marks
	 * the operation as COMPLETED when the transfer has finished.
	 * If an operation with the given identifier exists, its progress fields (bytesTransferred and totalBytes) are updated.
	 * When @p totalBytes is greater than zero and @p bytesTransferred is greater than or equal to @p totalBytes,
	 * the operation state is set to FileCopyOperation::State::COMPLETED.
	 *
	 * @param operationId      Identifier of the operation to update.
	 * @param bytesTransferred Current number of bytes that have been successfully copied.
	 * @param totalBytes       Total number of bytes to be copied.
	 *
	 * @return true  If a matching operation exists and, when completion criteria are met, the state transition to COMPLETED succeeds.
	 * @return false If no matching operation is found, or if the state cannot be updated to COMPLETED when requested,
	 *				 or if the operation is not yet completed.
	 */
	etl::expected<void, FileCopyOperation::OperationProgressUpdateError> updateOperationProgress(uint16_t operationId, uint32_t bytesTransferred, uint32_t totalBytes);

	/**
	 * @brief Signal successful completion of a file copy operation.
	 *
	 * Triggers the execution completion verification for the original request message and removes the corresponding
	 * operation from the internal registry. Returns if the operation is not found.
	 *
	 * @param operationId Identifier of the successfully completed operation.
	 */
	void notifyOperationSuccess(uint16_t operationId);

	/**
	 * @brief Report a failed file copy operation with a specific error.
	 *
	 * Maps the low-level filesystem copy error to a higher-level execution completion error type, reports it via
	 * the error handler using the original request message, and finally removes the operation from the registry.
	 * Returns if the operation is not found.
	 *
	 * @param operationId Identifier of the failed file copy operation.
	 * @param errorType   Reason for the failure as reported by the filesystem layer.
	 */
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
	 * TM[23,4] Create a report with the attributes from a file at the provided object path.
	 *
	 * Builds and stores a telemetry packet that carries the attributes of the given file,
	 * including its repository path, file name, size in bytes, and lock status, for later
	 * transmission to ground.
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
	 * TC[23,9] Create a directory on the filesystem.
	 *
	 * Reads the repository path and directory path from the telecommand, concatenates them into a full directory path,
	 * and validates that:
	 * - No wildcard characters are present in the resulting object path.
	 * - The repository path exists and refers to a directory.
	 * If the checks pass, attempts to create the directory; if a directory with the same path already exists or another
	 * error occurs, an appropriate execution-completion error is reported.
	 */
	void createDirectory(Message& message);

	/**
	 * TC[23,10] Delete a directory from the filesystem.
	 *
	 * Reads the repository path and directory path from the telecommand, concatenates them into a full directory path,
	 * and validates that:
	 * - No wildcard characters are present in the resulting object path.
	 * - The repository path exists and refers to a directory.
	 * If the directory deletion fails because the directory does not exist, is not empty, or for another reason,
	 * an appropriate execution-completion error is reported.
	 */
	void deleteDirectory(Message& message);

	/**
	 * TC[23,14] Copy a file in the requested path.
	 *
	 * Parses a file-copy request from the telecommand, including operation identifier, source path and target path,
	 * and validates that the copy operation can be registered and started.
	 * If validation succeeds, registers the file copy operation and instructs the filesystem to start copying the file
	 * associated with the given operation ID; on validation failure, a suitable error is reported.
	 */
	void copyFile(Message& message);

	/**
	 * TC[23,15] Move a file to the requested path.
	 *
	 * Parses a file-move request from the telecommand, including operation identifier, source path and target path,
	 * and validates that the move operation can be registered and started.
	 * If validation succeeds, registers the file move operation and instructs the filesystem to start moving the file
	 * associated with the given operation ID; on validation failure, a suitable error is reported.
	 */
	void moveFile(Message& message);

	/**
	 * TC[23,16] Suspend file copy operations.
	 *
	 * Reads a list of file copy operation identifiers from the telecommand and, for each one that exists, attempts to
	 * transition the operation state from IN_PROGRESS to ON_HOLD.
	 * If an operation ID cannot be found, an execution-start error is reported for that ID, while processing of the
	 * remaining IDs continues.
	 */
	void suspendFileCopyOperations(Message& message);

	/**
	 * TC[23,17] Resume file copy operations.
	 *
	 * Reads a list of file copy operation identifiers from the telecommand and, for each one that exists, attempts to
	 * transition the operation state from ON_HOLD back to IN_PROGRESS.
	 * If an operation ID cannot be found, an execution-start error is reported for that ID, while processing of the
	 * remaining IDs continues.
	 */
	void resumeFileCopyOperations(Message& message);

	/**
	 * TC[23,18] Abort file copy operations.
	 *
	 * Reads a list of file copy operation identifiers from the telecommand and, for each one that exists, attempts
	 * to transition the operation state to FAILED and remove the corresponding operation from the internal registry.
	 * If an invalid state transition is detected for an operation, or the operation ID is not found, an execution-start
	 * error is reported; processing then continues with the remaining IDs.
	 */
	void abortFileCopyOperations(Message& message);

	/**
	 * TC[23,19] Suspend all file copy operations involving a repository path.
	 *
	 * Reads a repository path from the telecommand and scans all registered file copy operations, transitioning any
	 * operation that is IN_PROGRESS and involves this repository path into the ON_HOLD state.
	 */
	void suspendFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,20] Resume all file copy operations involving a repository path.
	 *
	 * Reads a repository path from the telecommand and scans all registered file copy operations, transitioning any
	 * operation that is ON_HOLD and involves this repository path back into the IN_PROGRESS state.
	 */
	void resumeFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,21] Abort all file copy operations involving a repository path.
	 *
	 * Reads a repository path from the telecommand and scans all registered file copy operations, attempting to:
	 * - Mark each active operation involving the repository path as FAILED.
	 * - Remove the corresponding operation from the internal registry.
	 * If no active operations involving the given path are found, an execution-start error is reported to indicate
	 * that there were no file copy operations to abort.
	 */
	void abortFileCopyOperationsInPath(Message& message);

	/**
	 * TC[23,22] Enable the periodic reporting of the file copy status.
	 *
	 * Reads the requested reporting interval from the telecommand and verifies that it lies within the allowed bounds.
	 * If the interval is valid, stores it and enables periodic generation of file copy status reports.
	 * If the interval is out of range, an execution-start error is reported and periodic reporting is left disabled.
	 */
	void enablePeriodicFileCopyStatusReporting(Message& message);

	/**
	 * TM[23,23] File copy status report.
	 *
	 * Generates and queues a telemetry packet summarizing the current status of active file copy operations, to be sent
	 * periodically or on demand depending on the configured reporting policy.
	 */
	void generateFileCopyStatusReport();

	/**
	 * TC[23,24] Disable the periodic reporting of the file copy status.
	 *
	 * Validates the telecommand type and, if accepted, disables the generation of periodic file copy status reports
	 * while leaving any ongoing copy operations unaffected.
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

	/**
	 * @brief Validate and register a new file copy operation.
	 *
	 * Performs a series of checks on the requested file copy operation before registering it in the internal operation table.
	 * The function verifies that:
	 * - The operation identifier is not already in use.
	 * - The source and target paths do not contain wildcard characters.
	 * - The source path refers to an existing regular file (not a directory).
	 * - At least one of the paths involves the local repository.
	 * - The source file is not locked.
	 *
	 * If any of these checks fail, an appropriate execution-start error is reported using the provided @p message and
	 * the function returns false. If all checks pass, it attempts to register the operation via addFileCopyOperation();
	 * any registration error is translated to a corresponding execution-start error and also causes the function to return false.
	 *
	 * @param message        Telecommand message used for error reporting.
	 * @param operationId    Identifier requested for the new file copy operation.
	 * @param sourceFullPath Fully qualified source path of the file to be copied.
	 * @param targetFullPath Fully qualified target path where the file will be copied.
	 * @param operationType  Type of copy operation being requested.
	 *
	 * @return true  If all validation checks pass and the operation is successfully registered.
	 * @return false If validation fails or the operation cannot be registered.
	 */
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

		/**
		 * @brief Reserve an identifier for use.
		 *
		 * Attempts to add the given @p id to the set of active identifiers if there is available capacity and the
		 * identifier is not already in use.
		 *
		 * @param id Identifier to reserve.
		 * @return true  If the identifier was successfully reserved.
		 * @return false If the maximum number of identifiers is reached or @p id is already in use.
		 */
		[[nodiscard]] bool reserveId(const uint16_t id) {
			if (activeCount >= MaxConcurrentFileCopyOperations || isInUse(id)) {
				return false;
			}
			activeIds[activeCount++] = id;
			return true;
		}

		/**
		 * @brief Check whether an identifier is currently in use.
		 *
		 * Scans the active identifier list and returns true if the given @p id is present among the currently reserved identifiers.
		 *
		 * @param id Identifier to query.
		 * @return true  If the identifier is found in the active list.
		 * @return false If the identifier is not in use.
		 */
		[[nodiscard]] bool isInUse(const uint16_t id) const {
			for (uint8_t i = 0; i < activeCount; i++) {
				if (activeIds[i] == id) {
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Release a previously reserved identifier.
		 *
		 * Removes the given @p id from the active identifier list, if present, by replacing it with the last active
		 * entry and decrementing the active count.
		 *
		 * @param id Identifier to release back to the pool of available IDs.
		 */
		void releaseId(const uint16_t id) {
			for (uint8_t i = 0; i < activeCount; i++) {
				if (activeIds[i] == id) {
					activeIds[i] = activeIds[activeCount - 1];
					activeCount--;
					break;
				}
			}
		}
	};

	OperationIdManager operationIdManager{};

};
