#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "Parameter.hpp"

/**
 * Implementation of the Housekeeping report structure used by the Housekeeping Reporting Subservice (ST[03])
 * @author Petridis Konstantinos <petridkon@gmail.com>
 */
class HousekeepingStructure {
public:
	uint16_t structureId;
	/**
	 * Defined as units of minimum sampling interval.
	 */
	uint16_t collectionInterval = 0;
	/**
	 * Number of parameters with a single sample collected.
	 */
	uint16_t numOfSimplyCommutatedParams = 0;
	/**
	 * Indicates whether the periodic generation of housekeeping reports is enabled.
	 */
	bool periodicGenerationActionStatus = false;

	/**
	 * All the parameter Ids contained in the structure.
	 */
	etl::vector<uint16_t, ECSS_MAX_PARAMETERS> containedParameterIds;

	/**
	 * The IDs of the simply commutated parameters.
	 */
	etl::vector<uint16_t, ECSS_MAX_PARAMETERS> simplyCommutatedIds;

	/**
	 * Map containing references to housekeeping parameters. The ID of each parameter is used as a key.
	 */
	etl::map<uint16_t, std::reference_wrapper<ParameterBase>, ECSS_MAX_PARAMETERS> housekeepingParameters;

	HousekeepingStructure() = default;
};

#endif