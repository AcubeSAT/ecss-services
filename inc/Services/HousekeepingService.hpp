#ifndef ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameters/SystemParameters.hpp"
#include "Statistics/SystemStatistics.hpp"
#include "Services/HousekeepingStructure.hpp"

#define ECSS_MAX_HOUSEKEEPING_STRUCTS 10

class HousekeepingService : Service {
public:

	inline static const uint8_t ServiceType = 3;

	uint16_t collectionInterval = 0;    // As units of minimum sampling interval
	uint16_t structureIdCounter = 0;    // Helps track if a structure already exists or not
	bool housekeepingPeriodicGenerationActionStatus = true;

	// Here we save the housekeeping structs after the corresponding TC
	etl::array <HousekeepingStructure, ECSS_MAX_HOUSEKEEPING_STRUCTS> housekeepingStructuresArray;

	enum MessageType : uint8_t {
		HousekeepingParameterReport = 1,
	};

	void housekeepingParameterReport(Message& structId);
};

#endif