#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include <cfloat>
#include <cmath>
#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Service.hpp"
#include "Time/Time.hpp"
#include "Time/TimeStamp.hpp"
#include "Time/UTCTimestamp.hpp"
#include "etl/vector.h"

/**
 * Class containing all the statistics for every parameter. Includes functions that calculate and append the
 * statistics to messages
 */
class Statistic {
private:
	/**
	 * Converts the current UTC time, to CUC formatted timestamp.
	 *
	 * @note
	 * The original format of the CUC (etl array of bits), is not used here, because it's easier to append
	 * a type uint64_t to a message object, rather than a whole array. Thus, we use the custom CUC format.
	 *
	 * @return CUC formatted as elapsed 100ms ticks.
	 * @see Time.hpp
	 */
	uint64_t currentTimeToCustomCUC();

public:
	uint16_t selfSamplingInterval = 0;
	uint16_t sampleCounter = 0;
	uint64_t maxTime = 0;
	uint64_t minTime = 0;
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
	void appendStatisticsToMessage(Message& report);

	/**
	 * Setter function
	 */
	void setSelfSamplingInterval(uint16_t samplingInterval);

	/**
	 * Check if all the statistics are initialized
	 */
	bool statisticsAreInitialized();
};

#endif
