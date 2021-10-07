#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"

class HousekeepingStructure {
public:

	uint8_t structureId;
	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> containedParametersIdVec;
};

#endif