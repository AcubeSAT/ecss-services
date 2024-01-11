#ifndef ECSS_SERVICES_PMON_HPP
#define ECSS_SERVICES_PMON_HPP
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
class PMON {
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
	PMONRepetitionNumber repetitionNumber;
	/**
	 * The number of checks that have been conducted so far.
	 */
	uint16_t repetitionCounter = 0;
	bool monitoringEnabled = false;
	CheckingStatus checkingStatus = Unchecked;
	etl::array<CheckingStatus, 2> checkTransitionList = {};
	CheckType checkType;

	/**
	 * Returns the number of checks that need to be conducted in order to set a new Parameter Monitoring Status.
	 */
	PMONRepetitionNumber getRepetitionNumber() const {
		return repetitionNumber;
	}

	/**
	 * Returns True if Monitoring is enabled, False otherwise.
	 */
	bool isMonitoringEnabled() const {
		return monitoringEnabled;
	}

	/**
	 * Returns the current Check Type.
	 */
	CheckType getCheckType() const {
		return checkType;
	}

	/**
	 * Returns the current Checking Status.
	 */
	CheckingStatus getCheckingStatus() const {
		return checkingStatus;
	}

	virtual PMONExpectedValue getExpectedValue() {
		return 0.0; }
	virtual PMONBitMask getMask() {
		return 0; }
	virtual EventDefinitionId getUnexpectedValueEvent() {
		return 0; }
	virtual PMONLimit getLowLimit() {
		return 0.0; }
	virtual EventDefinitionId getBelowLowLimitEvent() {
		return 0; }
	virtual PMONLimit getHighLimit() {
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
	PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber);
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of expected value check type.
 */
class PMONExpectedValueCheck : public PMON {
public:
	PMONExpectedValue expectedValue;
	PMONBitMask mask;
	EventDefinitionId unexpectedValueEvent;

	explicit PMONExpectedValueCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, PMONExpectedValue expectedValue,
	                                PMONBitMask mask, EventDefinitionId unexpectedValueEvent)
	    : expectedValue(expectedValue), mask(mask), unexpectedValueEvent(unexpectedValueEvent),
	      PMON(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::ExpectedValue;
	};

	/**
	 * Returns the value of the bit mask used in an Expected Value Check.
	 */
	PMONBitMask getMask() override {
		return mask;
	}

	/**
	 * Returns the value resulting from applying the bit mask.
	 */
	PMONExpectedValue getExpectedValue() override {
		return expectedValue;
	}

	/**
	 * Returns the Id of an Unexpected Value Event.
	 */
	EventDefinitionId getUnexpectedValueEvent() override {
		return unexpectedValueEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of limit check type.
 */
class PMONLimitCheck : public PMON {
public:
	PMONLimit lowLimit;
	EventDefinitionId belowLowLimitEvent;
	PMONLimit highLimit;
	EventDefinitionId aboveHighLimitEvent;

	explicit PMONLimitCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, PMONLimit lowLimit,
	                        EventDefinitionId belowLowLimitEvent, PMONLimit highLimit, EventDefinitionId aboveHighLimitEvent)
	    : lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent), PMON(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Limit;
	};

	/**
	 * Returns the value of the Low PMONLimit used on a PMONLimit Check.
	 */
	PMONLimit getLowLimit() override {
		return lowLimit;
	}

	/**
	 * Returns the Id of a Below Low PMONLimit Event.
	 */
	EventDefinitionId getBelowLowLimitEvent() override {
		return belowLowLimitEvent;
	}

	/**
	 * Returns the value of the High PMONLimit used on a PMONLimit Check.
	 */
	PMONLimit getHighLimit() override {
		return highLimit;
	}

	/**
	 * Returns the Id of a High PMONLimit Event.
	 */
	EventDefinitionId getAboveHighLimitEvent() override {
		return aboveHighLimitEvent;
	}
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of delta check type.
 */
class PMONDeltaCheck : public PMON {
public:
	NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks;
	DeltaThreshold lowDeltaThreshold;
	EventDefinitionId belowLowThresholdEvent;
	DeltaThreshold highDeltaThreshold;
	EventDefinitionId aboveHighThresholdEvent;

	explicit PMONDeltaCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, DeltaThreshold lowDeltaThreshold,
	                        EventDefinitionId belowLowThresholdEvent, DeltaThreshold highDeltaThreshold,
	                        EventDefinitionId aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMON(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Delta;
	};

	/**
	 * Returns the number of consecutive Delta Checks.
	 */
	NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() override {
		return numberOfConsecutiveDeltaChecks;
	}

	/**
	 * Returns the value of the Low Threshold used on a Delta Check.
	 */
	DeltaThreshold getLowDeltaThreshold() override {
		return lowDeltaThreshold;
	}

	/**
	 * Returns the Id of a Below Low Threshold Event.
	 */
	EventDefinitionId getBelowLowThresholdEvent() override {
		return belowLowThresholdEvent;
	}

	/**
	 * Returns the value of the High Threshold used on a Delta Check.
	 */
	DeltaThreshold getHighDeltaThreshold() override {
		return highDeltaThreshold;
	}

	/**
	 * Returns the Id of an Above High Threshold Event.
	 */
	EventDefinitionId getAboveHighThresholdEvent() override {
		return aboveHighThresholdEvent;
	}
};
#endif // ECSS_SERVICES_PMON_HPP
