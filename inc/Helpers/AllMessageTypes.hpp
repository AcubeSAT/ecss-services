#ifndef ECSS_SERVICES_ALLMESSAGETYPES_HPP
#define ECSS_SERVICES_ALLMESSAGETYPES_HPP

#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"

/**
 * Namespace containing all the message types for every service type.
 * For use in ST[14] and ST[15]
 */
namespace AllMessageTypes {
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ServiceDefinitionsVector;
	const extern ServiceDefinitionsVector ST01Messages;
	const extern ServiceDefinitionsVector ST03Messages;
	const extern ServiceDefinitionsVector ST04Messages;
	const extern ServiceDefinitionsVector ST05Messages;
	const extern ServiceDefinitionsVector ST06Messages;
	const extern ServiceDefinitionsVector ST11Messages;
	const extern ServiceDefinitionsVector ST13Messages;
	const extern ServiceDefinitionsVector ST17Messages;
	const extern ServiceDefinitionsVector ST19Messages;
	const extern ServiceDefinitionsVector ST20Messages;

	/**
	 * Map containing all the message types, per service. The key is the ServiceType and the value,
	 * an etl vector containing the message types.
	 */
	extern const etl::map<uint8_t, ServiceDefinitionsVector, ECSSMaxServiceTypeDefinitions> MessagesOfService;

} // namespace AllMessageTypes

#endif
