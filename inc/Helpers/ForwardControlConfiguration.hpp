#ifndef ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP
#define ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/map.h"
#include "Helpers/Parameter.hpp"

/**
 * Implements the Real Time Forward Control configuration, which includes three separate configurations, Application
 * Process configuration, Housekeeping configuration and Event Report configuration. These configurations contain
 * definitions, which indicate whether a telemetry message should be forwarded to the ground station.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */

/**
 * The Application Process configuration. It's basically a map, storing a vector of report type definitions for each
 * pair of (applicationID, serviceType).
 */
class ApplicationProcessConfiguration {
public:
	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> reportTypeDefinitions;

	/**
	 * This is the key for the application process configuration map. It contains a pair with the applicationID and
	 * the serviceType.
	 */
	typedef std::pair<uint8_t, uint8_t> appServiceKey;

	/**
	 * Map containing the Application Process definitions. Each application has its own ID. The combination of the
	 * application ID and the service type is used as a key to provide access to the list of report type definitions.
	 */
	etl::map<appServiceKey, reportTypeDefinitions,
	         ECSSMaxControlledApplicationProcesses * ECSSMaxServiceTypeDefinitions>
	    definitions;

	ApplicationProcessConfiguration() = default;
};

#endif
