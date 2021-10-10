#ifndef ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameters/SystemParameters.hpp"
#include "Statistics/SystemStatistics.hpp"
#include "Services/HousekeepingStructure.hpp"
#include "etl/unordered_set.h"

class HousekeepingService : Service {
public:

	inline static const uint8_t ServiceType = 3;

	uint16_t structureIdCounter = -1;    // Helps track if a structure already exists or not

	// Here we save the housekeeping structs after the corresponding TC
	etl::array <HousekeepingStructure, ECSS_MAX_HOUSEKEEPING_STRUCTS> housekeepingStructuresArray;
	etl::unordered_set <uint16_t, ECSS_MAX_HOUSEKEEPING_STRUCTS> alreadyUsedStructIds;

	enum MessageType : uint8_t {
		ReportHousekeepingParameters = 1,
		HousekeepingParametersReport = 2,
		EnableHousekeepingParametersReport = 3,
		DisableHousekeepingParametersReport = 4,
		CreateHousekeepingReportStructure = 5,
		DeleteHousekeepingReportStructure = 6,

	};

	/**
	 * This function is the implementation of TM[3,25]. It generates a housekeeping parameters report with an
	 * appropriate notification.
	 */
	void housekeepingParametersReport(Message& structId);

	/**
	 * This func should be called by a FreeRTOS task and periodically report the housekeeping structures for which the
	 * periodic reporting is enabled as per 6.3.3.3(d).
	 */
	void periodicHousekeepingParameterReport();

	/**
	 * Implementation of TM[3,5]. Request to enable the periodic housekeeping parameters reporting for a specific
	 * housekeeping structure.
	 */
	void enablePeriodicHousekeepingParametersReport(Message& request);

	/**
	 * Implementation of TM[3,6]. Request to disable the periodic housekeeping parameters reporting for a specific
	 * housekeeping structure.
	 */
	void disablePeriodicHousekeepingParametersReport(Message& request);

	/**
	 * Implementation of TC[3,1]. Request to create a housekeeping parameters report structure.
	 */
	void createHousekeepingReportStructure(Message& request);

	/**
	 * Implementation of TC[3,3]. Request to delete a housekeeping parameters report structure.
	 */
	void deleteHousekeepingReportStructure(Message& request);
};

#endif