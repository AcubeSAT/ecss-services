#ifndef ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
#define ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP

#include <vector>
#include <Message.hpp>
#include <ErrorHandler.hpp>

/**
 * Supporting class for tests against ECSS services
 *
 * @todo See if members of this class can be made non-static
 */
class ServiceTests {
	static std::vector<Message> queuedMessages;
	static std::vector<ErrorHandler::InternalErrorType> internalErrors;

public:
	/**
	 * Get a message from the list of queued messages to send
	 * @param number The number of the message, starting from 0 in chronological order
	 */
	static Message &get(uint64_t number) {
		return queuedMessages.at(number);
	}

	/**
	 * Get an error from the list of logged errors
	 * @param number The number of the error, starting from 0 in chronological order
	 */
	static ErrorHandler::InternalErrorType &getError(uint64_t number) { // uint32 just for
		// overloading
		return internalErrors.at(number);
	}

	/**
	 * Add a message to the queue of messages to be sent
	 */
	static void queue(const Message &message) {
		queuedMessages.push_back(message);
	}

	/**
	 * Add an error to the queue of logged errors
	 */
	static void storeError(const ErrorHandler::InternalErrorType &error) {
		internalErrors.push_back(error);
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

	/**
	 * Remove all the queued messages and errors from the lists, starting over from 0 items again
	 */
	static void reset() {
		queuedMessages.clear();
		internalErrors.clear();
	}
};

#endif //ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
