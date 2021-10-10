#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/unordered_set.h"

#define ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT 10

class HousekeepingStructure {
public:

	uint8_t structureId = -1;
	uint16_t collectionInterval = 0;    // As units of minimum sampling interval
	uint16_t numOfSimplyCommutatedParams = 0;
	uint16_t numOfSuperCommutatedParameterSets = 0;
	bool periodicGenerationActionStatus = false;

	// vector storing the id's of the parameters contained in this structure, maybe needs to be a map for lookup
	etl::unordered_set <uint16_t, ECSS_MAX_PARAMETERS> containedParameterIds;

	// vector containing Ids of parameters with a single value sampled
	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> simplyCommutatedIdsVec;

	// vector containing pairs, every pair has the NUM_SAMPLES followed by a list of parameter IDs that gathered
	// NUM_SAMPLES samples
	etl::vector <std::pair <uint16_t, etl::vector <uint16_t, ECSS_MAX_PARAMETERS>>, ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT>
	    superCommutatedIdsVec;
};

#endif