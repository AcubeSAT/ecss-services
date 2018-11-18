#ifndef ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP
#define ECSS_SERVICES_TESTS_SERVICES_SERVICETESTS_HPP

#include <vector>
#include <Message.hpp>

/**
 * Supporting class for tests against ECSS services
 *
 * @todo See if members of this class can be made non-static
 */
class ServiceTests {
	static std::vector<Message> queuedMessages;

public:
	/**
	 * Get a message from the list of queued messages to send
	 * @param number The number of the message, starting from 0 in chronological order
	 */
	static Message& get(unsigned long number) {
		return queuedMessages.at(number);
	}

	/**
	 * Add a message to the queue of messages to be sent
	 */
	static void queue(const Message &message) {
		queuedMessages.push_back(message);
	}

	/**
	 * Counts the number of messages in the queue
	 */
	static unsigned long count() {
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
