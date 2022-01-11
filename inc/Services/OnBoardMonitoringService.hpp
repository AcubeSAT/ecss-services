#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <cstdint>
#include "Message.hpp"
#include "etl/array.h"
#include "Service.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/map.h"
#include "ECSS_Definitions.hpp"
#include "etl/list.h"

class PMONBase {
public:
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

	enum CheckType : uint8_t { LimitCheck = 1, ExpectedValueCheck = 2, DeltaCheck = 3 };

	std::reference_wrapper<ParameterBase> monitoredParameter;
	CheckType checkType;
	uint16_t monitoredParameterId;
	uint16_t repetitionNumber;
	uint16_t repetitionCounter;
	bool monitoringStatus;
	CheckingStatus checkingStatus;
	etl::array<CheckingStatus, 2> checkTransitionList;
	PMONBase(std::reference_wrapper<ParameterBase> monitoredParameter, CheckType checkType,
	         uint16_t monitoredParameterId, uint16_t repetitionNumber, uint16_t repetitionCounter,
	         bool monitoringStatus, CheckingStatus checkingStatus, etl::array<CheckingStatus, 2> checkTransitionList)
	    : monitoredParameter(monitoredParameter) {
		this->checkType = checkType;
		this->monitoredParameterId = monitoredParameterId;
		this->repetitionNumber = repetitionNumber;
		this->repetitionCounter = repetitionCounter;
		this->monitoringStatus = monitoringStatus;
		this->checkingStatus = checkingStatus;
		this->checkTransitionList = checkTransitionList;
	}
};

class PMONExpectedValueCheck : public PMONBase {
public:
	double expectedValue;
	uint16_t mask;
	uint16_t notExpectedValueEvent;
	explicit PMONExpectedValueCheck(std::reference_wrapper<ParameterBase> monitoredParameter, CheckType checkType,
	                                uint16_t monitoredParameterId, uint16_t repetitionNumber,
	                                uint16_t repetitionCounter, bool monitoringStatus, CheckingStatus checkingStatus,
	                                etl::array<CheckingStatus, 2> checkTransitionList, double expectedValue,
	                                uint16_t mask, uint16_t notExpectedValueEvent)
	    : PMONBase(monitoredParameter, checkType, monitoredParameterId, repetitionNumber, repetitionCounter,
	               monitoringStatus, checkingStatus, checkTransitionList) {
		this->expectedValue = expectedValue;
		this->mask = mask;
		this->notExpectedValueEvent = notExpectedValueEvent;
	}
};

class PMONLimitCheck : public PMONBase {
public:
	double lowLimit;
	uint16_t belowLowLimitEvent;
	double highLimit;
	uint16_t aboveHighLimitEvent;

	PMONLimitCheck(std::reference_wrapper<ParameterBase> monitoredParameter, PMONBase::CheckType checkType,
	               uint16_t monitoredParameterId, uint16_t repetitionNumber, uint16_t repetitionCounter,
	               bool monitoringStatus, PMONBase::CheckingStatus checkingStatus,
	               etl::array<PMONBase::CheckingStatus, 2> checkTransitionList, double lowLimit,
	               uint16_t belowLowLimitEvent, double highLimit, uint16_t aboveHighLimitEvent)
	    : PMONBase(monitoredParameter, checkType, monitoredParameterId, repetitionNumber, repetitionCounter,
	               monitoringStatus, checkingStatus, checkTransitionList) {
		this->lowLimit = lowLimit;
		this->belowLowLimitEvent = belowLowLimitEvent;
		this->highLimit = highLimit;
		this->aboveHighLimitEvent = aboveHighLimitEvent;
	}
};

class PMONDeltaCheck : public PMONBase {
public:
	uint16_t numberOfConsecutiveDeltaChecks;
	double lowDeltaThreshold;
	uint16_t belowLowThresholdEvent;
	double highDeltaThreshold;
	uint16_t aboveHighThresholdEvent;

	PMONDeltaCheck(std::reference_wrapper<ParameterBase> monitoredParameter, PMONBase::CheckType checkType,
	               uint16_t monitoredParameterId, uint16_t repetitionNumber, uint16_t repetitionCounter,
	               bool monitoringStatus, PMONBase::CheckingStatus checkingStatus,
	               etl::array<PMONBase::CheckingStatus, 2> checkTransitionList, uint16_t numberOfConsecutiveDeltaChecks,
	               double lowDeltaThreshold, uint16_t belowLowThresholdEvent, double highDeltaThreshold,
	               uint16_t aboveHighThresholdEvent)
	    : PMONBase(monitoredParameter, checkType, monitoredParameterId, repetitionNumber, repetitionCounter,
	               monitoringStatus, checkingStatus, checkTransitionList) {
		this->numberOfConsecutiveDeltaChecks = numberOfConsecutiveDeltaChecks;
		this->lowDeltaThreshold = lowDeltaThreshold;
		this->belowLowThresholdEvent = belowLowThresholdEvent;
		this->highDeltaThreshold = highDeltaThreshold;
		this->aboveHighThresholdEvent = aboveHighThresholdEvent;
	}
};

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

	uint16_t maximumTransitionReportingDelay = 0;
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, std::reference_wrapper<PMONBase>, ECSSMaxMonitoringDefinitions> ParameterMonitoringList;
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
