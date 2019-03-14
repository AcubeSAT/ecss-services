#ifndef ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
#define ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP

#include <vector>
#include <map>
#include <Message.hpp>

/**
 * Supporting class for tests against ECSS services
 *
 * @todo See if members of this class can be made non-static
 */
class ServiceTests {
	/**
	 * The list of Messages that have been sent as a result of all the processing.
	 */
	static std::vector<Message> queuedMessages;

	/**
	 * The list of Errors that the ErrorHandler caught
	 * @var A multimap with keys (ErrorSource, ErrorType) and values of 1
	 * @todo If errors get more complex, this should hold the complete error information
	 */
	static std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool> thrownErrors;

public:
	/**
	 * Get a message from the list of queued messages to send
	 * @param number The number of the message, starting from 0 in chronological order
	 */
	static Message &get(uint64_t number) {
		return queuedMessages.at(number);
	}

	/**
	 * Add a message to the queue of messages to be sent
	 */
	static void queue(const Message &message) {
		queuedMessages.push_back(message);
	}

	/**
	 * Add one error to the list of occurred errors.
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

	/**
	 * Remove all the queued messages from the list, starting over from 0 items again
	 */
	static void reset() {
		queuedMessages.clear();
	}
};

#endif //ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
