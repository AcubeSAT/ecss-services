#ifndef ECSS_SERVICES_EVENTREPORTBLOCKINGCONFIGURATION_HPP
#define ECSS_SERVICES_EVENTREPORTBLOCKINGCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "PacketSelectionDefinitions.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * This is the Event Report Blocking configuration as defined in ST[15] of the standard.
 *
 * @brief it contains all the necessary Event Definitions, which contain the application process ID, each followed by
 * a list of the Event Definition IDs. These IDs define which Event Definition packets are going to be allowed in the
 * packet store.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class EventReportBlockingConfiguration {
public:
	typedef etl::map <uint16_t, EventDefinition, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS>
	    applicationProcessDefinitions;

	etl::map <String <ECSS_MAX_STRING_SIZE>, applicationProcessDefinitions, ECSS_MAX_PACKET_STORES> definitions;
};

#endif