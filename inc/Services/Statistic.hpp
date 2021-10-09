#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include <cmath>

extern bool supportsStandardDeviation;

/**
 * Mother class, exists only to offer its functions to its Kid's class, so that we utilize polymorphism to our
 * advantage. The need for polymorphism exists because different sensors return values of different datatype so we
 * need our functions and our storage structs (vectors, arrays) to work with multiple datatypes.
 */
class StatisticBase {
public:
	uint16_t parameterId = 0;
	uint16_t selfSamplingInterval = 0;
	uint16_t sampleCounter = 0;

	virtual void storeSamples(uint8_t value) = 0;
	virtual void storeSamples(uint16_t value) = 0;
	virtual void storeSamples(uint32_t value) = 0;
	virtual void storeSamples(float value) = 0;
	virtual void calculateStatistics() = 0;
	virtual void clearStatisticSamples() = 0;
	virtual void appendStatisticsToMessage(StatisticBase& stat, Message& report) = 0;

	void setSelfSamplingInterval(uint16_t samplingInterval) {
		this -> selfSamplingInterval = samplingInterval;
	}
};

/**
 * This is the Kid class that overrides the functions of the mother class and makes them work for every datatype. It
 * also stores samples into a vector which is going to be of type DataType depending on the parameter whose
 * statistics we are trying to calculate and store.
 */
template <typename DataType>
class Statistic : public StatisticBase {
public:

	explicit Statistic(const StatisticBase& base) {}
	DataType max = 0;
	DataType min = 0;
	uint32_t maxTime = 0;
	uint32_t minTime = 0;
	double mean = 0;
	float standardDeviation = 0;

	Statistic() = default;

	etl::vector <DataType, SAMPLES_MAX_VECTOR_SIZE> samplesVector;
//	etl::vector <uint32_t, SAMPLES_MAX_VECTOR_SIZE> sampleTimestampsVector; <-this will be used when CUC is ready!!

	/**
	 * Gets the value from the sensor as argument and stores it into the samplesVector
	 */
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
		sampleCounter++;

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
		sampleCounter++;
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
		sampleCounter++;
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
		sampleCounter++;

	}

	/**
	 * Calculates all statistics for the current parameter, including max, min, mean, standard deviation, maxTime and
	 * minTime as specified by ECSS.
	 */
	inline void calculateStatistics() override {

		max = samplesVector.at(0);
		// maxTime = sampleTimestampsVector.at(0);
		min = max;
		// minTime = maxTime;
		uint16_t sum = samplesVector[0];

		for (int i = 1; i < samplesVector.size(); i++) {

			DataType currentValue = samplesVector[i];
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
				standardDeviation += pow(samplesVector[i] - mean, 2);
			}
			standardDeviation = sqrt(standardDeviation / samplesVector.size());
		}
	}

	/**
	 * Clears the statistics of the current parameter, ie empties the samplesVector.
	 */
	inline void clearStatisticSamples() override {

		samplesVector.clear();
		max = 0;
		min = 0;
		mean = 0;
		standardDeviation = 0;
		sampleCounter = 0;
		maxTime = 0;
		minTime = 0;
	}

	/**
	 * Gets a StatisticBase object, casts it into type Statistic, calculates its statistics and appends them to the
	 * message.
	 */
	inline void appendStatisticsToMessage(StatisticBase& stat, Message& report) override {

		Statistic <DataType> newStat = static_cast <Statistic <DataType>&> (stat);
		// Calculate all the statistics
		newStat.calculateStatistics();
		DataType &maxValue = newStat.max;
		DataType &minValue = newStat.min;
		double &meanValue = newStat.mean;

		// TODO: Append everything to the report message in the correct order
		report.append <DataType> (maxValue);
		// TODO: append maxTime here (returned from storeSamples)
		report.append <DataType> (minValue);
		// TODO: append minTime here (returned from storeSamples)
		report.appendFloat(meanValue);

		if (supportsStandardDeviation) {
			float sdValue = newStat.standardDeviation;
			report.appendFloat(sdValue);
		}
	}

};

#endif