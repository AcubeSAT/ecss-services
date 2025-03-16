#ifndef PROJECT_ERRORHANDLER_HPP
#define PROJECT_ERRORHANDLER_HPP

#include <type_traits>
#include "Helpers/TypeDefinitions.hpp"

// Forward declaration of the class, since its header file depends on the ErrorHandler
class Message;

/**
 * A class that handles unexpected software errors, including internal errors or errors due to
 * invalid & incorrect input data.
 *
 * @todo (#28) Add auxiliary data field to errors
 */
class ErrorHandler {
private:
	/**
	 * Log the error to a logging facility. Platform-dependent.
	 */
	template <typename ErrorType>
	static void logError(const Message& message, ErrorType errorType);

	/**
	 * Log an error without a Message to a logging facility. Platform-dependent.
	 */
	template <typename ErrorType>
	static void logError(ErrorType errorType);

public:
	enum InternalErrorType {
		UnknownInternalError = 0,
		/**
		 * While writing (creating) a message, an amount of bytes was tried to be added but
		 * resulted in failure, since the message storage was not enough.
		 */
		MessageTooLarge = 1,
		/**
		 * Asked to append a number of bits larger than supported
		 */
		TooManyBitsAppend = 2,
		/**
		 * Asked to append a byte, while the previous byte was not complete
		 */
		ByteBetweenBits = 3,
		/**
		 * A string is larger than the largest allowed string
		 */
		StringTooLarge = 4,
		/**
		 * An error in the header of a packet makes it unable to be parsed
		 */
		UnacceptablePacket = 5,
		/**
		 * A date that isn't valid according to the Gregorian calendar or cannot be parsed by the
		 * TimeHelper
		 */
		InvalidDate = 6,
		/**
		 * Asked a Message type that doesn't exist
		 */
		UnknownMessageType = 7,
		/**
		 * Asked to append unnecessary spare bits
		 */
		InvalidSpareBits = 8,
		/**
		 * A function received a Message that was not of the correct type
		 */
		OtherMessageType = 9,
		/**
		 * Attempt to insert new element in a full map ST[08]
		 */
		MapFull = 10,
		/**
		 * A Message that is included within another message is too large
		 */
		NestedMessageTooLarge = 11,
		/**
		 * Request to copy packets in a time window, whose type is not recognized (ST(15)).
		 */
		InvalidTimeWindowType = 12,
		/**
		 * A request to access a non existing housekeeping structure in ST[03]
		 */
		NonExistentHousekeeping = 13,
		/**
		 * Attempt to access an invalid parameter in ST[03]
		 */
		NonExistentParameter = 14,
		/**
		 * Invalid TimeStamp parameters at creation
		 */
		InvalidTimeStampInput = 15,
		/**
		 * A requested element is not found
		 */
		ElementNotInArray = 16,
		/**
		 * Timestamp out of bounds to be stored or converted
		 */
		TimeStampOutOfBounds = 17,
		/**
		 * PMON base couldn't be initialized with the given parameter ID
		 */
		InvalidParameterId = 18,
		/**
		 * An unknown or unsupported PMON check type was encountered
		 */
		UnknownCheckType = 19,
		/**
		 * Invalid Event ID.
		 * This error occurs when an event ID is outside the acceptable range or conflicts with reserved identifiers.
		 */
		InvalidEventID = 20,
		/**
		 * Event data size exceeded.
		 * This error occurs when the size of event data exceeds the defined maximum allowable size.
		 */
		EventDataSizeExceeded = 21,

		/**
		 * The length of the provided data exceeds the maximum number of events allowed.
		 * This error occurs when attempting to process more events than the system can handle.
		 */
		LengthExceedsNumberOfEvents = 22
	};

