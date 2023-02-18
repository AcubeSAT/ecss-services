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
struct HousekeepingStructure {
    uint8_t structureId;

    /**
     * Defined as integer multiples of the minimum sampling interval as per 6.3.3.2.c.5 #NOTE-2.
     */
    uint32_t collectionInterval = 0;

    /**
     * Indicates whether the periodic generation of housekeeping reports is enabled.
     */
    bool periodicGenerationActionStatus = false;

    /**
     * Vector containing the IDs of the simply commutated parameters, contained in the housekeeping structure.
     */
    etl::vector<uint16_t, ECSSMaxSimplyCommutatedParameters> simplyCommutatedParameterIds;

	HousekeepingStructure() = default;
};

#endif
