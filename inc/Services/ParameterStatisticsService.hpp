#ifndef ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP

#define MAX_NUM_OF_DEFINITIONS 10

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Parameter.hpp"
#include "Statistic.hpp"
#include "etl/deque.h"

/**
 * Implementation of the ST[04] parameter management service,
 * as defined in ECSS-E-ST-70-41C. Regarding the variable types
 * 0 means uint8, 1 means uint16 and 2 means uint32 (watch the implementation
 * of TC[02]).
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */

class ParameterStatisticsService : public Service {
public:

	//TODO: implement the statistic parsing from the dequeue and the resetting of each parameter's vector after
	//      resetFlag or periodic reset.

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

	bool periodicStatisticsReportingStatus = false;     // 1 means that periodic reporting is enabled
	bool hasAutomaticStatisticsReset = false;
	uint16_t periodicStatisticsReportingInterval;
	uint16_t numOfStatisticsDefinitions = 0;
	uint16_t nonDefinedStatistics = 0;

	etl::vector <std::reference_wrapper <StatisticBase>, ECSS_MAX_PARAMETERS> parameterStatisticsVector;

	ParameterStatisticsService() = default;
	/**
	 * This function receives a TM[4,1] packet and
	 * returns a TM[4,2] packet containing the parameter
	 * statistics report.
	 *
	 * @param resetFlag: a TC[4, 1] packet carrying a boolean value
	 */
	void reportParameterStatistics(Message& resetFlag);

	/**
	 * TC[4,3] reset parameter statistics
	 *
	 * @param reset: boolean value. Was not actually specified in the
	 * datasheet, will have to be discussed.
	 */
	void resetParameterStatistics(Message& reset);

	/**
	 * TC[4,4] enable periodic parameter reporting
	 *
	 * @param request: contains a boolean value
	 */
	void enablePeriodicStatisticsReporting(Message& request);

	/**
	 * TM[4,5] disable periodic parameter reporting
	 *
	 * @param request: contains a boolean value
	 */
	void disablePeriodicStatisticsReporting(Message& request);

	/**
	 * TM[4,6] add or update parameter statistics definitions
	 *
	 * @param paramIds: Ids of the parameters. Could be followed by intervals.
	 */
	void addOrUpdateStatisticsDefinitions(Message& paramIds);

	/**
	 * TM[4,7] delete parameter statistics definitions
	 *
	 * @param paramIds: Ids of the parameters
	 */
	void deleteStatisticsDefinitions(Message& paramIds);

	/**
	 * This function receives a TM[4,8] packet and
	 * returns a TM[4,9] packet containing the parameter
	 * statistics definitions report.
	 *
	 * @param
	 */
	void reportStatisticsDefinitions(Message& request);
	int test(Statistic <int> stat);
};

#endif