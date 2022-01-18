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

	std::reference_wrapper<ParameterBase> monitoredParameter;
	uint16_t monitoredParameterId;
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
	etl::array<CheckingStatus, 2> checkTransitionList;

	PMONBase(ParameterBase& monitoredParameter, uint16_t monitoredParameterId)
	    : monitoredParameter(std::ref(monitoredParameter)), monitoredParameterId(monitoredParameterId){};
};

/**
 * Contains the variables specific to Parameter Monitoring definitions of expected value check type.
 */
class PMONExpectedValueCheck : public PMONBase {
public:
	double expectedValue;
	uint16_t mask;
	uint16_t unexpectedValueEvent;

	explicit PMONExpectedValueCheck(ParameterBase& monitoredParameter, uint16_t monitoredParameterId)
	    : PMONBase(monitoredParameter, monitoredParameterId){};
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

	explicit PMONLimitCheck(ParameterBase& monitoredParameter, uint16_t monitoredParameterId)
	    : PMONBase(monitoredParameter, monitoredParameterId){};
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

	explicit PMONDeltaCheck(ParameterBase& monitoredParameter, uint16_t monitoredParameterId)
	    : PMONBase(monitoredParameter, monitoredParameterId){};
};

/**
 * Implementation of the ST[12] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, std::reference_wrapper<PMONBase>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

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
	/**
	 * The maximum time between two transition reports.
	 * Measured in "on-board parameter minimum sampling interval" units (see 5.4.3.2c in ECSS-E-ST-70-41C).
	 */
	uint16_t maximumTransitionReportingDelay = 0;
	/**
	 * If true, parameter monitoring is enabled
	 */
	bool parameterMonitoringFunctionStatus = false;
	/*
	 * Adds a new Parameter Monitoring definition to the parameter monitoring list.
	 */
	void addPMONDefinition(uint16_t PMONId, std::reference_wrapper<PMONBase> PMONDefinition){
		parameterMonitoringList.insert({PMONId, PMONDefinition});
	}
	/**
	 * @param PMONId
	 * @return Parameter Monitoring definition
	 */
	std::reference_wrapper<PMONBase> getPMONDefinition(uint16_t PMONId){
		return parameterMonitoringList.at(PMONId);
	}
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
