#ifndef ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP
#define ECSS_SERVICES_PARAMETERSTATISTICSSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/Statistic.hpp"
#include "Service.hpp"
#include "etl/deque.h"
#include "etl/map.h"

/**
 * Implementation of the ST[04] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 *
 * @ingroup Services
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class ParameterStatisticsService : public Service {
private:
	/**
	 * The time, at which the evaluation of statistics is initialized. It is basically the time when, the statistics
	 * are reset.
	 */
	uint64_t evaluationStartTime = 0;

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
	 * Map containing parameters' IDs followed by the statistics that correspond to the specified parameter
	 */
	etl::map<uint16_t, Statistic, ECSSMaxStatisticParameters> statisticsMap;

	/**
	 * true means that the periodic statistics reporting is enabled
	 */
	bool periodicStatisticsReportingStatus = false;
	/**
	 * If true, after every report reset the parameter statistics.
	 */
	bool hasAutomaticStatisticsReset = false; // todo: do const
	/**
	 * Indicates whether to append/read the sampling interval to/from message
	 */
	const bool supportsSamplingInterval = true;
	/**
	 * The parameter statistics reporting interval
	 */
	uint16_t reportingInterval = 5; // TODO: Must define units. Same as parameter sampling rates

	/**
	 * TC[4,1] report the parameter statistics, by calling parameterStatisticsReport()
	 */
	void reportParameterStatistics(Message& request);

	/**
	 * Constructs and stores a TM[4,2] packet containing the parameter statistics report.
	 */
	void parameterStatisticsReport();

	/**
	 * TC[4,3] reset parameter statistics, clearing all samples and values. This is the function called by TC from
	 * the GS.
	 */
	void resetParameterStatistics(Message& request);

	/**
	 * This function clears all the samples.
	 */
	void resetParameterStatistics();

	/**
	 * TC[4,4] enable periodic parameter statistics reporting
	 */
	void enablePeriodicStatisticsReporting(Message& request);

	/**
	 * TC[4,5] disable periodic parameter statistics reporting
	 */
	void disablePeriodicStatisticsReporting(Message& request);

	/**
	 * TC[4,6] add or update parameter statistics definitions
	 */
	void addOrUpdateStatisticsDefinitions(Message& request);

	/**
	 * TC[4,7] delete parameter statistics definitions.
	 */
	void deleteStatisticsDefinitions(Message& request);

	/**
	 * TC[4,8] report the parameter statistics definitions, by calling statisticsDefinitionsReport()
	 */
	void reportStatisticsDefinitions(Message& request);
	/**
	 * Constructs and stores a TM[4,9] packet containing the parameter statistics definitions report.
	 */
	void statisticsDefinitionsReport();

	/**
	 * Calls the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif
