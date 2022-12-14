#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Service.hpp"
#include "TimeGetter.hpp"
#include "etl/vector.h"

/**
 * Class containing all the statistics for every parameter. Includes functions that calculate and append the
 * statistics to messages
 */
class Statistic {
public:
	uint16_t selfSamplingInterval = 0;
	uint16_t sampleCounter = 0;
	Time::DefaultCUC timeOfMaxValue;
	Time::DefaultCUC timeOfMinValue;
	double max = -std::numeric_limits<double>::infinity();
	double min = std::numeric_limits<double>::infinity();
	double sumOfSquares = 0;
	double mean = 0;

	Statistic() = default;

	/**
	 * Gets the value from the sensor as argument and updates the statistics without storing it
	 * @param value returned value from "getValue()" of Parameter.hpp, i.e. the last sampled value from a parameter
	 */
	void updateStatistics(double value);

	/**
	 * Resets all statistics calculated to default values
	 */
	void resetStatistics();

	/**
	 * Appends itself to the received Message
	 * message.
	 */
	void appendStatisticsToMessage(Message& report) const;

	/**
	 * Setter function
	 */
	void setSelfSamplingInterval(uint16_t samplingInterval);

	/**
	 * Check if all the statistics are initialized
	 */
	bool statisticsAreInitialized() const;
};

#endif
