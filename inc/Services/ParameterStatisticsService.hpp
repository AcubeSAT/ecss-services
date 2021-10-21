#ifndef ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameters/SystemParameters.hpp"
#include "etl/deque.h"
#include "Statistics/SystemStatistics.hpp"

/**
 * Implementation of the ST[04] parameter management service, as defined in ECSS-E-ST-70-41C.
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */

class ParameterStatisticsService : public Service {
public:

	inline static const uint8_t ServiceType = 4;

	enum MessageType : uint8_t {
		ReportParameterStatistics = 1,
		ParameterStatisticsReport = 2,
		ResetParameterStatistics = 3,
		EnablePeriodicParameterReporting = 4,
		DisablePeriodicParameterReporting = 5,
		AddOrUpdateParameterStatisticsDefinitions = 6,
		DeleteParameterStatisticsDefinitions = 7,
		ReportParameterStatisticsDefinitions = 8,
		ParameterStatisticsDefinitionsReport = 9,
	};

	/**
	 * true means that the periodic statistics reporting is enabled
	 */
	bool periodicStatisticsReportingStatus = false;
	/**
	 * If true, after every report reset the parameter statistics.
	 */
	const bool hasAutomaticStatisticsReset = false;
	/**
	 * Whether the statistics report messages include the standard deviation or not
	 */
	const bool supportsStandardDeviation = true;
	const bool supportsSamplingInterval = true;
	uint16_t reportingInterval = 5;   // Must define units. Same as parameter sampling rates
	const uint16_t numOfStatisticsDefinitions = 0;

	/**
	 * This function receives a TM[4,1] packet and returns a TM[4,2] packet containing the parameter statistics report.
	 */
	void reportParameterStatistics(Message& resetFlag);

	/**
	 * TC[4,3] reset parameter statistics, clearing all samples and values.
	 */
	void resetParameterStatistics();

	/**
	 * TC[4,4] enable periodic parameter reporting
	 */
	void enablePeriodicStatisticsReporting(Message& request);

	/**
	 * TM[4,5] disable periodic parameter reporting
	 */
	void disablePeriodicStatisticsReporting(Message& request);

	/**
	 * TM[4,6] add or update parameter statistics definitions
	 */
	void addOrUpdateStatisticsDefinitions(Message& request);

	/**
	 * TM[4,7] delete parameter statistics definitions. This version specifies the IDs of the parameters whose
	 * definitions are to be deleted.
	 */
	void deleteStatisticsDefinitions(Message& request);
	/**
	 * TM[4,7] This version deletes all definitions
	 */
	void deleteAllStatisticsDefinitions();

	/**
	 * This function receives a TM[4,8] packet and returns a TM[4,9] packet containing the parameter statistics
	 * definitions report.
	 */
	void reportStatisticsDefinitions(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);

};

extern ParameterStatisticsService parameterStatisticsService;

#endif