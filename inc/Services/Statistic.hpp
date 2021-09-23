#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "cmath"

#define SAMPLES_MAX_VECTOR_SIZE 10

class StatisticBase {
public:
	uint16_t parameterId = 0;
	uint16_t numOfSamplesCounter = 0;
	uint16_t type = 0;
	virtual void storeStatistic() = 0;  //Maybe take message type argument from another task, containing the statistic.
	virtual void calculateStatistics() = 0;
};

template <typename DataType>
class Statistic : public StatisticBase {
public:

	explicit Statistic(const StatisticBase& base) {}
	DataType max = 0;
	DataType min = 0;
	uint16_t maxTime = 0;   //what time??
	uint16_t minTime = 0;
	float mean = 0;
	float standardDeviation = 0;

//	explicit Statistic(uint16_t paramId, uint16_t samplesCounter, uint16_t typeId) : parameterId(paramId) ,
//	      numOfSamplesCounter(samplesCounter) , type(typeId) {}

	etl::vector <DataType, SAMPLES_MAX_VECTOR_SIZE> samplesVector;

	inline void storeStatistic() override {
		DataType newSample = 1; //Dummy value, in reality it has to take a real sample at this point.
		Statistic::samplesVector.push_back(newSample);
		numOfSamplesCounter++;
	}

	inline void calculateStatistics() override {

		max = samplesVector.at(0);
		min = max;
		uint16_t sum = samplesVector.at(0);

		for (int i = 1; i < samplesVector.size(); i++) {

			DataType currentValue = samplesVector.at(i);
			if (currentValue > max) {
				max = currentValue;
			}
			if (currentValue < min) {
				min = currentValue;
			}
			sum += currentValue;
		}

		mean = sum / samplesVector.size();
		for (int i = 0; i < samplesVector.size(); i++) {
			standardDeviation += pow(samplesVector.at(i) - mean, 2);
		}
		standardDeviation = sqrt(standardDeviation / samplesVector.size());
	}

};

#endif