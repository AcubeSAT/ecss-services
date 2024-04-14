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
#include "etl/optional.h"
#include "etl/functional.h"

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

	etl::optional<etl::reference_wrapper<ParameterBase>> monitoredParameter;
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
	etl::optional<CheckType> checkType;

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

	/**
	 *  Minimal overhead to enable polymorphism
	 */
	PMON(const PMON& other) // copy constructor
	    : monitoredParameterId(other.monitoredParameterId),
	      repetitionNumber(other.repetitionNumber),
	      repetitionCounter(other.repetitionCounter),
	      monitoringEnabled(other.monitoringEnabled),
	      checkingStatus(other.checkingStatus),
	      checkTransitionList(other.checkTransitionList),
	      checkType(other.checkType) {
		if (other.monitoredParameter) {
			monitoredParameter = other.monitoredParameter;
		}
	}

	PMON& operator=(const PMON& other) = delete; // copy assignment operator
	PMON(PMON&&) = delete; // move constructor
	PMON& operator=(PMON&&) = delete; // move assignment operator

	virtual ~PMON() = default; // destructor

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

	PMONExpectedValueCheck(const PMONExpectedValueCheck& other)
	    : PMON(other),
	      expectedValue(other.expectedValue),
	      mask(other.mask),
	      unexpectedValueEvent(other.unexpectedValueEvent) {
	}

	PMONExpectedValueCheck& operator=(const PMONExpectedValueCheck&) = delete; // copy assignment operator
	PMONExpectedValueCheck(PMONExpectedValueCheck&&) = delete; // move constructor
	PMONExpectedValueCheck& operator=(PMONExpectedValueCheck&&) = delete; // move assignment operator

	virtual ~PMONExpectedValueCheck() = default; // destructor

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
	      aboveHighLimitEvent(aboveHighLimitEvent), PMON(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Limit;
	};

	PMONLimitCheck(const PMONLimitCheck& other) // copy constructor
	    : PMON(other),
	      lowLimit(other.lowLimit),
	      belowLowLimitEvent(other.belowLowLimitEvent),
	      highLimit(other.highLimit),
	      aboveHighLimitEvent(other.aboveHighLimitEvent) {
	}

	PMONLimitCheck& operator=(const PMONLimitCheck&) = delete; // copy assignment operator
	PMONLimitCheck(PMONLimitCheck&&) = delete; // move constructor
	PMONLimitCheck& operator=(PMONLimitCheck&&) = delete; // move assignment operator

	virtual ~PMONLimitCheck() = default; // destructor

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

	explicit PMONDeltaCheck(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber,
	                        NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks, DeltaThreshold lowDeltaThreshold,
	                        EventDefinitionId belowLowThresholdEvent, DeltaThreshold highDeltaThreshold,
	                        EventDefinitionId aboveHighThresholdEvent)
	    : numberOfConsecutiveDeltaChecks(numberOfConsecutiveDeltaChecks), lowDeltaThreshold(lowDeltaThreshold),
	      belowLowThresholdEvent(belowLowThresholdEvent), highDeltaThreshold(highDeltaThreshold),
	      aboveHighThresholdEvent(aboveHighThresholdEvent), PMON(monitoredParameterId, repetitionNumber) {
		checkType = CheckType::Delta;
	};

	PMONDeltaCheck(const PMONDeltaCheck& other) // copy constructor
	    : PMON(other),
	      numberOfConsecutiveDeltaChecks(other.numberOfConsecutiveDeltaChecks),
	      lowDeltaThreshold(other.lowDeltaThreshold),
	      belowLowThresholdEvent(other.belowLowThresholdEvent),
	      highDeltaThreshold(other.highDeltaThreshold),
	      aboveHighThresholdEvent(other.aboveHighThresholdEvent) {
	}

	PMONDeltaCheck& operator=(const PMONDeltaCheck&) = delete; // copy assignment operator
	PMONDeltaCheck(PMONDeltaCheck&&) = delete; // move constructor
	PMONDeltaCheck& operator=(PMONDeltaCheck&&) = delete; // move assignment operator

	virtual ~PMONDeltaCheck() = default; // destructor

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
};
#endif // ECSS_SERVICES_PMON_HPP
