#ifndef ECSS_SERVICES_PMON_HPP
#define ECSS_SERVICES_PMON_HPP
#include <cstdint>
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "TimeGetter.hpp"
#include "etl/array.h"
#include "etl/functional.h"
#include "etl/map.h"
#include "etl/optional.h"
#include "ECSS_Definitions.hpp"

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
	 * The number of consecutive checks with the same result that need to be conducted in order to set a new Parameter Monitoring Status.
	 */
	PMONRepetitionNumber repetitionNumber;

	/**
	 * The number of consecutive checks with the same result that have been conducted so far.
	 */
	PMONRepetitionNumber repetitionCounter = 0;

	/**
	 * If false, the parameter of this PMON will not be checked, and no events will be generated if it goes off-bounds.
	 */
	bool monitoringEnabled = false;

	CheckingStatus checkingStatus = Unchecked;

	/**
	 * The list of Checking Statuses that have been recorded so far.
	 */
	etl::array<CheckingStatus, 2> checkTransitionList = {};

	/**
	 * The check type of this monitoring definition, set by the child classes to differentiate between class types
	 */
	CheckType checkType;

	/**
	 * Returns the number of consecutive checks with the same result that need to be conducted in order to set a new Parameter Monitoring Status.
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
	 * Returns the number of consecutive checks with the same result that have been conducted so far.
	 */
	PMONRepetitionNumber getRepetitionCounter() const {
		return repetitionCounter;
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

	/**
	 * Pure virtual function to be implemented by derived classes for performing the specific check.
	 * The function updates PMON::checkingStatus based on the result of the check.
	 *
	 * This function is expected to be called by the periodic monitoring system, which is responsible for
	 * invoking the check on each monitored parameter at regular intervals. It ensures that parameters are
	 * within their defined limits, match expected values, or have acceptable delta changes over time.
	 *
	 * After the check, the function compares the current checking status to the previous one.
	 * If they are the same, the repetition counter is incremented. If they are different, the repetition counter is reset to 1.
	 *
	 * @note
	 * It is crucial that this function is called periodically and consistently every @ref ECSSMonitoringFrequency to ensure the
	 * reliability of the monitoring system. Irregular calls or missed checks can lead to incorrect status updates and potentially
	 * missed parameter anomalies.
	 *
	 * @note
	 * This function does not ensure that a monitoring definition is _enabled_. It will
	 * perform a check even if the PMON definition is _disabled_.
	 *
	 */
	virtual void performCheck() = 0;

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

	/**
	 * @brief Performs the check for the PMONExpectedValueCheck class.
	 *
	 * This function first retrieves the current value of the monitored parameter and applies the bit mask to it.
	 * It then compares the masked value to the expected value. If they match, the checking status is set to ExpectedValue.
	 * If they don't match, the checking status is set to UnexpectedValue.
	 *
	 * @note This function overrides the pure virtual function in the base PMON class.
	 */
	void performCheck() override {
		auto previousStatus = checkingStatus;
		auto currentValueAsUint64 = monitoredParameter.get().getValueAsUint64();
		uint64_t maskedValue = currentValueAsUint64 & getMask();

		if (maskedValue == getExpectedValue()) {
			checkingStatus = ExpectedValue;
		} else {
			checkingStatus = UnexpectedValue;
		}

		if (checkingStatus == previousStatus) {
			repetitionCounter++;
		} else {
			repetitionCounter = 1;
		}
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
	    : PMON(monitoredParameterId, repetitionNumber, CheckType::Limit), lowLimit(lowLimit), belowLowLimitEvent(belowLowLimitEvent), highLimit(highLimit),
	      aboveHighLimitEvent(aboveHighLimitEvent) {
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

	/**
	 * @brief Performs the check for the PMONLimitCheck class.
	 *
	 * This function first retrieves the current value of the monitored parameter.
	 * It then checks if the current value is below the low limit, above the high limit, or within the limits.
	 * Depending on the comparison, it sets the checking status to BelowLowLimit, AboveHighLimit, or WithinLimits.
	 *
	 * @note This function overrides the pure virtual function in the base PMON class.
	 */
	void performCheck() override {
		auto previousStatus = checkingStatus;
		auto currentValue = monitoredParameter.get().getValueAsDouble();
		if (currentValue < getLowLimit()) {
			checkingStatus = BelowLowLimit;
		} else if (currentValue > getHighLimit()) {
			checkingStatus = AboveHighLimit;
		} else {
			checkingStatus = WithinLimits;
		}

		if (checkingStatus == previousStatus) {
			repetitionCounter++;
		} else {
			repetitionCounter = 1;
		}
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
	double previousValue;
	etl::optional<Time::DefaultCUC> previousTimestamp;

public:
	explicit PMONDeltaCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, DeltaThreshold lowDeltaThreshold,
	                        EventDefinitionId belowLowThresholdEvent, DeltaThreshold highDeltaThreshold,
	                        EventDefinitionId aboveHighThresholdEvent)
	    : PMON(monitoredParameterId, repetitionNumber, CheckType::Delta), numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), previousValue(0.0) {
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
	 * This method updates the last value and timestamp of the PMONDeltaCheck object.
	 */
	void updatePreviousValueAndTimestamp(double newValue, const Time::DefaultCUC& newTimestamp) {
		previousValue = newValue;
		previousTimestamp = newTimestamp;
	}

	/**
	 * Returns the delta per second between the current value and the previous one.
	 */
	double getDeltaPerSecond(double currentValue) const {
		if (previousTimestamp.has_value()) {
			double delta = currentValue - previousValue;
			auto duration = TimeGetter::getCurrentTimeDefaultCUC() - *previousTimestamp;
			double deltaSeconds = std::chrono::duration<double>(duration).count();

			if (deltaSeconds == 0) {
				return 0;
			}
			return delta / deltaSeconds;
		}
		return 0;
	}

	/**
	 * This method checks if the PMON has a previous value by verifying if the previous timestamp has value.
	 */
	bool hasOldValue() const {
		return previousTimestamp.has_value();
	}

	/**
	 * @brief Performs the check for the PMONDeltaCheck class.
	 *
	 * This function first retrieves the current value of the monitored parameter and the current timestamp.
	 * If there is a previous value, it calculates the delta per second between the current and previous values.
	 * Depending on the delta per second, it sets the checking status to BelowLowThreshold, AboveHighThreshold, or WithinThreshold.
	 * If there is no previous value, it sets the checking status to Invalid.
	 *
	 * After the check, the function updates the previous value and timestamp to the current ones.
	 *
	 * @note This function overrides the pure virtual function in the base PMON class.
	 * @note The delta check is performed on the actual difference between the previous and the current
	 * value ($\Delta = \mathrm{current} - \mathrm{last}$). No absolute value is considered.
	 */
	void performCheck() override {
		auto previousStatus = checkingStatus;
		auto currentValue = monitoredParameter.get().getValueAsDouble();
		auto currentTimestamp = TimeGetter::getCurrentTimeDefaultCUC();

		if (hasOldValue()) {
			double deltaPerSecond = getDeltaPerSecond(currentValue);
			if (deltaPerSecond < getLowDeltaThreshold()) {
				checkingStatus = BelowLowThreshold;
			} else if (deltaPerSecond > getHighDeltaThreshold()) {
				checkingStatus = AboveHighThreshold;
			} else {
				checkingStatus = WithinThreshold;
			}
		} else {
			checkingStatus = Invalid;
		}

		updatePreviousValueAndTimestamp(currentValue, currentTimestamp);

		if (checkingStatus == previousStatus) {
			repetitionCounter++;
		} else {
			repetitionCounter = 1;
		}
	}
};
#endif // ECSS_SERVICES_PMON_HPP
