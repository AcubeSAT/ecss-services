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

	enum CheckType : uint8_t { LimitCheck = 1,
		                       ExpectedValueCheck = 2,
		                       DeltaCheck = 3 };

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

	virtual double getExpectedValue(){};
	virtual uint64_t getMask(){};
	virtual uint16_t getUnexpectedValueEvent(){};

	virtual double getLowLimit(){};
	virtual uint16_t getBelowLowLimitEvent(){};
	virtual double getHighLimit(){};
	virtual uint16_t getAboveHighLimitEvent(){};

	virtual uint16_t getNumberOfConsecutiveDeltaChecks(){};
	virtual double getLowDeltaThreshold(){};
	virtual uint16_t getBelowLowThresholdEvent(){};
	virtual double getHighDeltaThreshold(){};
	virtual uint16_t getAboveHighThresholdEvent(){};

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
	CheckType checkType = ExpectedValueCheck;

	explicit PMONExpectedValueCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber, double expectedValue,
	                                uint64_t mask, uint16_t unexpectedValueEvent)
	    : expectedValue(expectedValue), mask(mask), unexpectedValueEvent(unexpectedValueEvent),
	      PMONBase(monitoredParameterId, repetitionNumber){};

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
	CheckType checkType = LimitCheck;

	explicit PMONLimitCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber, double lowLimit,
	                        uint16_t belowLowLimitEvent, double highLimit, uint16_t aboveHighLimitEvent)
	    : lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent), PMONBase(monitoredParameterId, repetitionNumber){};

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
	CheckType checkType = DeltaCheck;

	explicit PMONDeltaCheck(uint16_t monitoredParameterId, uint16_t repetitionNumber,
	                        uint16_t numberOfConsecutiveDeltaChecks, double lowDeltaThreshold,
	                        uint16_t belowLowThresholdEvent, double highDeltaThreshold,
	                        uint16_t aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMONBase(monitoredParameterId, repetitionNumber){};

	uint16_t getNumberOfConsecutiveDeltaChecks() override{
		return numberOfConsecutiveDeltaChecks;
	}

	double getLowDeltaThreshold() override{
		return lowDeltaThreshold;
	}

	uint16_t getBelowLowThresholdEvent() override{
		return belowLowThresholdEvent;
	}

	uint16_t getAboveHighThresholdEvent() override{
		return aboveHighThresholdEvent;
	}
};
#endif // ECSS_SERVICES_PMONBASE_HPP