	/**
	 * The error code for failed acceptance reports, as specified in ECSS 6.1.4.3d
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum AcceptanceErrorType {
		UnknownAcceptanceError = 0,
		/**
		 * The received message does not contain enough information as specified
		 */
		MessageTooShort = 1,
		/**
		 * Asked to read a number of bits larger than supported
		 */
		TooManyBitsRead = 2,
		/**
		 * Cannot read a string, because it is larger than the largest allowed string
		 */
		StringTooShort = 4,
		/**
		 * Cannot parse a Message, because there is an error in its secondary header
		 */
		UnacceptableMessage = 5
	};

	/**
	 * The error code for failed start of execution reports, as specified in ECSS 5.3.5.2.3g
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum ExecutionStartErrorType {
		UnknownExecutionStartError = 0,
		/**
		 * In the Event Action Service, in the addEventActionDefinition function an attempt was
		 * made to add an event Action Definition with an eventDefinitionID that exists
		 */
		EventDefinitionIDExistsError = 1,

		/**
		 *In the Event Action Service, in the addEventActionDefinition function an attempt was
		 * made to add an event Action Definition that is already enabled
		 */
		EventActionEnabledError = 2,
		/**
		 * In the Event Action Service, in the deleteEventActionDefinition function, an attempt
		 * was made to delete an event action definition that was enabled
		 */
		EventActionDeleteEnabledDefinitionError = 3,
		/**
		 * In the Event Action Service, an access attempt was made to an unknown event
		 * action definition
		 */
		EventActionUnknownEventActionDefinitionError = 4,
		/**
		 * EventAction refers to the service, EventActionIDefinitionID refers to the variable
		 * In the Event Action Service, an access attempt was made to an unknown eventDefinitionID
		 */
		EventActionUnknownEventActionDefinitionIDError = 5,
		SubServiceExecutionStartError = 6,
		InstructionExecutionStartError = 7,
		/**
		 * Attempt to change the value of a non existing parameter (ST[20])
		 */
		SetNonExistingParameter = 8,
		/**
		 * Attempt to access a non existing parameter (ST[20])
		 */
		GetNonExistingParameter = 9,
		/**
		 * Attempt to access a packet store that does not exist (ST[15])
		 */
		NonExistingPacketStore = 10,
		/**
		 * Attempt to change the start time tag of a packet store, whose open retrieval status is in progress (ST[15])
		 */
		SetPacketStoreWithOpenRetrievalInProgress = 11,
		/**
		 * Attempt to resume open retrieval of a packet store, whose by-time-range retrieval is enabled (ST[15])
		 */
		SetPacketStoreWithByTimeRangeRetrieval = 12,
		/**
		 * Attempt to access a packet with by-time range retrieval enabled (ST[15])
		 */
		GetPacketStoreWithByTimeRangeRetrieval = 13,
		/**
		 * Attempt to start the by-time-range retrieval of packet store, whose open retrieval is in progress (ST[15])
		 */
		GetPacketStoreWithOpenRetrievalInProgress = 14,
		/**
		 * Attempt to start by-time-range retrieval when its already enabled (ST[15])
		 */
		ByTimeRangeRetrievalAlreadyEnabled = 15,
		/**
		 * Attempt to create packet store, whose ID already exists (ST[15])
		 */
		AlreadyExistingPacketStore = 16,
		/**
		 * Attempt to create packet store, when the max number of packet stores is already reached (ST[15])
		 */
		MaxNumberOfPacketStoresReached = 17,
		/**
		 * Attempt to access a packet store with the storage status enabled (ST[15])
		 */
		GetPacketStoreWithStorageStatusEnabled = 18,
		/**
		 * Attempt to delete a packet whose by time range retrieval status is enabled (ST[15])
		 */
		DeletionOfPacketWithByTimeRangeRetrieval = 19,
		/**
		 * Attempt to delete a packet whose open retrieval status is in progress (ST[15])
		 */
		DeletionOfPacketWithOpenRetrievalInProgress = 20,
		/**
		 * Requested a time window where the start time is larger than the end time (ST[15])
		 */
		InvalidTimeWindow = 21,
		/**
		 * Attempt to copy a packet store to a destination packet store that is not empty (ST[15])
		 */
		DestinationPacketStoreNotEmtpy = 22,
		/**
		 * Attempt to set a reporting rate which is smaller than the parameter sampling rate.
		 * ST[04]
		 */
		InvalidReportingRateError = 23,
		/**
		 * Attempt to add definition to the struct map but its already full.(ST[19])
		 */
		EventActionDefinitionsMapIsFull = 24,
		/**
		 * Attempt to report/delete non existing housekeeping structure (ST[03])
		 */
		RequestedNonExistingStructure = 25,
		/**
		 * Attempt to create already created structure (ST[03])
		 */
		RequestedAlreadyExistingStructure = 26,
		/**
		 * Attempt to delete structure which has the periodic reporting status enabled (ST[03]) as per 6.3.3.5.2(d-2)
		 */
		RequestedDeletionOfEnabledHousekeeping = 27,
		/**
		 * Attempt to append a new parameter ID to a housekeeping structure, but the ID is already in the structure
		 * (ST[03])
		 */
		AlreadyExistingParameter = 28,
		/**
		 * Attempt to append a new parameter id to a housekeeping structure, but the periodic generation status is
		 * enabled (ST[03])
		 */
		RequestedAppendToEnabledHousekeeping = 29,
		/**
		 * Attempt to create a new housekeeping structure in Housekeeping Service, when the maximum number of
		 * housekeeping structures is already reached (ST[03])
		 */
		ExceededMaxNumberOfHousekeepingStructures = 30,
		/**
		 * Attempt to add a new simply commutated parameter in a specific housekeeping structure, but the maximum
		 * number of simply commutated parameters for this structure is already reached (ST[03])
		 */
		ExceededMaxNumberOfSimplyCommutatedParameters = 31,
		/**
		 * Attempt to set a reporting rate which is smaller than the parameter sampling rate.
		 * ST[04]
		 */
		InvalidSamplingRateError = 32,
		/**
		 * Attempt to add new statistic definition but the maximum number is already reached (ST[04])
		 */
		MaxStatisticDefinitionsReached = 33,
		/**
		 * Attempt to set the virtual channel of a packet store to a invalid value (ST[15])
		 */
		InvalidVirtualChannel = 34,
		/**
		 * Attempt to delete a packet store, whose storage status is enabled (ST[15])
		 */
		DeletionOfPacketStoreWithStorageStatusEnabled = 35,
		/**
		 * Attempt to copy packets from a packet store to another, but either no packet timestamp falls inside the
		 * specified timestamp, or more than one boolean argument were given as true in the 'copyPacketsTo' function
		 * (ST[15])
		 */
		CopyOfPacketsFailed = 36,
		/**
		 * Attempt to set a packet store size to a value that the available memory cannot handle (ST[15]).
		 */
		UnableToHandlePacketStoreSize = 37,
		/**
		 * Attempt to delete all parameter monitoring definitions but the Parameter Monitoring Function Status is
		 * enabled.
		 */
		InvalidRequestToDeleteAllParameterMonitoringDefinitions = 38,
		/**
		 * Attempt to delete one parameter monitoring definition but its Parameter Monitoring Status is
		 * enabled.
		 */
		InvalidRequestToDeleteParameterMonitoringDefinition = 39,
		/**
		 * Attempt to add a parameter that already exists to the Parameter Monitoring List.
		 */
		AddAlreadyExistingParameter = 40,
		/**
		 * Attempt to add a parameter in the Parameter Monitoring List but it's full
		 */
		ParameterMonitoringListIsFull = 41,
		/**
		 * Attempt to add or modify a limit check parameter monitoring definition, but the high limit is lower than
		 * the low limit.
		 */
		HighLimitIsLowerThanLowLimit = 42,
		/**
		 * Attempt to add or modify a delta check parameter monitoring definition, but the high threshold is lower than
		 * the low threshold.
		 */
		HighThresholdIsLowerThanLowThreshold = 43,
		/**
		 * Attempt to modify a non existent Parameter Monitoring definition.
		 */
		ModifyParameterNotInTheParameterMonitoringList = 44,
		/**
		 * Attempt to modify a parameter monitoring definition, but the instruction refers to a monitored parameter
		 * that is not the one used in that parameter monitoring definition.
		 */
		DifferentParameterMonitoringDefinitionAndMonitoredParameter = 45,
		/**
		 * Attempt to get a parameter monitoring definition that does not exist.
		 */
		GetNonExistingParameterMonitoringDefinition = 46,
		/**
		 * Request to report a non existent parameter monitoring definition.
		 */
		ReportParameterNotInTheParameterMonitoringList = 47,
		/**
		 * Attempt to add a new report type, when the addition of all report types is already enabled in the
		 * Application Process configuration (ST[14])
		 */
		AllServiceTypesAlreadyAllowed = 48,
		/**
		 * Attempt to add a new report type, when the max number of reports types allowed per service type
		 * definition in the Application Process configuration is already reached (ST[14])
		 */
		MaxReportTypesReached = 49,
		/**
		 * Attempt to add a new service type, when the max number of service types allowed per application process
		 * definition in the Application Process configuration is already reached (ST[14])
		 */
		MaxServiceTypesReached = 50,
		/**
		 * Attempt to add a report/event definition/housekeeping report type, when the specified application process
		 * ID is not controlled by the Service (ST[14])
		 */
		NotControlledApplication = 51,
		/**
		 * Parameter is requested, but the provider of the parameter value does not exist yet
		 */
		ParameterValueMissing = 52,
		/**
		 * Attempted to write to a read-only parameter
		 */
		ParameterReadOnly = 53,
		/**
		 * Attempted to read from a write-only parameter
		 */
		ParameterWriteOnly = 54,
		/**
		 * Attempt to add a new report type, when the addition of all report types is already enabled in the
		 * Application Process configuration (ST[14])
		 */
		AllReportTypesAlreadyAllowed = 55,
		/**
		 * Attempt to access a non-existing report type definition, from the application process configuration (ST[14])
		 */
		NonExistentReportTypeDefinition = 56,
		/**
		 * Attempt to access a non-existing service type definition, from the application process configuration (ST[14])
		 */
		NonExistentServiceTypeDefinition = 57,
		/**
		 * Attempt to access a non-existing application process definition, from the application process
		 * configuration (ST[14])
		 */
		NonExistentApplicationProcess = 58,
		/**
         * Size of file is bigger than allowed
         */
		SizeOfFileIsOutOfBounds = 59,
		/**
		 * Object path is invalid
		 */
		ObjectPathIsInvalid = 60,
		/**
		 * A wildcard was found where it shouldn't be present
	 	 */
		UnexpectedWildcard = 61,
		/**
		 * A file type that was expected to be a directory is a file instead
		 */
		RepositoryPathLeadsToFile = 62,
		/**
		 * PMON Check Type is requested, but it is missing (ST[12])
		 */
		 PMONCheckTypeMissing = 63,
	};

	/**
	 * The error code for failed progress of execution reports, as specified in ECSS 5.3.5.2.3g
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum ExecutionProgressErrorType {
		UnknownExecutionProgressError = 0,
	};

	/**
	 * The error code for failed completion of execution reports, as specified in ECSS 5.3.5.2.3g
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum ExecutionCompletionErrorType {
		UnknownExecutionCompletionError = 0,
		/**
		 * MemoryManagementChecksum comparison failed
		 */
		ChecksumFailed = 1,
		/**
		 * Address of a memory is out of the defined range for the type of memory
		 */
		AddressOutOfRange = 2,
		/**
         * File already exists, thus can't be created again
         */
		FileAlreadyExists = 3,
		/**
         * The requested object does not exist
         */
		ObjectDoesNotExist = 4,
		/**
		 * A delete file command was requested on a file that is locked
		 */
		AttemptedDeleteOnLockedFile = 5,
		/**
		 * A delete file command was requested on a directory
		 */
		AttemptedDeleteOnDirectory = 6,
		/**
		 * The filesystem reported an error during file deletion
		 */
		UnknownFileDeleteError = 7,
		/**
		 * A report file attributes command was requested on a directory
		 */
		AttemptedReportAttributesOnDirectory = 8,
		/**
         * Directory already exists, thus can't be created again
         */
		DirectoryAlreadyExists = 9,
		/**
		 * A delete directory command was requested on a non empty directory
		 */
		AttemptedDeleteNonEmptyDirectory = 10,
		/**
		 * A lock/unlock file command was requested on a directory
		 */
		AttemptedAccessModificationOnDirectory = 11,
	};

	/**
	 * The error code for failed routing reports, as specified in ECSS 6.1.3.3d
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum RoutingErrorType {
		UnknownRoutingError = 0,
	};

	/**
	 * The location where the error occurred
	 */
	enum ErrorSource {
		Internal,
		Acceptance,
		ExecutionStart,
		ExecutionProgress,
		ExecutionCompletion,
		Routing,
	};

	/**
	 * Report a failure and, if applicable, store a failure report message
	 *
	 * @tparam ErrorType The Type struct of the error; can be AcceptanceErrorType,
	 * StartExecutionErrorType,CompletionExecutionErrorType,  or RoutingErrorType.
	 * @param message The incoming message that prompted the failure
	 * @param errorCode The error's code, as defined in ErrorHandler
	 * @todo (#241) See if this needs to include InternalErrorType
	 */
	template <typename ErrorType>
	static void reportError(const Message& message, ErrorType errorCode);

	/**
	 * Report a failure about the progress of the execution of a request
	 *
	 * @note This function is different from reportError, because we need one more \p stepID
	 * to call the proper function for reporting the progress of the execution of a request
	 *
	 * @param message The incoming message that prompted the failure
	 * @param errorCode The error's code, when a failed progress of the execution of a request
	 * occurs
	 * @param stepID If the execution of a request is a long process, then we can divide
	 * the process into steps. Each step goes with its own definition, the stepID. Each value
	 * ,that the stepID is assigned, should be documented.
	 */
	static void reportProgressError(const Message& message, ExecutionProgressErrorType errorCode, StepId stepID);

	/**
	 * Report a failure that occurred internally, not due to a failure of a received packet.
	 *
	 * Note that these errors correspond to bugs or faults in the software, and should be treated
	 * differently. Such an error may prompt a task or software reset.
	 */
	static void reportInternalError(InternalErrorType errorCode);

	/**
	 * Make an assertion, to ensure that a runtime condition is met.
	 *
	 * Reports a failure that occurred internally, not due to a failure of a received packet.
	 *
	 * Creates an error if \p condition is false. The created error is Internal.
	 *
	 * @param condition The condition to check. Throws an error if false.
	 * @param errorCode The error code that is assigned to this error. One of the \ref ErrorHandler enum values.
	 * @return Returns \p condition, i.e. true if the assertion is successful, false if not.
	 */
	static bool assertInternal(bool condition, InternalErrorType errorCode) {
		if (not condition) {
			reportInternalError(errorCode);
		}

		return condition;
	}

	/**
	 * Make an assertion, to ensure that a runtime condition is met.
	 *
	 * Reports a failure that occurred while processing a request, in any of the process phases.
	 *
	 * Creates an error if \p condition is false. The created error corresponds to a \p message.
	 *
	 * @param condition The condition to check. Throws an error if false.
	 * @param message The message to associate with this error
	 * @param errorCode The error code that is assigned to this error. One of the \ref ErrorHandler enum values.
	 * @return Returns \p condition, i.e. true if the assertion is successful, false if not.
	 */
	template <typename ErrorType>
	static bool assertRequest(bool condition, const Message& message, ErrorType errorCode) {
		if (not condition) {
			reportError(message, errorCode);
		}

		return condition;
	}

	/**
	 * Convert a parameter given in C++ to an ErrorSource that can be easily used in comparisons.
	 * @tparam ErrorType One of the enums specified in ErrorHandler.
	 * @param error An error code of a specific type
	 * @return The corresponding ErrorSource
	 */
	template <typename ErrorType>
	inline static ErrorSource findErrorSource(ErrorType errorType) {
		// Static type checking
		ErrorSource source = Internal;

		if (std::is_same<ErrorType, AcceptanceErrorType>()) {
			source = Acceptance;
		}
		if (std::is_same<ErrorType, ExecutionStartErrorType>()) {
			source = ExecutionStart;
		}
		if (std::is_same<ErrorType, ExecutionProgressErrorType>()) {
			source = ExecutionProgress;
		}
		if (std::is_same<ErrorType, ExecutionCompletionErrorType>()) {
			source = ExecutionCompletion;
		}
		if (std::is_same<ErrorType, RoutingErrorType>()) {
			source = Routing;
		}

		return source;
	}
};

#endif // PROJECT_ERRORHANDLER_HPP
