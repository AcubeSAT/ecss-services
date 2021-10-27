#include "ECSS_Definitions.hpp"
#include "etl/vector.h"
#include "Message.hpp"
#include <cmath>
#include <iostream>

/**
 * Mother class, exists only to offer its functions to its child class, so that we utilize polymorphism to our
 * advantage. The need for polymorphism exists because different sensors return values of different datatype so we
 * need our functions and our storage structs (vectors, arrays) to work with multiple datatypes.
 */
class HousekeepingParameterBase {
public:
	uint16_t parameterId = 0;
	uint16_t selfSamplingInterval = 0;
	uint16_t sampleCounter = 0;

	virtual void storeSamples(uint8_t value) = 0;
	virtual void storeSamples(uint16_t value) = 0;
	virtual void storeSamples(uint32_t value) = 0;
	virtual void storeSamples(float value) = 0;
	virtual void appendSampleToMessage(Message& report, uint16_t index) = 0;

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
class HousekeepingParameter : public HousekeepingParameterBase {
public:

	etl::vector <DataType, ECSS_MAX_HOUSEKEEPING_PARAMETER_SAMPLES> housekeepingParameterSamples;

	explicit HousekeepingParameter(int base) {}
	HousekeepingParameter() = default;

	/**
	 * Gets the value from the sensor as argument and stores it into the samplesVector
	 */
	inline void storeSamples(uint8_t value) override {  // TODO: arg is always double, add this func to housekeeping
		                                               // sample storing
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
		//TODO: Here implement kanavouras calculation formula
		housekeepingParameterSamples.push_back(value);
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
		housekeepingParameterSamples.push_back(value);
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
		housekeepingParameterSamples.push_back(value);
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
		housekeepingParameterSamples.push_back(value);
		sampleCounter++;
	}

	inline void appendSampleToMessage(Message& report, uint16_t index) override {

		DataType sampleValue = housekeepingParameterSamples[index];
//		std::cout<<sampleValue<<std::endl;
		report.append(sampleValue);
	}

};
