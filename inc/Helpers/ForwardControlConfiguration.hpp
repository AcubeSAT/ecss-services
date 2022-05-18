#ifndef ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP
#define ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * The Application Process configuration. It's basically a map, storing a vector of report type definitions for each
 * pair of (applicationID, serviceType). It contains definitions, which indicate whether a telemetry message, produced
 * by a service, inside an application process (subsystem), should be forwarded to the ground station.
 */
class ApplicationProcessConfiguration {
public:
	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ReportTypeDefinitions;

	/**
	 * This is the key for the application process configuration map. It contains a pair with the applicationID and
	 * the serviceType.
	 */
	typedef std::pair<uint8_t, uint8_t> AppServiceKey;

	/**
	 * Map containing the report type definitions. Each application process has its own ID. The combination of the
	 * application ID and the service type is used as a key to provide access to the list of report type definitions.
	 *
	 * @note
	 * The report type definitions are basically the message types of each service. For example a message type for the
	 * 'ParameterStatisticsService' (ST04) is 'ParameterStatisticsService::MessageType::ParameterStatisticsReport'. The
	 * Real Time Forwarding Control Service (ST14) uses this map as a lookup table, to identify whether a requested
	 * triplet (app->service->message type) is allowed to be forwarded to the ground station via the corresponding virtual
	 * channel. The requested message type is only forwarded, if the requested application process ID and service type
	 * already exist in the map, and the requested report type is located in the vector of report types, which corresponds
	 * to the appID and service type.
	 */
	etl::map<AppServiceKey, ReportTypeDefinitions, ECSSMaxApplicationsServicesCombinations> definitions;

	ApplicationProcessConfiguration() = default;
};

#endif
