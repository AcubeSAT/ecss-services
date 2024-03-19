#ifndef ECSS_SERVICES_SERVICEPOOL_HPP
#define ECSS_SERVICES_SERVICEPOOL_HPP

#include "ECSS_Configuration.hpp"
#include "Services/DummyService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "Services/StorageAndRetrievalService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
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
	etl::map<uint16_t, uint16_t, ECSSTotalMessageTypes> messageTypeCounter;

	/**
	 * A counter for messages that corresponds to the total number of TM packets sent from an APID
	 */
	uint16_t packetSequenceCounter = 0;

	/**
	 * Maximum counter value for the packet sequence counter is 2^14 - 1. In `getAndUpdatePacketSequenceCounter
	 * ()`, the counter is increased by 1 and if it reaches the maximum value, it is reset to 0. There is a comparison
	 * that shifts 1 to the left by 14 bits to compare, and then reset.
	 * For more info, see: ECSS-E-ST-70-41C, Figure 7-6, the CCSDS packet overview
	 */
	inline static const uint8_t MaxPacketSequenceCounterBit = 14U;

public:
#ifdef SERVICE_DUMMY
	DummyService dummyService;
#endif

#ifdef SERVICE_EVENTACTION
	EventActionService eventAction;
#endif

#ifdef SERVICE_EVENTREPORT
	EventReportService eventReport;
#endif

#ifdef SERVICE_FUNCTION
	FunctionManagementService functionManagement;
#endif

#ifdef SERVICE_HOUSEKEEPING
	HousekeepingService housekeeping;
#endif

#ifdef SERVICE_LARGEPACKET
	LargePacketTransferService largePacketTransferService;
#endif

#ifdef SERVICE_MEMORY
	MemoryManagementService memoryManagement;
#endif

#ifdef SERVICE_ONBOARDMONITORING
	OnBoardMonitoringService onBoardMonitoringService;
#endif

#ifdef SERVICE_PARAMETER
	ParameterService parameterManagement;
#endif

#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	RealTimeForwardingControlService realTimeForwarding;
#endif

#ifdef SERVICE_PARAMETERSTATISTICS
	ParameterStatisticsService parameterStatistics;
#endif

#ifdef SERVICE_REQUESTVERIFICATION
	RequestVerificationService requestVerification;
#endif

#ifdef SERVICE_STORAGEANDRETRIEVAL
	StorageAndRetrievalService storageAndRetrieval;
#endif

#ifdef SERVICE_TEST
	TestService testService;
#endif

#ifdef SERVICE_TIMESCHEDULING
	TimeBasedSchedulingService timeBasedScheduling;
#endif

#ifdef SERVICE_FILE_MANAGEMENT
    FileManagementService fileManagement;
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
	uint16_t getAndUpdateMessageTypeCounter(ServiceTypeNum serviceType, MessageTypeNum messageType);

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
extern ServicePool Services; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

#endif // ECSS_SERVICES_SERVICEPOOL_HPP
