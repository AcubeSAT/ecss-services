#ifndef PROJECT_ERRORHANDLER_HPP
#define PROJECT_ERRORHANDLER_HPP

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
	 * Log the error to a logging facility. Currently, this just displays the error on the screen.
	 *
	 * @todo This function MUST be moved as platform-dependent code. Currently, it uses g++ specific
	 * functions for desktop.
	 */
	template<typename ErrorType>
	static void logError(const Message &message, ErrorType errorType);

	/**
	 * Log an error without a Message to a logging facility. Currently, this just displays the error
	 * on the screen.
	 *
	 * @todo This function MUST be moved as platform-dependent code. Currently, it uses g++ specific
	 * functions for desktop.
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
		 * Asked a Message type that it doesn't exist
		 */
			UnknownMessageType = 6,
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
	 * The error code for failed completion of execution reports, as specified in ECSS 5.3.5.2.3g
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum ExecutionErrorType {
		UnknownExecutionError = 0,
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
	 * The error code for failed completion of execution reports, as specified in ECSS 6.1.3.3d
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
	 * 					 ExecutionErrorType, or RoutingErrorType.
	 * @param message The incoming message that prompted the failure
	 * @param errorCode The error's code, as defined in ErrorHandler
	 * @todo See if this needs to include InternalErrorType
	 */
	template<typename ErrorType>
	static void reportError(const Message &message, ErrorType errorCode);

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
};

#endif //PROJECT_ERRORHANDLER_HPP
