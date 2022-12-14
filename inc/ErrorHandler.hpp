#ifndef PROJECT_ERRORHANDLER_HPP
#define PROJECT_ERRORHANDLER_HPP

#include <stdint.h> // for the uint_8t stepID
#include <type_traits>

// Forward declaration of the class, since its header file depends on the ErrorHandler
class Message;

/**
 * A class that handles unexpected software errors, including internal errors or errors due to
 * invalid & incorrect input data.
 *
 * @todo Add auxiliary data field to errors
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
		 * made to add an event Action Definition with an eventActionDefinitionID that exists
		 */
		EventActionDefinitionIDExistsError = 1,
		/**
		 * In the Event Action Service, in the deleteEventActionDefinition function, an attempt
		 * was made to delete an event action definition that was enabled
		 */
		EventActionDeleteEnabledDefinitionError = 2,
		/**
		 * In the Event Action Service, an access attempt was made to an unknown event
		 * action definition
		 */
		EventActionUnknownEventDefinitionError = 3,
		/**
		 * EventAction refers to the service, EventActionIDefinitionID refers to the variable
		 * In the Event Action Service, an access attempt was made to an unknown eventActionDefinitionID
		 */
		EventActionUnknownEventActionDefinitionIDError = 4,
		SubServiceExecutionStartError = 5,
		InstructionExecutionStartError = 6,
		/**
		 * Attempt to change the value of a non existing parameter (ST[20])
		 */
		SetNonExistingParameter = 7,
		/**
		 * Attempt to access a non existing parameter (ST[20])
		 */
		GetNonExistingParameter = 8,
		/**
		 * Attempt to access a packet store that does not exist (ST[15])
		 */
		NonExistingPacketStore = 9,
		/**
		 * Attempt to change the start time tag of a packet store, whose open retrieval status is in progress (ST[15])
		 */
		SetPacketStoreWithOpenRetrievalInProgress = 10,
		/**
		 * Attempt to resume open retrieval of a packet store, whose by-time-range retrieval is enabled (ST[15])
		 */
		SetPacketStoreWithByTimeRangeRetrieval = 11,
		/**
		 * Attempt to access a packet with by-time range retrieval enabled (ST[15])
		 */
		GetPacketStoreWithByTimeRangeRetrieval = 12,
		/**
		 * Attempt to start the by-time-range retrieval of packet store, whose open retrieval is in progress (ST[15])
		 */
		GetPacketStoreWithOpenRetrievalInProgress = 13,
		/**
		 * Attempt to start by-time-range retrieval when its already enabled (ST[15])
		 */
		ByTimeRangeRetrievalAlreadyEnabled = 14,
		/**
		 * Attempt to create packet store, whose ID already exists (ST[15])
		 */
		AlreadyExistingPacketStore = 15,
		/**
		 * Attempt to create packet store, when the max number of packet stores is already reached (ST[15])
		 */
		MaxNumberOfPacketStoresReached = 16,
		/**
		 * Attempt to access a packet store with the storage status enabled (ST[15])
		 */
		GetPacketStoreWithStorageStatusEnabled = 17,
		/**
		 * Attempt to delete a packet whose by time range retrieval status is enabled (ST[15])
		 */
		DeletionOfPacketWithByTimeRangeRetrieval = 18,
		/**
		 * Attempt to delete a packet whose open retrieval status is in progress (ST[15])
		 */
		DeletionOfPacketWithOpenRetrievalInProgress = 19,
		/**
		 * Requested a time window where the start time is larger than the end time (ST[15])
		 */
		InvalidTimeWindow = 20,
		/**
		 * Attempt to copy a packet store to a destination packet store that is not empty (ST[15])
		 */
		DestinationPacketStoreNotEmtpy = 21,
		/**
		 * Attempt to set a reporting rate which is smaller than the parameter sampling rate.
		 * ST[04]
		 */
		InvalidReportingRateError = 22,
		/**
		 * Attempt to add definition to the struct map but its already full.(ST[19])
		 */
		EventActionDefinitionsMapIsFull = 23,
		/**
		 * Attempt to report/delete non existing housekeeping structure (ST[03])
		 */
		RequestedNonExistingStructure = 24,
		/**
		 * Attempt to create already created structure (ST[03])
		 */
		RequestedAlreadyExistingStructure = 25,
		/**
		 * Attempt to delete structure which has the periodic reporting status enabled (ST[03]) as per 6.3.3.5.2(d-2)
		 */
		RequestedDeletionOfEnabledHousekeeping = 26,
		/**
		 * Attempt to append a new parameter ID to a housekeeping structure, but the ID is already in the structure
		 * (ST[03])
		 */
		AlreadyExistingParameter = 27,
		/**
		 * Attempt to append a new parameter id to a housekeeping structure, but the periodic generation status is
		 * enabled (ST[03])
		 */
		RequestedAppendToEnabledHousekeeping = 28,
		/**
		 * Attempt to create a new housekeeping structure in Housekeeping Service, when the maximum number of
		 * housekeeping structures is already reached (ST[03])
		 */
		ExceededMaxNumberOfHousekeepingStructures = 29,
		/**
		 * Attempt to add a new simply commutated parameter in a specific housekeeping structure, but the maximum
		 * number of simply commutated parameters for this structure is already reached (ST[03])
		 */
		ExceededMaxNumberOfSimplyCommutatedParameters = 30,
		/**
		 * Attempt to set a reporting rate which is smaller than the parameter sampling rate.
		 * ST[04]
		 */
		InvalidSamplingRateError = 31,
		/**
		 * Attempt to add new statistic definition but the maximum number is already reached (ST[04])
		 */
		MaxStatisticDefinitionsReached = 32,
		/**
		 * Attempt to set the virtual channel of a packet store to a invalid value (ST[15])
		 */
		InvalidVirtualChannel = 33,
		/**
		 * Attempt to delete a packet store, whose storage status is enabled (ST[15])
		 */
		DeletionOfPacketStoreWithStorageStatusEnabled = 34,
		/**
		 * Attempt to copy packets from a packet store to another, but either no packet timestamp falls inside the
		 * specified timestamp, or more than one boolean argument were given as true in the 'copyPacketsTo' function
		 * (ST[15])
		 */
		CopyOfPacketsFailed = 35,
		/**
		 * Attempt to set a packet store size to a value that the available memory cannot handle (ST[15]).
		 */
		UnableToHandlePacketStoreSize = 36,
		/**
		 * Attempt to delete all parameter monitoring definitions but the Parameter Monitoring Function Status is
		 * enabled.
		 */
		InvalidRequestToDeleteAllParameterMonitoringDefinitions = 37,
		/**
		 * Attempt to delete one parameter monitoring definition but its Parameter Monitoring Status is
		 * enabled.
		 */
		InvalidRequestToDeleteParameterMonitoringDefinition = 38,
		/**
		 * Attempt to add a parameter that already exists to the Parameter Monitoring List.
		 */
		AddAlreadyExistingParameter = 39,
		/**
		 * Attempt to add a parameter in the Parameter Monitoring List but it's full
		 */
		ParameterMonitoringListIsFull = 40,
		/**
		 * Attempt to add or modify a limit check parameter monitoring definition, but the high limit is lower than
		 * the low limit.
		 */
		HighLimitIsLowerThanLowLimit = 41,
		/**
		 * Attempt to add or modify a delta check parameter monitoring definition, but the high threshold is lower than
		 * the low threshold.
		 */
		HighThresholdIsLowerThanLowThreshold = 42,
		/**
		 * Attempt to modify a non existent Parameter Monitoring definition.
		 */
		ModifyParameterNotInTheParameterMonitoringList = 43,
		/**
		 * Attempt to modify a parameter monitoring definition, but the instruction refers to a monitored parameter
		 * that is not the one used in that parameter monitoring definition.
		 */
		DifferentParameterMonitoringDefinitionAndMonitoredParameter = 44,
		/**
		 * Attempt to get a parameter monitoring definition that does not exist.
		 */
		GetNonExistingParameterMonitoringDefinition = 45,
		/**
		 * Request to report a non existent parameter monitoring definition.
		 */
		ReportParameterNotInTheParameterMonitoringList = 46,
		/**
		 * Attempt to add a new service type, when the addition of all service types is already enabled in the
		 * Application Process configuration (ST[14])
		 */
		AllServiceTypesAlreadyAllowed = 47,
		/**
		 * Attempt to add a new report type, when the max number of reports types allowed per service type
		 * definition in the Application Process configuration is already reached (ST[14])
		 */
		MaxReportTypesReached = 48,
		/**
		 * Attempt to add a new service type, when the max number of service types allowed per application process
		 * definition in the Application Process configuration is already reached (ST[14])
		 */
		MaxServiceTypesReached = 49,
		/**
		 * Attempt to add a report/event definition/housekeeping report type, when the specified application process
		 * ID is not controlled by the Service (ST[14])
		 */
		NotControlledApplication = 50,
		/**
		 * Parameter is requested, but the provider of the parameter value does not exist yet
		 */
		ParameterValueMissing = 51,
		/**
		 * Attempted to write to a read-only parameter
		 */
		ParameterReadOnly = 52,
		/**
		 * Attempted to read from a write-only parameter
		 */
		ParameterWriteOnly = 53,
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
		 * Checksum comparison failed
		 */
		ChecksumFailed = 1,
		/**
		 * Address of a memory is out of the defined range for the type of memory
		 */
		AddressOutOfRange = 2,
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
	 * @todo See if this needs to include InternalErrorType
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
	static void reportProgressError(const Message& message, ExecutionProgressErrorType errorCode, uint8_t stepID);

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
