#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#include <algorithm>
#include <cstdint>
#include "ECSS_Definitions.hpp"
#include "Helpers/PMON.hpp"
#include "Helpers/Parameter.hpp"
#include "Message.hpp"
#include "Service.hpp"
#include "etl/array.h"
#include "etl/functional.h"
#include "etl/list.h"
#include "etl/flat_map.h"
#include "etl/vector.h"

/**
 * Implementation of the ST[12] parameter statistics reporting service, as defined in ECSS-E-ST-70-41C.
 * @ingroup Services
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com> and Thomas Pravinos <tompravi99@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Flat map storing the parameter monitoring definitions.
	 */
	etl::flat_map<uint16_t, etl::reference_wrapper<PMON>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

	/**
	 * Storage for the limit checks that parameterMonitoringList refers to. A slot is reused once no definition refers
	 * to it, so the storage never needs more slots than the list has entries.
	 */
	etl::vector<PMONLimitCheck, ECSSMaxMonitoringDefinitions> limitChecks;

	/**
	 * Storage for the expected value checks that parameterMonitoringList refers to, reused like limitChecks.
	 */
	etl::vector<PMONExpectedValueCheck, ECSSMaxMonitoringDefinitions> expectedValueChecks;

	/**
	 * Storage for the delta checks that parameterMonitoringList refers to, reused like limitChecks.
	 */
	etl::vector<PMONDeltaCheck, ECSSMaxMonitoringDefinitions> deltaChecks;

	/**
	 * @return true if a definition in the parameter monitoring list refers to the check.
	 */
	bool isInParameterMonitoringList(const PMON& check) const {
		return std::any_of(parameterMonitoringList.begin(), parameterMonitoringList.end(),
		                   [&check](const auto& definition) { return &definition.second.get() == &check; });
	}

	/**
	 * Copies the check into a slot that no definition refers to, and adds it to the parameter monitoring list.
	 */
	template <typename Check>
	void addPMON(ParameterId PMONId, etl::ivector<Check>& checks, const Check& check) {
		if (not ASSERT_INTERNAL(not parameterMonitoringList.full(), ErrorHandler::InternalErrorType::MapFull)) {
			return;
		}
		auto* slot = checks.begin();
		while (slot != checks.end() and isInParameterMonitoringList(*slot)) {
			slot++;
		}
		if (slot == checks.end()) {
			checks.push_back(check);
			slot = &checks.back();
		} else {
			*slot = check;
		}
		parameterMonitoringList.insert({PMONId, etl::ref<PMON>(*slot)});
	}

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
	 * Adds a new Parameter Monitoring Limit Check to the parameter monitoring list.
	 */
	void addPMONLimitCheck(ParameterId PMONId, const PMONLimitCheck& limitCheck) {
		addPMON(PMONId, limitChecks, limitCheck);
	}


	/**
	 * Adds a new Parameter Monitoring Expected Value Check to the parameter monitoring list.
	 */
	void addPMONExpectedValueCheck(ParameterId PMONId, const PMONExpectedValueCheck& expectedValueCheck) {
		addPMON(PMONId, expectedValueChecks, expectedValueCheck);
	}

	/**
	 * Adds a new Parameter Monitoring Delta Check to the parameter monitoring list.
	 */
	void addPMONDeltaCheck(ParameterId PMONId, const PMONDeltaCheck& deltaCheck) {
		addPMON(PMONId, deltaChecks, deltaCheck);
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