#ifndef ECSS_SERVICES_ALLREPORTTYPES_HPP
#define ECSS_SERVICES_ALLREPORTTYPES_HPP

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
 * Namespace containing all the report types for every service type.
 * For use in ST[14] and ST[15]
 */
namespace AllReportTypes {
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ServiceDefinitionsVector;
	const extern ServiceDefinitionsVector ST01Reports;
	const extern ServiceDefinitionsVector ST03Reports;
	const extern ServiceDefinitionsVector ST04Reports;
	const extern ServiceDefinitionsVector ST05Reports;
	const extern ServiceDefinitionsVector ST06Reports;
	const extern ServiceDefinitionsVector ST11Reports;
	const extern ServiceDefinitionsVector ST13Reports;
	const extern ServiceDefinitionsVector ST17Reports;
	const extern ServiceDefinitionsVector ST19Reports;
	const extern ServiceDefinitionsVector ST20Reports;

	/**
	 * Map containing all the report types, per service. The key is the ServiceType and the value,
	 * an etl vector containing the report types.
	 */
	extern const etl::map<uint8_t, ServiceDefinitionsVector, ECSSMaxServiceTypeDefinitions> MessagesOfService;

} // namespace AllReportTypes

#endif
