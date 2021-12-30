#ifndef ECSS_SERVICES_SERVICEPOOL_HPP
#define ECSS_SERVICES_SERVICEPOOL_HPP

#include "ECSS_Configuration.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/TestService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/FileManagementService.hpp"
/**
 * Defines a class that contains instances of all Services.
 *
 * All Services should be stored here and should not be instantiated in a different way.
 */
class ServicePool {
	/**
	 * A counter for messages
	 *
	 * Each key-value pair corresponds to one MessageType within a Service. For the key, the most significant 8 bits are
     * the number of the service, while the least significant 8 bits are the number of the Message. The value is the
     * counter of each MessageType.
	 */
	etl::map<uint16_t, uint16_t, ECSS_TOTAL_MESSAGE_TYPES> messageTypeCounter;

	/**
	 * A counter for messages that corresponds to the total number of TM packets sent from an APID
	 */
	uint16_t packetSequenceCounter = 0;
public:
#ifdef SERVICE_EVENTACTION
	EventActionService eventAction;
#endif

#ifdef SERVICE_EVENTREPORT
	EventReportService eventReport;
#endif

#ifdef SERVICE_FUNCTION
	FunctionManagementService functionManagement;
#endif

#ifdef SERVICE_LARGEPACKET
	LargePacketTransferService largePacketTransferService;
#endif

#ifdef SERVICE_MEMORY
	MemoryManagementService memoryManagement;
#endif

#ifdef SERVICE_PARAMETER
	ParameterService parameterManagement;
#endif

#ifdef SERVICE_REQUESTVERIFICATION
	RequestVerificationService requestVerification;
#endif

#ifdef SERVICE_TEST
	TestService testService;
#endif

#ifdef SERVICE_TIMESCHEDULING
	TimeBasedSchedulingService timeBasedScheduling;
#endif

#ifdef SERVICE_FILE_MANAGEMENT
    FileManagementService fileManagementService;
#endif

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
	uint16_t getAndUpdateMessageTypeCounter(uint8_t serviceType, uint8_t messageType);

	/**
	 * Get and increase the "packet sequence count" for the next message
	 *
	 * The packet sequence count is incremented each time a packet is released, with a maximum value of 2^14 - 1
	 *
	 * @return The packet sequence count
	 */
	uint16_t getAndUpdatePacketSequenceCounter();
};

/**
 * A global variable that defines the basic pool where services can be fetched from
 */
extern ServicePool Services;

#endif // ECSS_SERVICES_SERVICEPOOL_HPP
