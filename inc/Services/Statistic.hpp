#ifndef ECSS_SERVICES_STATISTIC_HPP
#define ECSS_SERVICES_STATISTIC_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"

#define SAMPLES_MAX_VECTOR_SIZE 10

class StatisticBase {
public:
	virtual void storeStatistic() = 0;  //Maybe take message type argument from another task, containing the statistic.
};

template <typename DataType>
class Statistic : public StatisticBase {
public:

	uint16_t parameterId = 0;
	uint16_t numOfSamplesCounter = 0;
	DataType maxValue = 0;
	DataType minValue = 0;
	DataType mean = 0;
	DataType standardDeviation = 0;

	etl::vector <DataType, SAMPLES_MAX_VECTOR_SIZE> samplesVector;

	explicit Statistic(uint16_t paramId) : parameterId(paramId) {}

	inline void storeStatistic() override {
		DataType newSample = 1; //Dummy value, in reality it has to take a real sample at this point.
		Statistic::samplesVector.push_back(newSample);
	}

};

#endif