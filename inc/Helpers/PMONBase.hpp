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


	uint16_t monitoredParameterId;

	std::reference_wrapper<ParameterBase> monitoredParameter;
	/**
	 * The number of checks that need to be conducted in order to set a new Parameter Monitoring Status.
	 */
	uint16_t repetitionNumber;
	/**
	 * The number of checks that have been conducted so far.
	 */
	uint16_t repetitionCounter = 0;
	bool monitoringEnabled = false;
	CheckingStatus checkingStatus = Unchecked;
	etl::array<CheckingStatus, 2> checkTransitionList = {};
	CheckType checkType;

	uint16_t getRepetitionCounter() const {
		return repetitionCounter;
	}
	uint16_t getRepetitionNumber() const {
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

	virtual double getExpectedValue() {
		return 0.0; }
	virtual uint64_t getMask() {
		return 0; }
	virtual uint16_t getUnexpectedValueEvent() {
		return 0; }
	virtual double getLowLimit() {
		return 0.0; }
	virtual uint16_t getBelowLowLimitEvent() {
		return 0; }
	virtual double getHighLimit() {
		return 0.0; }
	virtual uint16_t getAboveHighLimitEvent() {
		return 0; }
	virtual uint16_t getNumberOfConsecutiveDeltaChecks() {
		return 0; }
	virtual double getLowDeltaThreshold() {
		return 0.0; }
	virtual uint16_t getBelowLowThresholdEvent() {
		return 0; }
	virtual double getHighDeltaThreshold() {
		return 0.0; }
	virtual uint16_t getAboveHighThresholdEvent() {
		return 0; }

protected:
	/**
	 * @param monitoredParameterId is assumed to be correct and not checked.
	 */
	PMONBase(uint16_t monitoredParameterId, uint16_t repetitionNumber);
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of expected value check type.
 */
class PMONExpectedValueCheck : public PMONBase {
public:
	double expectedValue;
	uint64_t mask;
	uint16_t unexpectedValueEvent;

	explicit PMONExpectedValueCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber, double expectedValue,
	                                uint64_t mask, uint16_t unexpectedValueEvent)
	    : expectedValue(expectedValue), mask(mask), unexpectedValueEvent(unexpectedValueEvent),
	      PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::ExpectedValue;
	};

	double getExpectedValue() override {
		return expectedValue;
	}

	uint64_t getMask() override {
		return mask;
	}

	uint16_t getUnexpectedValueEvent() override {
		return unexpectedValueEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of limit check type.
 */
class PMONLimitCheck : public PMONBase {
public:
	double lowLimit;
	uint16_t belowLowLimitEvent;
	double highLimit;
	uint16_t aboveHighLimitEvent;

	explicit PMONLimitCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber, double lowLimit,
	                        uint16_t belowLowLimitEvent, double highLimit, uint16_t aboveHighLimitEvent)
	    : lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Limit;
	};

	double getLowLimit() override {
		return lowLimit;
	}

	uint16_t getBelowLowLimitEvent() override {
		return belowLowLimitEvent;
	}

	double getHighLimit() override {
		return highLimit;
	}

	uint16_t getAboveHighLimitEvent() override {
		return aboveHighLimitEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of delta check type.
 */
class PMONDeltaCheck : public PMONBase {
public:
	uint16_t numberOfConsecutiveDeltaChecks;
	double lowDeltaThreshold;
	uint16_t belowLowThresholdEvent;
	double highDeltaThreshold;
	uint16_t aboveHighThresholdEvent;

	explicit PMONDeltaCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber,
	                        uint16_t numberOfConsecutiveDeltaChecks, double lowDeltaThreshold,
	                        uint16_t belowLowThresholdEvent, double highDeltaThreshold,
	                        uint16_t aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMONBase(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Delta;
	};

	uint16_t getNumberOfConsecutiveDeltaChecks() override {
		return numberOfConsecutiveDeltaChecks;
	}

	double getLowDeltaThreshold() override {
		return lowDeltaThreshold;
	}

	uint16_t getBelowLowThresholdEvent() override {
		return belowLowThresholdEvent;
	}

	double getHighDeltaThreshold() override {
		return highDeltaThreshold;
	}

	uint16_t getAboveHighThresholdEvent() override {
		return aboveHighThresholdEvent;
	}
};
#endif // ECSS_SERVICES_PMONBASE_HPP

