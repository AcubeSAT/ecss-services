#ifndef PROJECT_ERRORHANDLER_HPP
#define PROJECT_ERRORHANDLER_HPP

#include <type_traits>

// Forward declaration of the class, since its header file depends on the ErrorHandler
class Message;

#include <stdint.h> // for the uint_8t stepID

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
	template<typename ErrorType>
	static void logError(const Message &message, ErrorType errorType);

	/**
	 * Log an error without a Message to a logging facility. Platform-dependent.
	 */
	template<typename ErrorType>
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
			UnacceptableMessage = 5,
	};

	/**
	 * The error code for failed start of execution reports, as specified in ECSS 5.3.5.2.3g
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum ExecutionStartErrorType {
		UnknownExecutionStartError = 0,
		FailedStartOfExecutionError = 1,
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
		UnknownRoutingError = 0
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
		Routing
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
	template<typename ErrorType>
	static void reportError(const Message &message, ErrorType errorCode);

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
	static void reportProgressError(const Message &message, ExecutionProgressErrorType errorCode,
	                                uint8_t stepID);

	/**
	 * Report a failure that occurred internally, not due to a failure of a received packet.
	 *
	 * Note that these errors correspond to bugs or faults in the software, and should be treated
	 * differently. Such an error may prompt a task or software reset.
	 *
	 */
	static void reportInternalError(InternalErrorType errorCode);

	/**
	 * Make an assertion, to ensure that a runtime condition is met.
	 *
	 * Reports a failure that occurred internally, not due to a failure of a received packet.
	 *
	 * Creates an error if \p condition is false. The created error is Internal.
	 */
	static void assertInternal(bool condition, InternalErrorType errorCode) {
		if (not condition) {
			reportInternalError(errorCode);
		}
	}

	/**
	 * Make an assertion, to ensure that a runtime condition is met.
	 *
	 * Reports a failure that occurred while processing a request, in any of the process phases.
	 *
	 * Creates an error if \p condition is false. The created error corresponds to a \p message.
	 */
	template<typename ErrorType>
	static void assertRequest(bool condition, const Message &message, ErrorType errorCode) {
		if (not condition) {
			reportError(message, errorCode);
		}
	}

	/**
	 * Convert a parameter given in C++ to an ErrorSource that can be easily used in comparisons.
	 * @tparam ErrorType One of the enums specified in ErrorHandler.
	 * @param error An error code of a specific type
	 * @return The corresponding ErrorSource
	 */
	template<typename ErrorType>
	inline static ErrorSource findErrorSource(ErrorType error) {
		// While this may seem like a "hacky" way to convert enums to ErrorSource, it should be
		// optimised by the compiler to constant time.

		if (std::is_same<ErrorType, AcceptanceErrorType>()) {
			return Acceptance;
		}
		if (std::is_same<ErrorType, ExecutionStartErrorType>()) {
			return ExecutionStart;
		}
		if (std::is_same<ErrorType, ExecutionProgressErrorType>()) {
			return ExecutionProgress;
		}
		if (std::is_same<ErrorType, ExecutionCompletionErrorType>()) {
			return ExecutionCompletion;
		}
		if (std::is_same<ErrorType, RoutingErrorType>()) {
			return Routing;
		}
		return Internal;
	}
};

#endif //PROJECT_ERRORHANDLER_HPP
