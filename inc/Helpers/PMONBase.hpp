#ifndef ECSS_SERVICES_PMONBASE_HPP
#define ECSS_SERVICES_PMONBASE_HPP
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "etl/array.h"
#include "etl/list.h"
#include "etl/map.h"

/**
 * Base class for Parameter Monitoring definitions. Contains the common variables of all check types.
 */
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

	enum class CheckType : uint8_t { Limit = 1,
		                             ExpectedValue = 2,
		                             Delta = 3 };


	ParameterId monitoredParameterId;

	std::reference_wrapper<ParameterBase> monitoredParameter;
	/**
	 * The number of checks that need to be conducted in order to set a new Parameter Monitoring Status.
	 */
	RepetitionNumber repetitionNumber;
	/**
	 * The number of checks that have been conducted so far.
	 */
	uint16_t repetitionCounter = 0;
	bool monitoringEnabled = false;
	CheckingStatus checkingStatus = Unchecked;
	etl::array<CheckingStatus, 2> checkTransitionList = {};
	CheckType checkType;

	RepetitionNumber getRepetitionNumber() const {
		return repetitionNumber;
	}
	bool isMonitoringEnabled() const {
		return monitoringEnabled;
	}
	CheckType getCheckType() const {
		return checkType;
	}
	CheckingStatus getCheckingStatus() const {
		return checkingStatus;
	}

	virtual Expected_Value getExpectedValue() {
		return 0.0; }
	virtual Mask getMask() {
		return 0; }
	virtual EventDefinitionId getUnexpectedValueEvent() {
		return 0; }
	virtual Limit getLowLimit() {
		return 0.0; }
	virtual EventDefinitionId getBelowLowLimitEvent() {
		return 0; }
	virtual Limit getHighLimit() {
		return 0.0; }
	virtual EventDefinitionId getAboveHighLimitEvent() {
		return 0; }
	virtual NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() {
		return 0; }
	virtual DeltaThreshold getLowDeltaThreshold() {
		return 0.0; }
	virtual EventDefinitionId getBelowLowThresholdEvent() {
		return 0; }
	virtual DeltaThreshold getHighDeltaThreshold() {
		return 0.0; }
	virtual EventDefinitionId getAboveHighThresholdEvent() {
		return 0; }

protected:
	/**
	 * @param monitoredParameterId is assumed to be correct and not checked.
	 */
	PMONBase(ParameterId monitoredParameterId, RepetitionNumber repetitionNumber);
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of expected value check type.
 */
class PMONExpectedValueCheck : public PMONBase {
public:
	Expected_Value expectedValue;
	Mask mask;
	EventDefinitionId unexpectedValueEvent;

	explicit PMONExpectedValueCheck(ParameterId monitoredParameterId, RepetitionNumber repetitionNumber, Expected_Value expectedValue,
	                                Mask mask, EventDefinitionId unexpectedValueEvent)
	    : expectedValue(expectedValue), mask(mask), unexpectedValueEvent(unexpectedValueEvent),
	      PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::ExpectedValue;
	};

	Expected_Value getExpectedValue() override {
		return expectedValue;
	}

	Mask getMask() override {
		return mask;
	}

	EventDefinitionId getUnexpectedValueEvent() override {
		return unexpectedValueEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of limit check type.
 */
class PMONLimitCheck : public PMONBase {
public:
	Limit lowLimit;
	EventDefinitionId belowLowLimitEvent;
	Limit highLimit;
	EventDefinitionId aboveHighLimitEvent;

	explicit PMONLimitCheck(ParameterId monitoredParameterId, RepetitionNumber repetitionNumber, Limit lowLimit,
	                        EventDefinitionId belowLowLimitEvent, Limit highLimit, EventDefinitionId aboveHighLimitEvent)
	    : lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Limit;
	};

	Limit getLowLimit() override {
		return lowLimit;
	}

	EventDefinitionId getBelowLowLimitEvent() override {
		return belowLowLimitEvent;
	}

	Limit getHighLimit() override {
		return highLimit;
	}

	EventDefinitionId getAboveHighLimitEvent() override {
		return aboveHighLimitEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of delta check type.
 */
class PMONDeltaCheck : public PMONBase {
public:
	NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks;
	DeltaThreshold lowDeltaThreshold;
	EventDefinitionId belowLowThresholdEvent;
	DeltaThreshold highDeltaThreshold;
	EventDefinitionId aboveHighThresholdEvent;

	explicit PMONDeltaCheck(ParameterId monitoredParameterId, RepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, DeltaThreshold lowDeltaThreshold,
	                        EventDefinitionId belowLowThresholdEvent, DeltaThreshold highDeltaThreshold,
	                        EventDefinitionId aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Delta;
	};

	NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() override {
		return numberOfConsecutiveDeltaChecks;
	}

	DeltaThreshold getLowDeltaThreshold() override {
		return lowDeltaThreshold;
	}

	EventDefinitionId getBelowLowThresholdEvent() override {
		return belowLowThresholdEvent;
	}

	DeltaThreshold getHighDeltaThreshold() override {
		return highDeltaThreshold;
	}

	EventDefinitionId getAboveHighThresholdEvent() override {
		return aboveHighThresholdEvent;
	}
};
#endif // ECSS_SERVICES_PMONBASE_HPP

