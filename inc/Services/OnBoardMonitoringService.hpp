#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/PMONBase.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "etl/array.h"
#include "etl/list.h"
#include "etl/map.h"

/**
 * Implementation of the ST[12] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 * @ingroup Services
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com> and Thomas Pravinos <tompravi99@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, std::reference_wrapper<PMONBase>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

	/**
	 * Maximum number of checks for each check type is calculated individually
	 * based on figure 8-114 on page 509 of the ECSS Standard and the existing code.
	 * For the calculation each 16-bit enumeration is 2 bytes in size, a 16-bit unsigned integer is 2 bytes as well
	 * and a double precision floating-point number is 8 bytes in size.
	 * The maximum data size for an ECSS Message is currently 1024 bytes.
	 * For Limit Check Messages there are 5 16-bit enumerations (PMON ID, currentMonitoredParameterId, check type, belowLowLimitEventId, aboveHighLimitEventId),
	 * 2 16-bit unsigned integers (N, currentPMONRepetitionNumber)
	 * and 2 double precision floating-point numbers (lowLimit, highLimit).
	 * The total is 5*2+2*2+2*8=30 bytes per Limit Check.
	 * For the maximum number : 1024/30=34.13, so 34 is the maximum number of checks.
	 */
	static constexpr uint8_t MaximumNumberOfChecksLimitCheck = 34;

	/**
	 * For Expected Value Check Messages there are 4 16-bit enumerations (PMON ID, currentMonitoredParameterId, check type, unExpectedValueEvent),
	 * 2 16-bit unsigned integers (N, currentPMONRepetitionNumber)
	 * and 2 double precision floating-point numbers (expectedValue, mask).
	 * The total is 4*2+2*2+2*8=28 bytes per Expected Value Check.
	 * For the maximum number : 1024/28=36.57, so 36 is the maximum number of checks.
	 */
	static constexpr uint8_t MaximumNumberOfChecksExpectedValueCheck = 36;

	/**
	 * For Delta Check Messages there are 5 16-bit enumerations (PMON ID, currentMonitoredParameterId, check type, belowLowThresholdEventId, aboveHighThresholdEventId),
	 * 3 16-bit unsigned integers (N, currentPMONRepetitionNumber, numberOfConsecutiveDeltaChecks)
	 * and 2 double precision floating-point numbers (lowDeltaThreshold, highDeltaThreshold).
	 * The total is 5*2+3*2+2*8=32 bytes per Expected Value Check.
	 * For the maximum number : 1024/32=32, so 32 is the maximum number of checks.
	 */
	static constexpr uint8_t MaximumNumberOfChecksDeltaCheck = 32;

public:
	inline static const ServiceTypeNum ServiceType = 12;
	enum MessageType : uint8_t {
		EnableParameterMonitoringDefinitions = 1,
		DisableParameterMonitoringDefinitions = 2,
		ChangeMaximumTransitionReportingDelay = 3,
		DeleteAllParameterMonitoringDefinitions = 4,
		AddParameterMonitoringDefinitions = 5,
		DeleteParameterMonitoringDefinitions = 6,
		ModifyParameterMonitoringDefinitions = 7,
		ReportParameterMonitoringDefinitions = 8,
		ParameterMonitoringDefinitionReport = 9,
		ReportOutOfLimits = 10,
		OutOfLimitsReport = 11,
		CheckTransitionReport = 12,
		ReportStatusOfParameterMonitoringDefinition = 13,
		ParameterMonitoringDefinitionStatusReport = 14
	};

	OnBoardMonitoringService() {
		serviceType = ServiceType;
	}

	/**
	 * The maximum time between two transition reports.
	 * Measured in "on-board parameter minimum sampling interval" units (see 5.4.3.2c in ECSS-E-ST-70-41C).
	 */
	uint16_t maximumTransitionReportingDelay = 0;
	/**
	 * If true, parameter monitoring is enabled
	 */
	bool parameterMonitoringFunctionStatus = false;
	/*
	 * Adds a new Parameter Monitoring definition to the parameter monitoring list.
	 */
	void addPMONDefinition(PMONId PMONId, std::reference_wrapper<PMONBase> PMONDefinition) {
		parameterMonitoringList.insert({PMONId, PMONDefinition});
	}
	/**
	 * @param PMONId
	 * @return Parameter Monitoring definition
	 */
	std::reference_wrapper<PMONBase> getPMONDefinition(PMONId PMONId) {
		return parameterMonitoringList.at(PMONId);
	}
	/**
	 * @return true if PMONList is empty.
	 */
	bool isPMONListEmpty() {
		return parameterMonitoringList.empty();
	}
	uint16_t getCount(uint16_t key) const {
		return parameterMonitoringList.count(key);
	}
	/**
	 * Enables the PMON definitions which correspond to the ids in TC[12,1].
	 */
	void enableParameterMonitoringDefinitions(Message& message);

	/**
	 * Disables the PMON definitions which correspond to the ids in TC[12,2].
	 */
	void disableParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,3]
	 * Changes the maximum time between two transition reports.
	 */
	void changeMaximumTransitionReportingDelay(Message& message);

	/**
	 * TC[12,4]
	 * Deletes all the PMON definitions in the PMON list.
	 */
	void deleteAllParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,5]
	 */
	void addParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,6]
	 */
	void deleteParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,7]
	 */
	void modifyParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,8]
	 */
	void reportParameterMonitoringDefinitions(Message& message);

	/**
	 * TM[12,9]
	 */
	void parameterMonitoringDefinitionReport(Message& message);

	/**
	 * TC[12,10]
	 */
	void reportOutOfLimits(Message& message);

	/**
	 * TM[12,11]
	 */
	void outOfLimitsReport();

	/**
	 * TM[12,12]
	 */
	void checkTransitionReport();

	/**
	 * TC[12,13]
	 */
	void reportStatusOfParameterMonitoringDefinition(Message& message);

	/**
	 * TM[12,14]
	 */
	void parameterMonitoringDefinitionStatusReport();

	void execute(Message& message);
};

#endif // ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP