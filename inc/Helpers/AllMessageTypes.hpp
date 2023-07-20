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
 * todo: needs to be updated after the implementation of the remaining ecss services.
 */
namespace AllMessageTypes {
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st01Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st03Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st04Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st05Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st06Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st11Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st13Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st17Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st19Messages;
	extern etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st20Messages;

	/**
	 * Map containing all the message types, per service. The key is the ServiceType and the value,
	 * an etl vector containing the message types.
	 */
	extern const etl::map<uint8_t, etl::vector<uint8_t, ECSSMaxReportTypeDefinitions>, ECSSMaxServiceTypeDefinitions> MessagesOfService;

} // namespace AllMessageTypes

#endif
