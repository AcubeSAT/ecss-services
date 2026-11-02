#pragma once
#include "etl/vector.h"
#include "etl/flat_map.h"
#include "ECSS_Configuration.hpp"
#include "ECSS_Definitions.hpp"
#include "Helpers/TypeDefinitions.hpp"

/**
 * Namespace containing all the report types for every service type.
 * For use in ST[14] and ST[15]
 *
 * @note
 * This deviates from ECSS-E-ST-70-41C on purpose. In ECSS 6.14.3.2.2 and 6.15.4.2.2, the subservice has no knowledge of
 * the existing report types, and an empty list of definitions means that everything is allowed. Here, "add all report
 * types" is expanded to an explicit list from this table, which lists the report-generating services enabled in this
 * build. Those of a subsystem whose reports are controlled from here must be enabled here too.
 */
namespace AllReportTypes {
	typedef uint8_t ReportId;
	/**
	 * The number of services listed in MessagesOfService, i.e. the report-generating services enabled in this build.
	 */
	inline constexpr size_t ReportingServiceCount = 0 //cppcheck-suppress misra-c2012-2.2
#ifdef SERVICE_REQUESTVERIFICATION
	    + 1
#endif
#ifdef SERVICE_HOUSEKEEPING
	    + 1
#endif
#ifdef SERVICE_PARAMETERSTATISTICS
	    + 1
#endif
#ifdef SERVICE_EVENTREPORT
	    + 1
#endif
#ifdef SERVICE_MEMORY
	    + 1
#endif
#ifdef SERVICE_TIMESCHEDULING
	    + 1
#endif
#ifdef SERVICE_ONBOARDMONITORING
	    + 1
#endif
#ifdef SERVICE_LARGEPACKET
	    + 1
#endif
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	    + 1
#endif
#ifdef SERVICE_STORAGEANDRETRIEVAL
	    + 1
#endif
#ifdef SERVICE_TEST
	    + 1
#endif
#ifdef SERVICE_EVENTACTION
	    + 1
#endif
#ifdef SERVICE_PARAMETER
	    + 1
#endif
#ifdef SERVICE_FILE_MANAGEMENT
	    + 1
#endif
	    ;
	/**
	 * Vector to contain all report types defined for a service
	 */
	using ServiceDefinitionsVector = etl::vector<ReportId, ECSSMaxReportTypeDefinitions>;
#ifdef SERVICE_REQUESTVERIFICATION
	const extern ServiceDefinitionsVector ST01Reports;
#endif
#ifdef SERVICE_HOUSEKEEPING
	const extern ServiceDefinitionsVector ST03Reports;
#endif
#ifdef SERVICE_PARAMETERSTATISTICS
	const extern ServiceDefinitionsVector ST04Reports;
#endif
#ifdef SERVICE_EVENTREPORT
	const extern ServiceDefinitionsVector ST05Reports;
#endif
#ifdef SERVICE_MEMORY
	const extern ServiceDefinitionsVector ST06Reports;
#endif
#ifdef SERVICE_TIMESCHEDULING
	const extern ServiceDefinitionsVector ST11Reports;
#endif
#ifdef SERVICE_ONBOARDMONITORING
	const extern ServiceDefinitionsVector ST12Reports;
#endif
#ifdef SERVICE_LARGEPACKET
	const extern ServiceDefinitionsVector ST13Reports;
#endif
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	const extern ServiceDefinitionsVector ST14Reports;
#endif
#ifdef SERVICE_STORAGEANDRETRIEVAL
	const extern ServiceDefinitionsVector ST15Reports;
#endif
#ifdef SERVICE_TEST
	const extern ServiceDefinitionsVector ST17Reports;
#endif
#ifdef SERVICE_EVENTACTION
	const extern ServiceDefinitionsVector ST19Reports;
#endif
#ifdef SERVICE_PARAMETER
	const extern ServiceDefinitionsVector ST20Reports;
#endif
#ifdef SERVICE_FILE_MANAGEMENT
	const extern ServiceDefinitionsVector ST23Reports;
#endif

	/**
	 * Flat map containing all the report types, per service. The key is the ServiceType and the value
	 * is an etl vector containing the report types.
	 */
	extern const etl::flat_map<ServiceTypeNum, const ServiceDefinitionsVector&, ReportingServiceCount> MessagesOfService;

} // namespace AllReportTypes