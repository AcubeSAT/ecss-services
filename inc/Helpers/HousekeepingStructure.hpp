#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "Helpers/Parameter.hpp"

/**
 * Implementation of the Housekeeping report structure used by the Housekeeping Reporting Subservice (ST[03]). The
 * current version includes only simply commutated parameters, i.e. parameters that contain a single sampled value.
 *
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
	 * Indicates whether the periodic generation of housekeeping reports is enabled.
	 */
	bool periodicGenerationActionStatus = false;

	/**
	 * Map containing references to simply commutated housekeeping parameters. The ID of each parameter is used as a
	 * key.
	 */
	etl::map<uint16_t, std::reference_wrapper<ParameterBase>, ECSS_MAX_PARAMETERS> simplyCommutatedParameters;

	HousekeepingStructure() = default;
};

#endif