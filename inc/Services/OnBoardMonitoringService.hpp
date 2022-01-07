#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <Message.hpp>
#include "etl/array.h"
#include "Service.hpp"
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

	enum CheckType : uint8_t { LimitCheck = 1, ExpectedValueCheck = 2, DeltaCheck = 3 };

	struct LimitCheck {
		uint16_t lowLimit;
		Event belowLowLimitEvent;
		uint16_t highLimit;
		Event aboveHighLimitEvent;
	};

	struct ExpectedValueCheck {
		uint16_t expectedValue;
		// TODO: Find what variable type is a bit string.
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
	/**
	 * Map storing the parameters for each parameter monitoring definition.
	 */
	etl::map<uint16_t, std::reference_wrapper<ParameterBase>, ECSSMaxParameters> ParameterMonitoringList;
	/**
	 * Map storing the Monitored Parameter Ids that correspond to the Parameter Monitoring Ids.
	 */
	etl::map<uint16_t, uint16_t, ECSSMaxParameters> MonitoredParameterIds;
	/**
	 * Map storing the checking status for each parameter monitoring definition.
	 */
	etl::map<uint16_t, CheckingStatus, ECSSMaxParameters> ParameterMonitoringCheckingStatus;
	/**
	 * Map storing the number of consecutive checks that have been conducted for each parameter monitoring definition.
	 */
	etl::map<uint16_t, uint16_t, ECSSMaxParameters> RepetitionCounter;
	/**
	 * Map storing the number of consecutive checks that need to be conducted for each parameter in order to set a new
	 * checking status.
	 */
	etl::map<uint16_t, uint16_t, ECSSMaxParameters> RepetitionNumber;
	/**
	 * Map storing the status of each parameter monitoring definition.
	 */
	etl::map<uint16_t, bool, ECSSMaxParameters> ParameterMonitoringStatus;
	/**
	 * Map storing the transitions of each parameter monitoring definition's status.
	 */
	etl::map<uint16_t, etl::array<CheckingStatus, 2>, ECSSMaxParameters> CheckTransitionList;
	/**
	 * Map storing the check type of each parameter monitoring definition.
	 */
	etl::map<uint16_t, CheckType, ECSSMaxParameters> ParameterMonitoringCheckTypes;
	/**
	 * Map storing the type-specific parameters for limit checks.
	 */
	etl::map<uint16_t, struct LimitCheck, ECSSMaxParameters> LimitCheckParameters;
	/**
	 * Map storing the type-specific parameters for expected value checks.
	 */
	etl::map<uint16_t, struct ExpectedValueCheck, ECSSMaxParameters> ExpectedValueCheckParameters;
	/**
	 * Map storing the type-specific parameters for delta checks.
	 */
	etl::map<uint16_t, struct DeltaCheck, ECSSMaxParameters> DeltaCheckParameters;

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

	void execute(Message& message);
};

#endif // ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
