#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/unordered_set.h"

/**
 * Implementation of the Housekeeping report structure used by the Housekeeping Reporting Subservice (ST[03])
 * @author Petridis Konstantinos <petridkon@gmail.com>
 */

class HousekeepingStructure {
public:

	uint8_t structureId;
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
	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> containedParameterIds;

	etl::vector <uint16_t, ECSS_MAX_PARAMETERS> simplyCommutatedIds;

	etl::vector <std::pair <uint16_t, etl::vector <uint16_t, ECSS_MAX_PARAMETERS>>, ECSS_MAX_SUPER_COMMUTATED_SETS_PER_HOUSEKEEPING_STRUCT>
	    superCommutatedIds;
};

#endif