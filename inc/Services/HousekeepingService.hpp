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

	etl::array <HousekeepingStructure, ECSS_MAX_HOUSEKEEPING_STRUCTS> housekeepingStructuresArray;
	/**
	 * For quick look-up at the existing structures in the service.
	 */
	etl::unordered_set <uint16_t, ECSS_MAX_HOUSEKEEPING_STRUCTS> existingStructIds;

	const bool supportsPeriodicGeneration = true;

	enum MessageType : uint8_t {
		ReportHousekeepingParameters = 1,
		HousekeepingParametersReport = 2,
		EnableHousekeepingParametersReport = 3,
		DisableHousekeepingParametersReport = 4,
		CreateHousekeepingReportStructure = 5,
		DeleteHousekeepingReportStructure = 6,
		ReportHousekeepingStructures = 7,
		HousekeepingStructuresReport = 8,
		AppendParametersToHousekeepingStructure = 9,
		ModifyCollectionIntervalOfStructures = 10,
		ReportHousekeepingPeriodicProperties = 11,
		HousekeepingPeriodicPropertiesReport = 12,
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

	/**
	 * This function takes a structure ID as argument and constructs a TM[3,10]-formatted report.
	 */
	void housekeepingStructureReport(Message& request);

	/**
	 * This function takes as argument a message type TC[3,9] and for every struct ID in it, calls the
	 * "housekeepingStructureReport" function.
	 */
	void reportHousekeepingStructures(Message& request);

	/**
	 * This function takes as argument a message type TC[3,27] and responds with a TM[3,25].
	 */
	void generateOneShotHousekeepingReport(Message& request);

	/**
	 * This function appends new parameters to an already existing housekeeping structure (TC[3,29]).
	 */
	void appendParametersToHousekeepingStructure(Message& newParams);

	/**
	 * This function modifies the collection interval of specified structures (TC[3,31]).
	 */
	void modifyCollectionIntervalOfStructures(Message& request);

	/**
	 * This function takes as argument a message type TC[3,33] and responds with a TM[3,35]. What it does is, report
	 * the periodic properties of each requested structure.
 	 */
	void housekeepingPeriodicPropertiesReport(Message& request);

};

#endif