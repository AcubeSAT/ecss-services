#ifndef ECSS_SERVICES_HOUSEKEEPINGREPORTCONFIGURATION_HPP
#define ECSS_SERVICES_HOUSEKEEPINGREPORTCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * This is the Housekeeping Parameter Report configuration as defined in ST[15] of the standard.
 *
 * @brief it contains all the necessary housekeeping parameter report definitions, which contain the application
 * process ID, each followed by a list of the housekeeping structure IDs. These IDs define which housekeeping TM
 * packets are going to be allowed in the packet store.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class HousekeepingReportConfiguration {
public:
	typedef etl::map <uint16_t, HousekeepingDefinition, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS>
	    applicationProcessDefinitions;

	etl::map <String <ECSS_MAX_PACKET_STORE_ID_SIZE>, applicationProcessDefinitions, ECSS_MAX_PACKET_STORES> definitions;
};

#endif