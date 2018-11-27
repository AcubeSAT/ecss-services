#ifndef ECSS_SERVICES_SERVICE_HPP
#define ECSS_SERVICES_SERVICE_HPP

#include <cstdint>
#include "Message.hpp"

/**
 * A spacecraft service, as defined in ECSS-E-ST-70-41C
 *
 * A member of the Service class should be used as a singleton, i.e. must be created only once in
 * the code
 *
 * @todo Disable copy constructor
 */
class Service {
private:
	uint16_t messageTypeCounter = 0;
protected:
	uint8_t serviceType{};

	/**
	 * Creates a new empty telemetry package originating from this service
	 *
	 * @param messageType The ID of the message type, as specified in the standard. For example,
	 *                    the TC[17,3] message has `messageType = 3`.
	 * @todo See if the Message must be returned by reference
	 * @todo Set the application ID to the current application
	 * @todo Use the messageTypeCounter
	 */
	Message createTM(uint8_t messageType) {
		return Message(serviceType, messageType, Message::TM, 0);
	}

	/**
	 * Stores a message so that it can be transmitted to the ground station
	 *
	 * Note: For now, since we don't have any mechanisms to queue messages and send them later,
	 * we just print the message to the screen
	 */
	void storeMessage(const Message& message);
};


#endif //ECSS_SERVICES_SERVICE_HPP
