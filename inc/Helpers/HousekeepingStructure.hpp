#ifndef ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSTRUCTURE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/vector.h"

/**
 * Implementation of the Housekeeping report structure used by the Housekeeping Reporting Subservice (ST[03]). The
 * current version includes only simply commutated parameters, i.e. parameters that contain a single sampled value.
 *
 * @author Petridis Konstantinos <petridkon@gmail.com>
 */
struct HousekeepingStructure {
	ParameterReportStructureId structureId = 0;

	/**
     * Defined as integer multiples of the minimum sampling interval as per 6.3.3.2.c.5 #NOTE-2.
     */
	CollectionInterval collectionInterval = 0;

	/**
     * Indicates whether the periodic generation of housekeeping reports is enabled.
     */
	bool periodicGenerationActionStatus = false;

	/**
     * Vector containing the IDs of the simply commutated parameters, contained in the housekeeping structure.
     */
	etl::vector<ParameterId, ECSSMaxSimplyCommutatedParameters> simplyCommutatedParameterIds;

	HousekeepingStructure() = default;
};

#endif
