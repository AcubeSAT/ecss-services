#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/unordered_set.h"

class HousekeepingStructure {
public:

	uint8_t structureId = -1;
	/**
	 * Defined as units of minimum sampling interval.
	 */
	uint16_t collectionInterval = 0;
	/**
	 * Num of parameters with a single sample collected.
	 */
	uint16_t numOfSimplyCommutatedParams = 0;
	/**
	 * Num of sets of super commutated parameters (with more than one samples collected).
	 */
	uint16_t numOfSuperCommutatedParameterSets = 0;
	bool periodicGenerationActionStatus = false;

	/**
	 * All the parameter Ids contained in the structure
	 */
	etl::unordered_set <uint16_t, ECSS_MAX_PARAMETERS> containedParameterIds;

	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> simplyCommutatedIdsVec;

	etl::vector <std::pair <uint16_t, etl::vector <uint16_t, ECSS_MAX_PARAMETERS>>, ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT>
	    superCommutatedIdsVec;
};

#endif