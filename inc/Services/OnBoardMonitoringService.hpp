#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <Message.hpp>
#include "Service.hpp"
#include "Parameters/SystemParameters.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/map.h"
#include "ECSS_Definitions.hpp"



class OnBoardMonitoringService : public Service {

public:
	inline static const uint8_t ServiceType = 12;
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

	enum ParameterMonitoringCheckingStatus : uint8_t {
		ExpectedValueCheckUnchecked = 1,
		ExpectedValueCheckInvalid = 2,
		ExpectedValueCheckExpectedValue = 3,
		ExpectedValueCheckUnexpectedValue = 4,
		LimitCheckUnchecked = 5,
		LimitCheckInvalid = 6,
		LimitCheckWithinLimits = 7,
		LimitCheckBelowLowLimit = 8,
		LimitCheckAboveHighLimit = 9,
		DeltaCheckUnchecked = 10,
		DeltaCheckInvalid = 11,
		DeltaCheckWithinThreshold = 12,
		DeltaCheckBelowLowThreshold = 13,
		DeltaCheckAboveHighThreshold = 14
	};

	//TODO: Evaluate the parameter data type
	etl::map<uint16_t, Parameter<uint32_t>, ECSSMaxParameters> ParameterMonitoringList;

	etl::map<Parameter<uint32_t>, ParameterMonitoringCheckingStatus, ECSSMaxParameters> CheckingStatus;

	etl::map<Parameter<uint32_t>, uint16_t, ECSSMaxParameters> RepetitionCounter;

	etl::map<Parameter<uint32_t>, bool, ECSSMaxParameters> ParameterMonitoringStatus;


	/**
	 * If true, parameter monitoring is enabled
	 */
	bool parameterMonitoringFunctionStatus = false;

	/**
	 * TC[12,1]
	 */
	void enableParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,2]
	 */
	void disableParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,3]
	 */
	void changeMaximumTransitionReportingDelay(Message& message);

	/**
	 * TC[12,4]
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
