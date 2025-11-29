#pragma once
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/PMON.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "etl/array.h"
#include "etl/functional.h"
#include "etl/list.h"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * Implementation of the ST[12] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 * @ingroup Services
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com>
 * @author Thomas Pravinos <tompravi99@gmail.com>
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, etl::reference_wrapper <PMON>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

	/**
	 * Maximum number of checks for each Limit Check.
	 */
	static constexpr uint8_t MaximumNumberOfChecksLimitCheck = 32;

	/**
	 * Maximum number of checks for each Expected Value Check.
	 */
	static constexpr uint8_t MaximumNumberOfChecksExpectedValueCheck = 32;

	/**
	 * Maximum number of checks for each Delta check.
	 */
	static constexpr uint8_t MaximumNumberOfChecksDeltaCheck = 32;

	/**
	 * This vector is used as a mean of storing the PMON Definitons
	 * resulting from the addParameterMonitoringDefinitions method according to their Check Type
	 * before they are added to the parameterMonitoringList.
	 */
	etl::vector<PMONLimitCheck, MaximumNumberOfChecksLimitCheck> limitChecks;

	/**
	 * This vector is used as a mean of storing the PMON Definitons
	 * resulting from the addParameterMonitoringDefinitions method according to their Check Type
	 * before they are added to the parameterMonitoringList.
	 */
	etl::vector<PMONExpectedValueCheck, MaximumNumberOfChecksExpectedValueCheck> expectedValueChecks;

	/**
	 * This vector is used as a mean of storing the PMON Definitons
	 * resulting from the addParameterMonitoringDefinitions method according to their Check Type
	 * before they are added to the parameterMonitoringList.
	 */
	etl::vector<PMONDeltaCheck, MaximumNumberOfChecksDeltaCheck> deltaChecks;

	/**
	 * If true, parameter monitoring is enabled
	 */
	bool parameterMonitoringFunctionStatus = false;

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
		ParameterMonitoringDefinitionStatusReport = 14,
		EnableParameterMonitoringFunctions = 15,
		DisableParameterMonitoringFunctions = 16
	};

	OnBoardMonitoringService() {
		serviceType = ServiceType;
	}

	explicit
	OnBoardMonitoringService(const bool initialParameterMonitoringFunctionStatus) : parameterMonitoringFunctionStatus(
		initialParameterMonitoringFunctionStatus) {
	};

	/**
	 * The maximum time between two transition reports.
	 * Measured in "on-board parameter minimum sampling interval" units (see 5.4.3.2c in ECSS-E-ST-70-41C).
	 */
	uint16_t maximumTransitionReportingDelay = 0;

	/**
	 * Adds a new Parameter Monitoring Limit Check to the parameter monitoring list.
	 */
	void addPMONLimitCheck(ParameterId PMONId, PMONLimitCheck& limitCheck) {
		limitChecks.push_back(limitCheck);
		parameterMonitoringList.insert(
			etl::pair<const ParameterId, etl::reference_wrapper <PMON> >(PMONId, etl::ref(limitChecks.back())));
	}


	/**
	 * Adds a new Parameter Monitoring Expected Value Check to the parameter monitoring list.
	 */
	void addPMONExpectedValueCheck(ParameterId PMONId, PMONExpectedValueCheck& expectedValueCheck) {
		expectedValueChecks.push_back(expectedValueCheck);
		parameterMonitoringList.insert(
			etl::pair<const ParameterId, etl::reference_wrapper <PMON> >(PMONId,
				etl::ref(expectedValueChecks.back())));
	}

	/**
	 * Adds a new Parameter Monitoring Delta Check to the parameter monitoring list.
	 */
	void addPMONDeltaCheck(ParameterId PMONId, PMONDeltaCheck& deltaCheck) {
		deltaChecks.push_back(deltaCheck);
		parameterMonitoringList.insert(
			etl::pair<const ParameterId, etl::reference_wrapper <PMON> >(PMONId, etl::ref(deltaChecks.back())));
	}

	/**
	 * This function clears the Parameter Monitoring List map.
	 */
	void clearParameterMonitoringList() {
		parameterMonitoringList.clear();
	}

	/**
	 * @param PMONId
	 * @return Parameter Monitoring definition
	 */
	etl::reference_wrapper<PMON> getPMONDefinition(ParameterId PMONId) {
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
	 * Checks all PMON objects in the parameter monitoring list if they are enabled.
	 * This function iterates through all PMON objects in the parameter monitoring list
	 * and calls the performCheck method for each enabled PMON.
	 */
	void checkAll() const;

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
	void parameterMonitoringDefinitionReport();

	/**
	 * TC[12,10]
	 */
	void reportOutOfLimits(const Message& message);

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
	void reportStatusOfParameterMonitoringDefinition(const Message& message);

	/**
	 * TM[12,14]
	 */
	void parameterMonitoringDefinitionStatusReport();

	/**
	 * TC[12,15]
	 * @param message The TC message
	 */
	void enableParameterMonitoringFunction(const Message& message);

	/**
	 * TC[12,16]
	 * @param message The TC message
	 */
	void disableParameterMonitoringFunction(const Message& message);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);

	/**
	 * @return The current status of parameter monitoring function
	 */
	bool getParameterMonitoringFunctionStatus() const {
		return parameterMonitoringFunctionStatus;
	}
};

