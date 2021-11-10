#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include <cmath>
#include <cfloat>

/**
 * Class containing all the statistics for every parameter. Includes functions that calculate and append the
 * statistics to messages
 */
class Statistic {
public:
	uint16_t selfSamplingInterval = 0;
	uint16_t sampleCounter = 0;
	uint32_t maxTime = 0;
	uint32_t minTime = 0;   //TODO: CUC Format timestamp
	double max = 0;
	double min = 99;
	double sumOfSquares = 0;
	double mean = 0;
	double meanOfSquares = 0;
	double standardDeviation = 0;

	Statistic() = default;

	/**
	 * Gets the value from the sensor as argument and updates the statistics without storing it
	 * @param value: returned value from "getValue()" of Parameter.hpp, i.e the last sampled value from a parameter
	 */
	void updateStatistics(double value);

	/**
	 * Resets all statistics calculated to default values
	 */
	void resetStatistics();

	/**
	 * Gets a StatisticBase object, casts it into type Statistic, calculates its statistics and appends them to the
	 * message.
	 */
	void appendStatisticsToMessage(Message& report);

	/**
	 * Setter function
	 */
	void setSelfSamplingInterval(uint16_t samplingInterval);

};

#endif