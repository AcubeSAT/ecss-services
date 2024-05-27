#ifndef ECSS_SERVICES_PMON_HPP
#define ECSS_SERVICES_PMON_HPP
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "TimeGetter.hpp"
#include "etl/array.h"
#include "etl/functional.h"
#include "etl/list.h"
#include "etl/map.h"
#include "etl/optional.h"

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
	etl::reference_wrapper<ParameterBase> monitoredParameter;
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
	etl::optional<CheckType> getCheckType() const {
		return checkType;
	}

	/**
	 * Returns the current Checking Status.
	 */
	CheckingStatus getCheckingStatus() const {
		return checkingStatus;
	}

protected:
	/**
	 * @param monitoredParameterId is assumed to be correct and not checked.
	 */
	PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType);
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
	    : PMON(monitoredParameterId, repetitionNumber, CheckType::ExpectedValue),
	      expectedValue(expectedValue),
	      mask(mask),
	      unexpectedValueEvent(unexpectedValueEvent) {
	}

	/**
	 * Returns the value of the bit mask used in an Expected Value Check.
	 */
	PMONBitMask getMask() const {
		return mask;
	}

	/**
	 * Returns the value resulting from applying the bit mask.
	 */
	PMONExpectedValue getExpectedValue() const {
		return expectedValue;
	}

	/**
	 * Returns the Id of an Unexpected Value Event.
	 */
	EventDefinitionId getUnexpectedValueEvent() const {
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
	      aboveHighLimitEvent(aboveHighLimitEvent), PMON(monitoredParameterId, repetitionNumber, CheckType::Limit) {
	}

	/**
	 * Returns the value of the Low PMONLimit used on a PMONLimit Check.
	 */
	PMONLimit getLowLimit() const {
		return lowLimit;
	}

	/**
	 * Returns the Id of a Below Low PMONLimit Event.
	 */
	EventDefinitionId getBelowLowLimitEvent() const {
		return belowLowLimitEvent;
	}

	/**
	 * Returns the value of the High PMONLimit used on a PMONLimit Check.
	 */
	PMONLimit getHighLimit() const {
		return highLimit;
	}

	/**
	 * Returns the Id of a High PMONLimit Event.
	 */
	EventDefinitionId getAboveHighLimitEvent() const {
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

private:
	double currentValue;
	double previousValue;
	Time::DefaultCUC currentTimestamp;
	Time::DefaultCUC previousTimestamp;

public:
	explicit PMONDeltaCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, DeltaThreshold lowDeltaThreshold,
	                        EventDefinitionId belowLowThresholdEvent, DeltaThreshold highDeltaThreshold,
	                        EventDefinitionId aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMON(monitoredParameterId, repetitionNumber, CheckType::Delta),
	      currentValue(0.0), previousValue(0.0) {
	}

	/**
	 * Returns the number of consecutive Delta Checks.
	 */
	NumberOfConsecutiveDeltaChecks getNumberOfConsecutiveDeltaChecks() const {
		return numberOfConsecutiveDeltaChecks;
	}

	/**
	 * Returns the value of the Low Threshold used on a Delta Check.
	 */
	DeltaThreshold getLowDeltaThreshold() const {
		return lowDeltaThreshold;
	}

	/**
	 * Returns the Id of a Below Low Threshold Event.
	 */
	EventDefinitionId getBelowLowThresholdEvent() const {
		return belowLowThresholdEvent;
	}

	/**
	 * Returns the value of the High Threshold used on a Delta Check.
	 */
	DeltaThreshold getHighDeltaThreshold() const {
		return highDeltaThreshold;
	}

	/**
	 * Returns the Id of an Above High Threshold Event.
	 */
	EventDefinitionId getAboveHighThresholdEvent() const {
		return aboveHighThresholdEvent;
	}

	/**
	 * @brief Updates the values and timestamps for the PMONDeltaCheck object.
	 *
	 * This method is used to update the current and previous values and timestamps
	 * of the PMONDeltaCheck object. The current value and timestamp are moved to
	 * their respective previous value and timestamp variables, and the new current
	 * value and timestamp are set to the provided value and the current time
	 *
	 * @param newValue The new value to be set as the current value.
	 */
	void updateValuesAndTimestamps(double newValue) {
		previousValue = currentValue;
		previousTimestamp = currentTimestamp;
		currentValue = newValue;
		currentTimestamp = TimeGetter::getCurrentTimeDefaultCUC();
	}

	/**
	 * This method calculates the difference between the current and previous values of the monitored parameter,
	 * and divides it by the time difference between the current and previous timestamps. The result is the rate
	 * of change of the monitored parameter per second.
	 *
	 * @return The rate of change of the monitored parameter per second.
	 */
	double getDeltaPerSecond() const {
		double delta = currentValue - previousValue;
		auto duration = currentTimestamp - previousTimestamp;
		double deltaTime = std::chrono::duration<double>(duration).count();
		return delta / deltaTime;
	}

	bool isPreviousTimestampValid() const {
		return previousTimestamp.isValid();
	}
};
#endif // ECSS_SERVICES_PMON_HPP
