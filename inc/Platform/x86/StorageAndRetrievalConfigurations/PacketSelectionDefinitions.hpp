#ifndef ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP
#define ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/map.h"

/**
 * All necessary definitions for the packet selection subservice, storage control configuration in the Storage and
 * Retrieval Service (ST[15])
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */

/**
 * Application Process Definitions, each containing service type definitions, each containing report type definitions.
 * The Application Process Definitions are used by the Application Process Storage Control configuration.
 */
class ApplicationProcessDefinition {
public:
	typedef etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypeDefinitions;
	etl::map <uint16_t, reportTypeDefinitions, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypeDefinitions;
};

/**
 * Housekeeping Parameter Report Definitions, each containing a vector of the housekeeping structure ids.
 * The Housekeeping Parameter Report Definitions are used by the Housekeeping Parameter Report storage control
 * configuration.
 */
class HousekeepingDefinition {
public:
	etl::vector <std::pair <uint16_t, uint16_t>, ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL> housekeepingStructIds;
};

/**
 * Event Report Blocking Definitions, each containing a vector of Event Definition Ids. The Event Report Blocking
 * definitions are used by the Event Report Blocking Storage Control configuration.
 */
class EventDefinition {
public:
	etl::vector <uint16_t, ECSS_MAX_EVENT_DEFINITION_IDS> eventDefinitionIds;
};

#endif