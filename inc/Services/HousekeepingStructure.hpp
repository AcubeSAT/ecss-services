#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"

#define ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT 10

class HousekeepingStructure {
public:

	uint8_t structureId;

	// vector storing the id's of the parameters contained in this structure, maybe needs to be a map for lookup
	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> containedParametersIdVec;

	// vector containing Ids of parameters with a single value sampled
	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> simplyCommutatedIdsVec;

	// vector containing pairs, every pair has the NUM_SAMPLES followed by a list of parameter IDs that gathered
	// NUM_SAMPLES samples
	etl::vector <std::pair <uint16_t, etl::vector <uint16_t, ECSS_MAX_PARAMETERS>>, ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT>
	    superCommutatedIdsVec;
};

#endif