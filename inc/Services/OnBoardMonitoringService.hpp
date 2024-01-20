#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/PMON.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "etl/array.h"
#include "etl/list.h"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * Implementation of the ST[12] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 * @ingroup Services
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com> and Thomas Pravinos <tompravi99@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, std::reference_wrapper<PMON>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

	/**
	 * Maximum number of checks for each check type.
	 */
	static constexpr uint8_t MaximumNumberOfChecksLimitCheck = 32;

	static constexpr uint8_t MaximumNumberOfChecksExpectedValueCheck = 32;

	static constexpr uint8_t MaximumNumberOfChecksDeltaCheck = 32;

public:
	inline static constexpr ServiceTypeNum ServiceType = 12;
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

	OnBoardMonitoringService() {
		serviceType = ServiceType;
	}

	etl::vector<PMONLimitCheck, MaximumNumberOfChecksLimitCheck> limitChecks;
	etl::vector<PMONExpectedValueCheck, MaximumNumberOfChecksExpectedValueCheck> expectedValueChecks;
	etl::vector<PMONDeltaCheck, MaximumNumberOfChecksDeltaCheck> deltaChecks;

	/**
	 * The maximum time between two transition reports.
	 * Measured in "on-board parameter minimum sampling interval" units (see 5.4.3.2c in ECSS-E-ST-70-41C).
	 */
	uint16_t maximumTransitionReportingDelay = 0;

	/**
	 * If true, parameter monitoring is enabled
	 */
	bool parameterMonitoringFunctionStatus = false;

	/**
	 * Adds a new Parameter Monitoring definition to the parameter monitoring list.
	 */
	void addPMONDefinition(ParameterId PMONId, std::reference_wrapper<PMON> PMONDefinition) {
		parameterMonitoringList.insert({PMONId, PMONDefinition});
	}

	/**
	 * Adds a new Parameter Monitoring Limit Check to the parameter monitoring list.
	 */
	void addPMONLimitCheck(ParameterId PMONId, PMONLimitCheck& limitCheck) {
		limitChecks.push_back(limitCheck);
		parameterMonitoringList.insert({PMONId, std::ref(limitChecks.back())});
	}

	/**
	 * Adds a new Parameter Monitoring Expected Value Check to the parameter monitoring list.
	 */
	void addPMONExpectedValueCheck(ParameterId PMONId, PMONExpectedValueCheck& expectedValueCheck) {
		expectedValueChecks.push_back(expectedValueCheck);
		parameterMonitoringList.insert({PMONId, std::ref(expectedValueChecks.back())});
	}

	/**
	 * Adds a new Parameter Monitoring Delta Check to the parameter monitoring list.
	 */
	void addPMONDeltaCheck(ParameterId PMONId, PMONDeltaCheck& deltaCheck) {
		deltaChecks.push_back(deltaCheck);
		parameterMonitoringList.insert({PMONId, std::ref(deltaChecks.back())});
	}

	/**
	 * @param PMONId
	 * @return Parameter Monitoring definition
	 */
	std::reference_wrapper<PMON> getPMONDefinition(ParameterId PMONId) {
		return parameterMonitoringList.at(PMONId);
	}

	/**
	 * @return true if PMONList is empty.
	 */
	bool isPMONListEmpty() {
		return parameterMonitoringList.empty();
	}

	/**
	 * @return The number of occurrences of the specified key in the parameter monitoring list.
	 */
	uint16_t getCount(uint16_t key) const {
		return parameterMonitoringList.count(key);
	}

	/**
	 * Enables the PMON definitions which correspond to the ids in TC[12,1].
	 */
	void enableParameterMonitoringDefinitions(Message& message);

	/**
	 * Disables the PMON definitions which correspond to the ids in TC[12,2].
	 */
	void disableParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,3]
	 * Changes the maximum time between two transition reports.
	 */
	void changeMaximumTransitionReportingDelay(Message& message);

	/**
	 * TC[12,4]
	 * Deletes all the PMON definitions in the PMON list.
	 */
	void deleteAllParameterMonitoringDefinitions(const Message& message);

	/**
	 * TC[12,5]
	 */
	void addParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,6]
	 */
	void deleteParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,7]
	 */
	void modifyParameterMonitoringDefinitions(Message& message);

	/**
	 * TC[12,8]
	 */
	void reportParameterMonitoringDefinitions(Message& message);

	/**
	 * TM[12,9]
	 */
	void parameterMonitoringDefinitionReport(Message& message);

	/**
	 * TC[12,10]
	 */
	void reportOutOfLimits(Message& message);

	/**
	 * TM[12,11]
	 */
	void outOfLimitsReport();

	/**
	 * TM[12,12]
	 */
	void checkTransitionReport();

	/**
	 * TC[12,13]
	 */
	void reportStatusOfParameterMonitoringDefinition(Message& message);

	/**
	 * TM[12,14]
	 */
	void parameterMonitoringDefinitionStatusReport();

	void execute(Message& message);
};

#endif // ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP