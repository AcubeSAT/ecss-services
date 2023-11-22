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


	MonitoredParameterId monitoredParameterId;

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

	RepetitionCounter getRepetitionCounter() const {
		return repetitionCounter;
	}
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
	virtual UnexpectedValueEvent getUnexpectedValueEvent() {
		return 0; }
	virtual LowLimit getLowLimit() {
		return 0.0; }
	virtual BelowLowLimitEvent getBelowLowLimitEvent() {
		return 0; }
	virtual HighLimit getHighLimit() {
		return 0.0; }
	virtual AboveHighLimitEvent getAboveHighLimitEvent() {
		return 0; }
	virtual NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() {
		return 0; }
	virtual LowDeltaThreshold getLowDeltaThreshold() {
		return 0.0; }
	virtual BelowLowThresholdEvent getBelowLowThresholdEvent() {
		return 0; }
	virtual HighDeltaThreshold getHighDeltaThreshold() {
		return 0.0; }
	virtual AboveHighThresholdEvent getAboveHighThresholdEvent() {
		return 0; }

protected:
	/**
	 * @param monitoredParameterId is assumed to be correct and not checked.
	 */
	PMONBase(MonitoredParameterId monitoredParameterId, RepetitionNumber repetitionNumber);
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of expected value check type.
 */
class PMONExpectedValueCheck : public PMONBase {
public:
	Expected_Value expectedValue;
	Mask mask;
	UnexpectedValueEvent unexpectedValueEvent;

	explicit PMONExpectedValueCheck(MonitoredParameterId monitoredParameterId, RepetitionNumber repetitionNumber, Expected_Value expectedValue,
	                                Mask mask, UnexpectedValueEvent unexpectedValueEvent)
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

	UnexpectedValueEvent getUnexpectedValueEvent() override {
		return unexpectedValueEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of limit check type.
 */
class PMONLimitCheck : public PMONBase {
public:
	LowLimit lowLimit;
	BelowLowLimitEvent belowLowLimitEvent;
	HighLimit highLimit;
	AboveHighLimitEvent aboveHighLimitEvent;

	explicit PMONLimitCheck(MonitoredParameterId monitoredParameterId, RepetitionNumber repetitionNumber, LowLimit lowLimit,
	                        BelowLowLimitEvent belowLowLimitEvent, HighLimit highLimit, AboveHighLimitEvent aboveHighLimitEvent)
	    : lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Limit;
	};

	LowLimit getLowLimit() override {
		return lowLimit;
	}

	BelowLowLimitEvent getBelowLowLimitEvent() override {
		return belowLowLimitEvent;
	}

	HighLimit getHighLimit() override {
		return highLimit;
	}

	AboveHighLimitEvent getAboveHighLimitEvent() override {
		return aboveHighLimitEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of delta check type.
 */
class PMONDeltaCheck : public PMONBase {
public:
	NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks;
	LowDeltaThreshold lowDeltaThreshold;
	BelowLowThresholdEvent belowLowThresholdEvent;
	HighDeltaThreshold highDeltaThreshold;
	AboveHighThresholdEvent aboveHighThresholdEvent;

	explicit PMONDeltaCheck(MonitoredParameterId monitoredParameterId, RepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, LowDeltaThreshold lowDeltaThreshold,
	                        BelowLowThresholdEvent belowLowThresholdEvent, HighDeltaThreshold highDeltaThreshold,
	                        AboveHighThresholdEvent aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Delta;
	};

	NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() override {
		return numberOfConsecutiveDeltaChecks;
	}

	LowDeltaThreshold getLowDeltaThreshold() override {
		return lowDeltaThreshold;
	}

	BelowLowThresholdEvent getBelowLowThresholdEvent() override {
		return belowLowThresholdEvent;
	}

	HighDeltaThreshold getHighDeltaThreshold() override {
		return highDeltaThreshold;
	}

	AboveHighThresholdEvent getAboveHighThresholdEvent() override {
		return aboveHighThresholdEvent;
	}
};
#endif // ECSS_SERVICES_PMONBASE_HPP

