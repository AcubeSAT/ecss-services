#ifndef ECSS_SERVICES_SERVICEPOOL_HPP
#define ECSS_SERVICES_SERVICEPOOL_HPP

#include <Services/TimeBasedSchedulingService.hpp>
#include "Services/LargePacketTransferService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/TimeManagementService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/TestService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/FunctionManagementService.hpp"

/**
 * Defines a class that contains instances of all Services.
 *
 * All Services should be stored here and should not be instantiated in a different way.
 *
 * @todo Find a way to disable services which are not used
 */
class ServicePool {
	/**
	 * A counter for messages
	 *
	 * Each key-value pair corresponds to one MessageType within a Service. The most significant 8 bits are the number
	 * of the service, while the least significant 8 bits are the number of the Message.
	 *
	 * @todo Update this according to the final number of Services and Messages
	 */
	etl::map<uint16_t, uint16_t, 10*20> messageTypeCounter;
public:
	RequestVerificationService requestVerification;
	EventReportService eventReport;
	MemoryManagementService memoryManagement;
	TimeManagementService timeManagement;
	EventActionService eventAction;
	TestService testService;
	ParameterService parameterManagement;
	LargePacketTransferService largePacketTransferService;
	FunctionManagementService functionManagement;
	TimeBasedSchedulingService timeBasedScheduling;

	/**
	 * The default ServicePool constructor
	 */
	ServicePool() = default;

	/**
	 * Reset all the services and their contents/properties to the original values
	 *
	 * @note This performs the reset in-place, i.e. no new memory is allocated. As such, all
	 * Services already stored as values will point to the "new" Services after a reset.
	 */
	void reset();

	/**
	 * Get and increase the "message type counter" for the next message of a type
	 *
	 * The message type counter counts the type of generated messages per destination, according to requirement
	 * 5.4.2.1j. If the value reaches its max, it is wrapped back to 0.
	 *
	 * @param serviceType The service type ID
	 * @param messageType The message type ID
	 * @return The message type count
	 */
	uint16_t getMessageTypeCounter(uint8_t serviceType, uint8_t messageType);
};

/**
 * A global variable that defines the basic pool where services can be fetched from
 */
extern ServicePool Services;

#endif // ECSS_SERVICES_SERVICEPOOL_HPP
