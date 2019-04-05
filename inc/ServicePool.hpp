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
};

/**
 * A global variable that defines the basic pool where services can be fetched from
 */
extern ServicePool Services;


#endif //ECSS_SERVICES_SERVICEPOOL_HPP
