#include <iostream>
#include "Helpers/Statistic.hpp"

void Statistic::updateStatistics(double value) {
	/*
	* TODO:
	*      if periodic, just calculate next time without the CUC
	*      function. Very easy, but we cannot include the
	*      ParameterStatisticsService.hpp because that includes
	*      the Statistic.hpp file, not the other way around.
	*      There are plenty of ways to do that though!!
	* */

	if (value > max) {
		max = value;
		// TODO: maxTime = as_CUC_timestamp();
	}
	if (value < min) {
		min = value;
		// TODO: minTime = as_CUC_timestamp();
	}
	mean = (mean * sampleCounter + value) / (sampleCounter + 1);
	meanOfSquares = (meanOfSquares * sampleCounter + pow(value,2)) / (sampleCounter + 1);
	sumOfSquares += pow(value,2);
	standardDeviation = sqrt(meanOfSquares - pow(mean,2));
	sampleCounter++;
}

void Statistic::appendStatisticsToMessage(Message& report) {
	report.appendFloat(static_cast <float> (max));
	report.appendUint32(maxTime);
	report.appendFloat(static_cast <float> (min));
	report.appendUint32(minTime);
	report.appendFloat(static_cast <float> (mean));

	if (SUPPORTS_STANDARD_DEVIATION) {
		report.appendFloat(static_cast <float> (standardDeviation));
	}
}

void Statistic::setSelfSamplingInterval(uint16_t samplingInterval)  {
	this -> selfSamplingInterval = samplingInterval;
}

void Statistic::resetStatistics() {
	max = 0;
	min = 0;
	maxTime = 0;
	minTime = 0;
	mean = 0;
	meanOfSquares = 0;
	sumOfSquares = 0;
	standardDeviation = 0;
	sampleCounter = 0;
}
