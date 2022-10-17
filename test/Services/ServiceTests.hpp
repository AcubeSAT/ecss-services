#ifndef ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
#define ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP

#include <vector>
#include <map>
#include <Message.hpp>
#include <ServicePool.hpp>

/**
 * Supporting class for tests against ECSS services
 *
 * @todo See if members of this class can be made non-static
 */
class ServiceTests {
protected:
	/**
	 * The list of Messages that have been sent as a result of all the processing.
	 *
	 * Whenever a Message is sent from anywhere in the code, it is stored in this array. The
	 * testing code can fetch these Messages using the ServiceTests::get() method.
	 */
	static std::vector<Message> queuedMessages;

	/**
	 * The list of Errors that the ErrorHandler caught.
	 *
	 * Whenever an Error is thrown anywhere in the code, it is collected in the thrownErrors
	 * array. Then, the user can tests whether or which types of errors were thrown, using
	 * the ServiceTests::hasNoErrors() and ServiceTests::thrownError() functions.
	 *
	 * A multimap with keys (ErrorHandler::ErrorSource, ErrorHandler::ErrorType) and values of `1`.
	 *
	 * @todo If errors get more complex, this should hold the complete error information
	 */
	static std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool> thrownErrors;

	/**
	 * Whether an error assertion function was called, indicating that we are expecting to see
	 * Errors thrown after this Message
	 */
	static bool expectingErrors;

public:
	/**
	 * Get a message from the list of queued messages to send
	 * @param number The number of the message, starting from 0 in chronological order
	 */
	static Message& get(uint64_t number) {
		return queuedMessages.at(number);
	}

	/**
	 * Add a message to the queue of messages having been sent
	 */
	static void queue(const Message& message) {
		queuedMessages.push_back(message);
	}

	/**
	 * Add one error to the list of occurred errors.
	 *
	 * @note This function will be called automatically by the ErrorHandler, and should not be
	 * used in tests.
	 *
	 * @param errorSource The source of the error.
	 * @param errorCode The integer code of the error, coming directly from one of the ErrorCode
	 * enumerations in ErrorHandler.
	 */
	static void addError(ErrorHandler::ErrorSource errorSource, uint16_t errorCode) {
		thrownErrors.emplace(std::make_pair(errorSource, errorCode), 1);
	}

	/**
	 * Counts the number of messages in the queue
	 */
	static uint64_t count() {
		return queuedMessages.size();
	}

	/**
	 * Checks that there is *exactly* one message in the list of queued messages
	 */
	static bool hasOneMessage() {
		return count() == 1;
	}

	static void resetErrors() {
		queuedMessages.clear();
		thrownErrors.clear();
		expectingErrors = false;
	}

	/**
	 * Reset the testing environment, starting from zero for all parameters
	 */
	static void reset() {
		resetErrors();

		Services.reset();
	}

	/**
	 * Return whether an error assertion function was called, which means that we are expecting this
	 * request to contain errors
	 * @return
	 */
	static bool isExpectingErrors() {
		return expectingErrors;
	}

	/**
	 * Find if there are *no* thrown errors
	 * @return True if 0 errors were thrown after the message
	 * @todo Implement a way to run this assertion at the end of every test
	 */
	static bool hasNoErrors() {
		return thrownErrors.empty();
	}

	/**
	 * Find the number of thrown errors after the processing of this Message.
	 */
	static uint64_t countErrors() {
		expectingErrors = true;

		return thrownErrors.size();
	}

	/**
	 * Find if an error exists
	 * @tparam ErrorType An enumeration of ErrorHandler
	 * @param errorType The error code of the Error, corresponding to the correct type as
	 * specified in ErrorHandler
	 */
	template <typename ErrorType>
	static bool thrownError(ErrorType errorType) {
		ErrorHandler::ErrorSource errorSource = ErrorHandler::findErrorSource(errorType);

		expectingErrors = true;

		return thrownErrors.find(std::make_pair(errorSource, errorType)) != thrownErrors.end();
	}
	/**
 	 * Find the number of times that an error exists
 	 * @tparam ErrorType An enumeration of ErrorHandler
 	 * @param errorType The error code of the Error, corresponding to the correct type as
 	 * specified in ErrorHandler
 	 */
	template <typename ErrorType>
	static int countThrownErrors(ErrorType errorType) {
		ErrorHandler::ErrorSource errorSource = ErrorHandler::findErrorSource(errorType);

		expectingErrors = true;

		return thrownErrors.count(std::make_pair(errorSource, errorType));
	}

	/**
	 * Get the list of all thrown errors
	 */
	static std::vector<std::pair<ErrorHandler::ErrorSource, uint16_t>> getThrownErrors() {
		std::vector<std::pair<ErrorHandler::ErrorSource, uint16_t>> errors;

		for (auto error : thrownErrors) {
			errors.push_back(error.first);
		}

		return errors;
	}
};

#endif // ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
