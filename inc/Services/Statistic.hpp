#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "cmath"

extern bool supportsStandardDeviation;
#define SAMPLES_MAX_VECTOR_SIZE 10

class StatisticBase {
public:
	uint16_t parameterId = 0;
	uint16_t selfSamplingInterval = 0;
	uint16_t numOfSamplesCounter = 0;

	virtual void storeSamples(uint8_t value) = 0;
	virtual void storeSamples(uint16_t value) = 0;
	virtual void storeSamples(uint32_t value) = 0;
	virtual void storeSamples(float value) = 0;
	virtual void calculateStatistics() = 0;
	virtual void clearStatisticSamples() = 0;
	virtual int appendStatisticsToMessage(StatisticBase& stat, Message& report) = 0;
	virtual void getSample(Message& report, int index) = 0;

	void setSelfTimeInterval(uint16_t timeInterval) {
		this -> selfSamplingInterval = timeInterval;
	}
};

template <typename DataType>
class Statistic : public StatisticBase {
public:

	explicit Statistic(const StatisticBase& base) {}
	DataType max = 0;
	DataType min = 0;
	uint16_t maxTime = 0;
	uint16_t minTime = 0;
	float mean = 0;
	float standardDeviation = 0;

	Statistic() = default;

	etl::vector <DataType, SAMPLES_MAX_VECTOR_SIZE> samplesVector;
//	etl::vector <uint32_t, SAMPLES_MAX_VECTOR_SIZE> sampleTimestampsVector; <-this will be used when CUC is ready!!

	inline void storeSamples(uint8_t value) override {
		/*
		 * TODO:
		 *      uint32_t currentSampleTime = as_CUC_timestamp();
		 *      sampleTimestampsVector.push_back(currentSampleTime);
		 *
		 *      if periodic, just calculate next time without the CUC
		 *      function. Very easy, but we cannot include the
		 *      ParameterStatisticsService.hpp because that includes
		 *      the Statistic.hpp file, not the other way around.
		 *      There are plenty of ways to do that though!!
		 * */
		Statistic::samplesVector.push_back(value);
		numOfSamplesCounter++;

	}

	inline void storeSamples(uint16_t value) override {
		/*
		 * TODO:
		 *      uint32_t currentSampleTime = as_CUC_timestamp();
		 *      sampleTimestampsVector.push_back(currentSampleTime);
		 *
		 *      if periodic, just calculate next time without the CUC
		 *      function. Very easy, but we cannot include the
		 *      ParameterStatisticsService.hpp because that includes
		 *      the Statistic.hpp file, not the other way around.
		 *      There are plenty of ways to do that though!!
		 * */
		Statistic::samplesVector.push_back(value);
		numOfSamplesCounter++;
	}

	inline void storeSamples(uint32_t value) override {
		/*
		 * TODO:
		 *      uint32_t currentSampleTime = as_CUC_timestamp();
		 *      sampleTimestampsVector.push_back(currentSampleTime);
		 *
		 *      if periodic, just calculate next time without the CUC
		 *      function. Very easy, but we cannot include the
		 *      ParameterStatisticsService.hpp because that includes
		 *      the Statistic.hpp file, not the other way around.
		 *      There are plenty of ways to do that though!!
		 * */
		Statistic::samplesVector.push_back(value);
		numOfSamplesCounter++;
	}

	inline void storeSamples(float value) override {
		/*
		 * TODO:
		 *      uint32_t currentSampleTime = as_CUC_timestamp();
		 *      sampleTimestampsVector.push_back(currentSampleTime);
		 *
		 *      if periodic, just calculate next time without the CUC
		 *      function. Very easy, but we cannot include the
		 *      ParameterStatisticsService.hpp because that includes
		 *      the Statistic.hpp file, not the other way around.
		 *      There are plenty of ways to do that though!!
		 * */
		Statistic::samplesVector.push_back(value);
		numOfSamplesCounter++;
	}

	inline void calculateStatistics() override {

		max = samplesVector.at(0);
		// maxTime = sampleTimestampsVector.at(0);
		min = max;
		// minTime = maxTime;
		uint16_t sum = samplesVector.at(0);

		for (int i = 1; i < samplesVector.size(); i++) {

			DataType currentValue = samplesVector.at(i);
			if (currentValue > max) {
				max = currentValue;
				// maxTime = sampleTimestampsVector.at(i);
			}
			if (currentValue < min) {
				min = currentValue;
				// minTime = sampleTimestampsVector.at(i);
			}
			sum += currentValue;
		}

		mean = sum / samplesVector.size();
		if (supportsStandardDeviation) {
			for (int i = 0; i < samplesVector.size(); i++) {
				standardDeviation += pow(samplesVector.at(i) - mean, 2);
			}
			standardDeviation = sqrt(standardDeviation / samplesVector.size());
		}
	}

	inline void clearStatisticSamples() override {

		samplesVector.clear();
		max = 0;
		min = 0;
		mean = 0;
		standardDeviation = 0;
		numOfSamplesCounter = 0;
		maxTime = 0;
		minTime = 0;
	}

	inline int appendStatisticsToMessage(StatisticBase& stat, Message& report) override {

		Statistic <DataType> newStat = static_cast <Statistic <DataType>&> (stat);
		// Calculate all the statistics
		newStat.calculateStatistics();
		DataType &maxValue = newStat.max;
		DataType &minValue = newStat.min;
		float &meanValue = newStat.mean;

		// Append everything to the report message in the correct order
		report.append <DataType> (maxValue);
		// append maxTime here (returned from storeSamples)
		report.append <DataType> (minValue);
		// append minTime here (returned from storeSamples)
		report.appendFloat(meanValue);

		if (supportsStandardDeviation) {
			float &sdValue = newStat.standardDeviation;
			report.appendFloat(sdValue);
			return 0;
		}
		return maxValue;
	}

	inline void getSample(Message& report, int index) override {

		DataType sampleValue = samplesVector[index];
		report.append(sampleValue);
	}

};


#endif