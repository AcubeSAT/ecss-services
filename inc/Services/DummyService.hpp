#ifndef ECSS_SERVICES_DUMMYSERVICE_HPP
#define ECSS_SERVICES_DUMMYSERVICE_HPP

#include "Logger_Definitions.hpp"
#include "Service.hpp"

/**
 * This is a dummy Service used during testing. Its functionality is to contain LOG_ data but be sent through CAN bus.
 * During the environmental tests, we will use both CAN and UART to send data from the MCU to the PC. However, the
 * current CAN protocol does not accommodate random data strings. Therefore, we will use this dummy service to send such
 * messages.
 *
 * Per the ECSS-E-ST-70-41C standard, p. 27-28, custom Services and Messages should start above 127.
 */
class DummyService : public Service {
private:
	etl::string<1> termination = "\n";

public:
	inline static const uint8_t ServiceType = 128;
	enum MessageType : uint8_t {
		LogString = 128,
	};

	DummyService() {
		serviceType = ServiceType;
	}

	/**
     * Send data as a part of a custom ECSS Message
     * Creates a TM[128, 128]
     */
	void logAsECSSMessage(const etl::string<LOGGER_MAX_MESSAGE_SIZE>& data) {
		Message log = createTM(MessageType::LogString);
		log.appendString(data);
		log.appendString(termination);
		storeMessage(log);
	}
};

#endif //ECSS_SERVICES_DUMMYSERVICE_HPP
