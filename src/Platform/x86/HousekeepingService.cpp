#include "ECSS_Configuration.hpp"
#ifdef SERVICE_HOUSEKEEPING

#include "Services/HousekeepingService.hpp"
#include "Parameters/PlatformParameters.hpp"

void HousekeepingService::initializeHousekeepingStructures() {
	HousekeepingStructure structure;
	structure.structureId = 1;
	structure.periodicGenerationActionStatus = true;
	structure.collectionInterval = 500;
	structure.simplyCommutatedParameterIds =
	    etl::vector<uint16_t, ECSSMaxSimplyCommutatedParameters>{PlatformParameters::parameter1.getValue()};
	housekeepingStructures.insert({structure.structureId, structure});
}

#endif