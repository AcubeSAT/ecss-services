#include <iostream>
#include "Helpers/Statistic.hpp"

uint64_t Statistic::currentTimeToCustomCUC() {
	UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0); // todo: get the current time for max value.
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> time(currentTime);
	return time.asCustomCUCTimestamp().elapsed100msTicks;
}

void Statistic::updateStatistics(double value) {
	/*
	 * TODO:
	 *      if periodic, just calculate next time without the CUC
	 *      function.
	 * */

	if (value > max) {
		max = value;
		maxTime = currentTimeToCustomCUC();
	}
	if (value < min) {
		min = value;
		minTime = currentTimeToCustomCUC();
	}
	if (sampleCounter + 1 > 0) {
		mean = (mean * sampleCounter + value) / (sampleCounter + 1);
	}
	sumOfSquares += pow(value, 2);
	sampleCounter++;
}

void Statistic::appendStatisticsToMessage(Message& report) {
	report.appendFloat(static_cast<float>(max));
	report.appendUint32(maxTime);
	report.appendFloat(static_cast<float>(min));
	report.appendUint32(minTime);
	report.appendFloat(static_cast<float>(mean));

	if (SupportsStandardDeviation) {
		double standardDeviation = 0;
		if (sampleCounter == 0) {
			standardDeviation = 0;
		} else {
			double meanOfSquares = sumOfSquares / sampleCounter;
			standardDeviation = sqrt(abs(meanOfSquares - pow(mean, 2)));
		}
		report.appendFloat(static_cast<float>(standardDeviation));
	}
}

void Statistic::setSelfSamplingInterval(uint16_t samplingInterval) {
	this->selfSamplingInterval = samplingInterval;
}

void Statistic::resetStatistics() {
	max = -std::numeric_limits<double>::infinity();
	min = std::numeric_limits<double>::infinity();
	maxTime = 0;
	minTime = 0;
	mean = 0;
	sumOfSquares = 0;
	sampleCounter = 0;
}

bool Statistic::statisticsAreInitialized() {
	return (sampleCounter == 0 and mean == 0 and sumOfSquares == 0 and maxTime == 0 and minTime == 0 and
	        max == -std::numeric_limits<double>::infinity() and min == std::numeric_limits<double>::infinity());
}

