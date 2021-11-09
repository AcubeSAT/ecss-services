#ifndef ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP
#define ECSS_SERVICES_PACKETSELECTIONDEFINITIONS_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/map.h"

class ApplicationProcessDefinition {
public:
	uint16_t packetStoreId;
	typedef etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypeDefinitions;
	etl::map <uint16_t, reportTypeDefinitions, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypeDefinitions;
};

#endif