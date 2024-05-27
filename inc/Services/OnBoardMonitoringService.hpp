#ifndef ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
#define ECSS_SERVICES_ONBOARDMONITORINGSERVICE_HPP
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
 * @author Konstantinos Michopoulos <konstantinos.michopoulos@gmail.com> and Thomas Pravinos <tompravi99@gmail.com>
 */
class OnBoardMonitoringService : public Service {
private:
	/**
	 * Map storing the parameter monitoring definitions.
	 */
	etl::map<uint16_t, etl::reference_wrapper<PMON>, ECSSMaxMonitoringDefinitions> parameterMonitoringList;

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
	void addPMONLimitCheck(ParameterId PMONId, PMONLimitCheck& limitCheck) {
		limitChecks.push_back(limitCheck);
		parameterMonitoringList.insert(etl::pair<const ParameterId, etl::reference_wrapper<PMON>>(PMONId, etl::ref(limitChecks.back())));
	}


	/**
	 * Adds a new Parameter Monitoring Expected Value Check to the parameter monitoring list.
	 */
	void addPMONExpectedValueCheck(ParameterId PMONId, PMONExpectedValueCheck& expectedValueCheck) {
		expectedValueChecks.push_back(expectedValueCheck);
		parameterMonitoringList.insert(etl::pair<const ParameterId, etl::reference_wrapper<PMON>>(PMONId, etl::ref(expectedValueChecks.back())));
	}

	/**
	 * Adds a new Parameter Monitoring Delta Check to the parameter monitoring list.
	 */
	void addPMONDeltaCheck(ParameterId PMONId, PMONDeltaCheck& deltaCheck) {
		deltaChecks.push_back(deltaCheck);
		parameterMonitoringList.insert(etl::pair<const ParameterId, etl::reference_wrapper<PMON>>(PMONId, etl::ref(deltaChecks.back())));
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
     * Helper function to get the previous PMON with the same check type.
 	 */
	etl::optional<etl::reference_wrapper<PMON>> getPreviousPMON(ParameterId PMONId, PMON::CheckType checkType) const {
		for (auto reverseIt = parameterMonitoringList.crbegin(); reverseIt != parameterMonitoringList.crend(); ++reverseIt) {
			if (reverseIt->second.get().monitoredParameterId == PMONId && reverseIt->second.get().checkType == checkType) {
				return reverseIt->second;
			}
		}

		return {};
	}

	/**
	 * This function performs a check on the provided PMON object. The type of check performed is determined by the checkType member of the PMON object.
	 * The function updates the checkingStatus member of the PMON object based on the result of the check.
	 *
	 * This function is expected to be called by the periodic monitoring system, which is responsible for
	 * invoking the check on each monitored parameter at regular intervals. It ensures that parameters are
	 * within their defined limits, match expected values, or have acceptable delta changes over time.
	 *
	 * @note
	 * It is crucial that this function is called periodically and consistently to ensure the reliability of the monitoring system.
	 * Irregular calls or missed checks can lead to incorrect status updates and potentially missed parameter anomalies.
	 *
	 * @param pmon A reference to the PMON object to be checked.
	 */
	void performCheck(PMON& pmon) const {
		auto currentValue = pmon.monitoredParameter.get().getValueAsDouble();
		PMON::CheckingStatus previousStatus = pmon.checkingStatus;

		switch (pmon.checkType) {
			case PMON::CheckType::Limit: {
				auto& limitCheck = static_cast<PMONLimitCheck&>(pmon);
				if (currentValue < limitCheck.getLowLimit()) {
					pmon.checkingStatus = PMON::CheckingStatus::BelowLowLimit;
				} else if (currentValue > limitCheck.getHighLimit()) {
					pmon.checkingStatus = PMON::CheckingStatus::AboveHighLimit;
				} else {
					pmon.checkingStatus = PMON::CheckingStatus::WithinLimits;
				}
				break;
			}
			case PMON::CheckType::ExpectedValue: {
				auto& expectedValueCheck = static_cast<PMONExpectedValueCheck&>(pmon);
				uint64_t currentValueAsUint64 = pmon.monitoredParameter.get().getValueAsUint64();
				uint64_t maskedValue = currentValueAsUint64 & expectedValueCheck.getMask();
				if (maskedValue == expectedValueCheck.getExpectedValue()) {
					pmon.checkingStatus = PMON::CheckingStatus::ExpectedValue;
				} else {
					pmon.checkingStatus = PMON::CheckingStatus::UnexpectedValue;
				}
				break;
			}
			case PMON::CheckType::Delta: {
				auto& deltaCheck = static_cast<PMONDeltaCheck&>(pmon);
				if (deltaCheck.isPreviousTimestampValid()) {
					double deltaPerSecond = deltaCheck.getDeltaPerSecond();
					if (deltaPerSecond < deltaCheck.getLowDeltaThreshold()) {
						pmon.checkingStatus = PMON::CheckingStatus::BelowLowThreshold;
					} else if (deltaPerSecond > deltaCheck.getHighDeltaThreshold()) {
						pmon.checkingStatus = PMON::CheckingStatus::AboveHighThreshold;
					} else {
						pmon.checkingStatus = PMON::CheckingStatus::WithinThreshold;
					}
				} else {
					pmon.checkingStatus = PMON::CheckingStatus::Invalid;
				}

				deltaCheck.updateValuesAndTimestamps(currentValue);
				break;
			}
		}

		if (pmon.checkingStatus == previousStatus) {
			pmon.repetitionCounter++;
		} else {
			pmon.repetitionCounter = 1;
		}
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