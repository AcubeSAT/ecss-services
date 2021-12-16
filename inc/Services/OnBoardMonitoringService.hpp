#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <Message.hpp>
#include "etl/array.h"
#include "Service.hpp"
#include "Parameters/SystemParameters.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/map.h"
#include "ECSS_Definitions.hpp"
#include "etl/list.h"

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

	enum CheckingStatus : uint8_t {
		Unchecked = 1,
		Invalid = 2,
		ExpectedValue = 3,
		UnexpectedValue = 4,
		WithinLimits = 5,
		BelowLowLimit = 6,
		AboveHighLimit = 7,
		WithinThreshold = 8,
		BelowLowThreshold = 9,
		AboveHighThreshold = 10
	};
	// TODO: Find out if events should be declared here or in EventReportService.hpp
	enum Event : uint8_t {
		BelowLowLimitEvent = 1,
		AboveHighLimitEvent = 2,
		NotExpectedValueEvent = 3,
		BelowLowThresholdEvent = 4,
		AboveHighThresholdEvent = 5
	};

	enum CheckType : uint8_t {
		LimitCheck = 1,
		ExpectedValueCheck = 2,
		DeltaCheck = 3
	};

	struct LimitCheck {
		uint16_t lowLimit;
		Event belowLowLimitEvent;
		uint16_t highLimit;
		Event aboveHighLimitEvent;
	};

	struct ExpectedValueCheck {
		uint16_t expectedValue;
		uint8_t mask;
		Event notExpectedValueEvent;
	};

	struct DeltaCheck {
		uint16_t numberOfConsecutiveDeltaChecks;
		uint16_t lowDeltaThreshold;
		Event belowLowThresholdEvent;
		uint16_t highDeltaThreshold;
		Event aboveHighThresholdEvent;
	};


	uint16_t maximumTransitionReportingDelay = 0;

	etl::map<uint16_t, std::reference_wrapper<ParameterBase>, ECSSMaxParameters> ParameterMonitoringList;

	etl::map<std::reference_wrapper<ParameterBase>, CheckingStatus, ECSSMaxParameters>
	    ParameterMonitoringCheckingStatus;

	etl::map<std::reference_wrapper<ParameterBase>, uint16_t, ECSSMaxParameters> RepetitionCounter;

	etl::map<std::reference_wrapper<ParameterBase>, bool, ECSSMaxParameters> ParameterMonitoringStatus;

	etl::map<std::reference_wrapper<ParameterBase>, etl::array<CheckingStatus, 2>, ECSSMaxParameters>
	    CheckTransitionList;

	etl::map<std::reference_wrapper<ParameterBase>, CheckType, ECSSMaxParameters> ParameterMonitoringCheckTypes;

	etl::map<std::reference_wrapper<ParameterBase>, struct LimitCheck, ECSSMaxParameters> LimitCheckParameters;

	etl::map<std::reference_wrapper<ParameterBase>, struct ExpectedValueCheck, ECSSMaxParameters> ExpectedValueCheckParameters;

	etl::map<std::reference_wrapper<ParameterBase>, struct DeltaCheck, ECSSMaxParameters> DeltaCheckParameters;

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
