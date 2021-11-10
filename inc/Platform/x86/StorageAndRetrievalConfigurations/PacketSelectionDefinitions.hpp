#ifndef ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP
#define ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/map.h"

class ApplicationProcessDefinition {
public:
	typedef etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypeDefinitions;
	etl::map <uint16_t, reportTypeDefinitions, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypeDefinitions;
};

class HousekeepingDefinition {
public:
	etl::vector <std::pair <uint16_t, uint16_t>, ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL> housekeepingStructIds;
};

#endif