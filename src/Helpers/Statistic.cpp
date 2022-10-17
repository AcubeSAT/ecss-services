#include "Helpers/Statistic.hpp"
#include <cmath>

void Statistic::updateStatistics(double value) {
	if (value > max) {
		max = value;
		timeOfMaxValue = TimeGetter::getCurrentTimeDefaultCUC();
	}
	if (value < min) {
		min = value;
		timeOfMinValue = TimeGetter::getCurrentTimeDefaultCUC();
	}
	if (sampleCounter + 1 > 0) {
		mean = (mean * sampleCounter + value) / (sampleCounter + 1);
	}
	sumOfSquares += pow(value, 2);
	sampleCounter++;
}

void Statistic::appendStatisticsToMessage(Message& report) const {
	report.appendFloat(static_cast<float>(max));
	report.append(timeOfMaxValue);
	report.appendFloat(static_cast<float>(min));
	report.append(timeOfMinValue);
	report.appendFloat(static_cast<float>(mean));

	if constexpr (SupportsStandardDeviation) {
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
	timeOfMaxValue = Time::DefaultCUC(0);
	timeOfMinValue = Time::DefaultCUC(0);
	mean = 0;
	sumOfSquares = 0;
	sampleCounter = 0;
}

bool Statistic::statisticsAreInitialized() const {
	return (sampleCounter == 0 and mean == 0 and sumOfSquares == 0 and
	        timeOfMaxValue == Time::DefaultCUC(0) and timeOfMinValue == Time::DefaultCUC(0) and
	        max == -std::numeric_limits<double>::infinity() and min == std::numeric_limits<double>::infinity());
}
