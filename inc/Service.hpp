#ifndef ECSS_SERVICES_SERVICE_HPP
#define ECSS_SERVICES_SERVICE_HPP

#include <cstdint>
#include "Message.hpp"

/**
 * @defgroup Services Services
 * ECSS Services implementations, as defined in ECSS-E-ST-70-41C. These services receive TC Messages, and output TM
 * Messages.
 */

/**
 * A spacecraft service, as defined in ECSS-E-ST-70-41C
 *
 * A member of the Service class should be used as a singleton, i.e. must be created only once in
 * the code
 */
class Service {
private:
	uint16_t messageTypeCounter = 0;

protected:
	/**
	 * The service type of this Service. For example, ST[12]'s serviceType is `12`.
	 * Specify this value in the constructor of your service.
	 */
	uint8_t serviceType{};

	/**
	 * Creates a new empty telemetry package originating from this service
	 *
	 * @param messageType The ID of the message type, as specified in the standard. For example,
	 *                    the TC[17,3] message has `messageType = 3`.
	 * @todo See if the Message must be returned by reference
	 */
	Message createTM(uint8_t messageType) const {
		return Message(serviceType, messageType, Message::TM);
	}

	/**
	 * Stores a message so that it can be transmitted to the ground station
	 *
	 * Note: For now, since we don't have any mechanisms to queue messages and send them later,
	 * we just print the message to the screen
	 */
	void storeMessage(Message& message);

	/**
	 * This function declared only to remind us that every service must have a function like
	 * this, but this particular function does actually nothing.
	 */
	void execute(Message& message);

	/**
	 * Default protected constructor for this Service
	 */
	Service() = default;

public:
	/**
	 * @brief Unimplemented copy constructor
	 *
	 * Does not allow Services should be copied. There should be only one instance for each Service.
	 */
	Service(Service const&) = delete;

	/**
	 * Unimplemented assignment operation
	 *
	 * Does not allow changing the instances of Services, as Services are singletons.
	 */
	void operator=(Service const&) = delete;

	/**
	 * Default destructor
	 */
	~Service() = default;

	/**
	 * Default move constructor
	 */
	Service(Service&& service) noexcept = default;

	/**
	 * Default move assignment operator
	 */
	Service& operator=(Service&& service) noexcept = default;
};

#endif // ECSS_SERVICES_SERVICE_HPP
