#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include <Message.hpp>
#include "Services/OnBoardMonitoringService.hpp"
#include "etl/map.h"
#include "ServicePool.hpp"

#define firstTransitionIndex 0
#define secondTransitionIndex 1

void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, EnableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = true;
	uint16_t numberOfParameters = message.readUint16();
	uint16_t currentId = message.readEnum16();
	for (uint16_t i = 0; i < numberOfParameters; i++) {
		if (ParameterMonitoringList.find(currentId) != ParameterMonitoringList.end()) {
			RepetitionCounter.at(currentId) = 0;
			ParameterMonitoringStatus.at(currentId) = true;
		} else {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
		}
		currentId = message.readEnum16();
	}
}

void OnBoardMonitoringService::disableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DisableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = false;
	uint16_t numberOfParameters = message.readUint16();
	uint16_t currentId = message.readEnum16();
	for (uint16_t i = 0; i < numberOfParameters; i++) {
		if (ParameterMonitoringList.find(currentId) != ParameterMonitoringList.end()) {
			ParameterMonitoringStatus.at(currentId) = false;
			ParameterMonitoringCheckingStatus.at(currentId) = Unchecked;
		} else {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
		}
		currentId = message.readEnum16();
	}
}

void OnBoardMonitoringService::changeMaximumTransitionReportingDelay(Message& message) {
	message.assertTC(ServiceType, ChangeMaximumTransitionReportingDelay);
	maximumTransitionReportingDelay = message.readUint16();
}

void OnBoardMonitoringService::deleteAllParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteAllParameterMonitoringDefinitions);
	if (parameterMonitoringFunctionStatus) {
		ErrorHandler::reportError(
		    message,
		    ErrorHandler::ExecutionStartErrorType::InvalidRequestToDeleteAllParameterMonitoringDefinitionsError);
	} else {
		ParameterMonitoringList.clear();
		MonitoredParameterIds.clear();
		ParameterMonitoringCheckingStatus.clear();
		RepetitionCounter.clear();
		RepetitionNumber.clear();
		ParameterMonitoringStatus.clear();
		CheckTransitionList.clear();
		ParameterMonitoringCheckTypes.clear();
		LimitCheckParameters.clear();
		ExpectedValueCheckParameters.clear();
		DeltaCheckParameters.clear();
	}
}

void OnBoardMonitoringService::addParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, AddParameterMonitoringDefinitions);
	// TODO: Undertand error types 6.12.3.9.1.e.3-4.
	// TODO: Evaluate if the optional values in TC[12,5] are going to be used.
	uint16_t numberOfIds = message.readUint16();
	ParameterService parameterService = ParameterService();
	uint16_t currentPMONId = message.readUint16();
	uint16_t currentMonitoredParameterId = message.readUint16();
	uint16_t currentParameterRepetitionNumber = message.readUint16();
	uint16_t currentCheckType = message.readEnum8();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (ParameterMonitoringList.find(currentPMONId) != ParameterMonitoringList.end()) {
			if (ParameterMonitoringList.full()) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionStartErrorType::ParameterMonitoringListIsFull);
				break;
			} else {
				if (auto parameterToBeAdded = parameterService.getParameter(currentMonitoredParameterId)) {
					if (currentCheckType == LimitCheck) {
						// TODO: Find out how we read deduced message values.
						uint8_t lowLimit = message.readUint8();
						auto belowLowLimitEventId = static_cast<Event>(message.readEnum8());
						uint8_t highLimit = message.readUint8();
						auto aboveHighLimitEventId = static_cast<Event>(message.readEnum8());
						if (highLimit <= lowLimit) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
							break;
						}
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						MonitoredParameterIds.insert({currentPMONId, currentMonitoredParameterId});
						RepetitionCounter.insert({currentPMONId, 0});
						RepetitionNumber.insert({currentPMONId, currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({currentPMONId, false});
						ParameterMonitoringCheckTypes.insert({currentPMONId, LimitCheck});
						ParameterMonitoringCheckingStatus.insert({currentPMONId, Unchecked});
						struct LimitCheck limitCheck = {lowLimit, belowLowLimitEventId, highLimit,
						                                aboveHighLimitEventId};
						LimitCheckParameters.insert({currentPMONId, limitCheck});
					} else if (currentCheckType == ExpectedValueCheck) {
						// TODO: Find out how to read bit string.
						uint8_t mask = message.readUint8();
						uint8_t expectedValue = message.readUint8();
						auto notExpectedValueEventId = static_cast<Event>(message.readEnum8());
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						MonitoredParameterIds.insert({currentPMONId, currentMonitoredParameterId});
						RepetitionCounter.insert({currentPMONId, 0});
						RepetitionNumber.insert({currentPMONId, currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({currentPMONId, false});
						ParameterMonitoringCheckTypes.insert({currentPMONId, ExpectedValueCheck});
						ParameterMonitoringCheckingStatus.insert({currentPMONId, Unchecked});
						struct ExpectedValueCheck expectedValueCheck = {mask, expectedValue, notExpectedValueEventId};
						ExpectedValueCheckParameters.insert({currentPMONId, expectedValueCheck});
					} else if (currentCheckType == DeltaCheck) {
						uint8_t lowDeltaThreshold = message.readUint8();
						auto belowLowThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t highDeltaThreshold = message.readUint8();
						auto aboveHighThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t numberOfConsecutiveDeltaChecks = message.readUint8();
						if (highDeltaThreshold <= lowDeltaThreshold) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
							break;
						}
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						MonitoredParameterIds.insert({currentPMONId, currentMonitoredParameterId});
						RepetitionCounter.insert({currentPMONId, 0});
						RepetitionNumber.insert({currentPMONId, currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({currentPMONId, false});
						ParameterMonitoringCheckTypes.insert({currentPMONId, DeltaCheck});
						ParameterMonitoringCheckingStatus.insert({currentPMONId, Unchecked});
						struct DeltaCheck deltaCheck = {lowDeltaThreshold, belowLowThresholdEventId,
						                                aboveHighThresholdEventId, numberOfConsecutiveDeltaChecks};
						DeltaCheckParameters.insert({currentPMONId, deltaCheck});
					}
				} else {
					ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
				}
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::AddAlreadyExistingParameter);
		}
		currentPMONId = message.readUint16();
		currentMonitoredParameterId = message.readUint16();
		currentParameterRepetitionNumber = message.readUint16();
		currentCheckType = message.readEnum8();
	}
}

void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	uint16_t currentPMONId = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (ParameterMonitoringList.find(currentPMONId) == ParameterMonitoringList.end() ||
		    ParameterMonitoringStatus.at(currentPMONId)) {
			ErrorHandler::reportError(message, ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinitionError);
		} else {
			ParameterMonitoringList.erase(currentPMONId);
			MonitoredParameterIds.erase(currentPMONId);
			ParameterMonitoringCheckingStatus.erase(currentPMONId);
			RepetitionCounter.erase(currentPMONId);
			RepetitionNumber.erase(currentPMONId);
			ParameterMonitoringStatus.erase(currentPMONId);
			ParameterMonitoringCheckTypes.erase(currentPMONId);
			if (LimitCheckParameters.find(currentPMONId) != LimitCheckParameters.end()) {
				LimitCheckParameters.erase(currentPMONId);
			} else if (ExpectedValueCheckParameters.find(currentPMONId) != ExpectedValueCheckParameters.end()) {
				ExpectedValueCheckParameters.erase(currentPMONId);
			} else if (DeltaCheckParameters.find(currentPMONId) != DeltaCheckParameters.end()) {
				DeltaCheckParameters.erase(currentPMONId);
			}
		}
		currentPMONId = message.readUint16();
	}
}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	ParameterService parameterService = ParameterService();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		uint16_t currentPMONId = message.readUint16();
		uint16_t currentMonitoredParameterId = message.readUint16();
		uint16_t currentParameterRepetitionNumber = message.readUint16();
		uint16_t currentCheckType = message.readEnum8();
		if (ParameterMonitoringList.find(currentPMONId) != ParameterMonitoringList.end()) {
			if (auto parameterToBeModified = parameterService.getParameter(currentMonitoredParameterId)) {
				if (MonitoredParameterIds.at(currentPMONId) == currentMonitoredParameterId) {
					if (currentCheckType == LimitCheck) {
						uint8_t lowLimit = message.readUint8();
						auto belowLowLimitEventId = static_cast<Event>(message.readEnum8());
						uint8_t highLimit = message.readUint8();
						auto aboveHighLimitEventId = static_cast<Event>(message.readEnum8());
						if (highLimit <= lowLimit) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
							break;
						}
						RepetitionCounter.at(currentPMONId) = 0;
						ParameterMonitoringCheckingStatus.at(currentPMONId) = Unchecked;
						if (ParameterMonitoringCheckTypes.at(currentPMONId) == LimitCheck) {
							LimitCheckParameters.at(currentPMONId).lowLimit = lowLimit;
							LimitCheckParameters.at(currentPMONId).belowLowLimitEvent = belowLowLimitEventId;
							LimitCheckParameters.at(currentPMONId).highLimit = highLimit;
							LimitCheckParameters.at(currentPMONId).aboveHighLimitEvent = aboveHighLimitEventId;
						} else {
							ParameterMonitoringCheckTypes.at(currentPMONId) = LimitCheck;
							struct LimitCheck limitCheck = {lowLimit, belowLowLimitEventId, highLimit,
							                                aboveHighLimitEventId};
							LimitCheckParameters.insert({currentPMONId, limitCheck});
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == ExpectedValueCheck) {
								ExpectedValueCheckParameters.erase(currentPMONId);
							}
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == DeltaCheck) {
								DeltaCheckParameters.erase(currentPMONId);
							}
						}

					} else if (currentCheckType == ExpectedValueCheck) {
						uint8_t mask = message.readUint8();
						uint8_t expectedValue = message.readUint8();
						auto notExpectedValueEventId = static_cast<Event>(message.readEnum8());
						RepetitionCounter.at(currentPMONId) = 0;
						ParameterMonitoringCheckingStatus.at(currentPMONId) = Unchecked;
						if (ParameterMonitoringCheckTypes.at(currentPMONId) == ExpectedValueCheck) {
							ExpectedValueCheckParameters.find(currentPMONId)->second.mask = mask;
							ExpectedValueCheckParameters.find(currentPMONId)->second.expectedValue = expectedValue;
							ExpectedValueCheckParameters.find(currentPMONId)->second.notExpectedValueEvent =
							    notExpectedValueEventId;
						} else {
							ParameterMonitoringCheckTypes.at(currentPMONId) = ExpectedValueCheck;
							struct ExpectedValueCheck expectedValueCheck = {mask, expectedValue,
							                                                notExpectedValueEventId};
							ExpectedValueCheckParameters.insert({currentPMONId, expectedValueCheck});
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == LimitCheck) {
								LimitCheckParameters.erase(currentPMONId);
							}
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == DeltaCheck) {
								DeltaCheckParameters.erase(currentPMONId);
							}
						}

					} else if (currentCheckType == DeltaCheck) {
						uint8_t lowDeltaThreshold = message.readUint8();
						auto belowLowThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t highDeltaThreshold = message.readUint8();
						auto aboveHighThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t numberOfConsecutiveDeltaChecks = message.readUint8();
						if (highDeltaThreshold <= lowDeltaThreshold) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
							break;
						}
						RepetitionCounter.at(currentPMONId) = 0;
						ParameterMonitoringCheckingStatus.at(currentPMONId) = Unchecked;
						if (ParameterMonitoringCheckTypes.at(currentPMONId) == DeltaCheck) {
							DeltaCheckParameters.at(currentPMONId).lowDeltaThreshold = lowDeltaThreshold;
							DeltaCheckParameters.at(currentPMONId).belowLowThresholdEvent = belowLowThresholdEventId;
							DeltaCheckParameters.at(currentPMONId).highDeltaThreshold = highDeltaThreshold;
							DeltaCheckParameters.at(currentPMONId).aboveHighThresholdEvent = aboveHighThresholdEventId;
							DeltaCheckParameters.at(currentPMONId).numberOfConsecutiveDeltaChecks =
							    numberOfConsecutiveDeltaChecks;
						} else {
							ParameterMonitoringCheckTypes.at(currentPMONId) = DeltaCheck;
							struct DeltaCheck deltaCheck = {lowDeltaThreshold, belowLowThresholdEventId,
							                                aboveHighThresholdEventId, numberOfConsecutiveDeltaChecks};
							DeltaCheckParameters.insert({currentPMONId, deltaCheck});
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == LimitCheck) {
								LimitCheckParameters.erase(currentPMONId);
							}
							if (ParameterMonitoringCheckTypes.at(currentPMONId) == ExpectedValueCheck) {
								ExpectedValueCheckParameters.erase(currentPMONId);
							}
						}
					}
				} else {
					ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::
					                                       DifferentParameterMonitoringDefinitionAndMonitoredParameter);
				}
			} else {
				ErrorHandler::reportError(message, ErrorHandler::GetNonExistingParameter);
			}
		} else {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::ModifyParameterNotInTheParameterMonitoringList);
		}
	}
}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	parameterMonitoringDefinitionReport(message);
}

void OnBoardMonitoringService::parameterMonitoringDefinitionReport(Message& message) {
	message.assertTC(ServiceType, ParameterMonitoringDefinitionReport);
	Message parameterMonitoringDefinitionReport(ServiceType, MessageType::ParameterMonitoringDefinitionReport,
	                                            Message::TM, 0);
	// TODO: Check if maximum transition reporting delay is needed.
	parameterMonitoringDefinitionReport.appendUint16(maximumTransitionReportingDelay);
	uint16_t numberOfIds = message.readUint16();
	parameterMonitoringDefinitionReport.appendUint16(numberOfIds);
	uint16_t currentPMONId = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (ParameterMonitoringList.find(currentPMONId) != ParameterMonitoringList.end()) {
			parameterMonitoringDefinitionReport.appendEnum16(currentPMONId);
			parameterMonitoringDefinitionReport.appendEnum16(MonitoredParameterIds.at(currentPMONId));
			parameterMonitoringDefinitionReport.appendEnumerated(1, ParameterMonitoringStatus.at(currentPMONId));
			parameterMonitoringDefinitionReport.appendEnum16(RepetitionNumber.at(currentPMONId));
			parameterMonitoringDefinitionReport.appendEnum8(ParameterMonitoringCheckTypes.at(currentPMONId));
			if (ParameterMonitoringCheckTypes.at(currentPMONId) == LimitCheck) {
				parameterMonitoringDefinitionReport.appendUint16(LimitCheckParameters.at(currentPMONId).lowLimit);
				parameterMonitoringDefinitionReport.appendEnum8(
				    LimitCheckParameters.at(currentPMONId).belowLowLimitEvent);
				parameterMonitoringDefinitionReport.appendUint16(LimitCheckParameters.at(currentPMONId).highLimit);
				parameterMonitoringDefinitionReport.appendEnum8(
				    LimitCheckParameters.at(currentPMONId).aboveHighLimitEvent);
			} else if (ParameterMonitoringCheckTypes.at(currentPMONId) == ExpectedValueCheck) {
				parameterMonitoringDefinitionReport.appendUint8(ExpectedValueCheckParameters.at(currentPMONId).mask);
				parameterMonitoringDefinitionReport.appendUint16(
				    ExpectedValueCheckParameters.at(currentPMONId).expectedValue);
				parameterMonitoringDefinitionReport.appendEnum8(
				    ExpectedValueCheckParameters.at(currentPMONId).notExpectedValueEvent);
			} else {
				parameterMonitoringDefinitionReport.appendUint16(
				    DeltaCheckParameters.at(currentPMONId).lowDeltaThreshold);
				parameterMonitoringDefinitionReport.appendEnum8(
				    DeltaCheckParameters.at(currentPMONId).belowLowThresholdEvent);
				parameterMonitoringDefinitionReport.appendUint16(
				    DeltaCheckParameters.at(currentPMONId).highDeltaThreshold);
				parameterMonitoringDefinitionReport.appendEnum8(
				    DeltaCheckParameters.at(currentPMONId).aboveHighThresholdEvent);
				parameterMonitoringDefinitionReport.appendUint16(
				    DeltaCheckParameters.at(currentPMONId).numberOfConsecutiveDeltaChecks);
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ReportParameterNotInTheParameterMonitoringList);
		}

		currentPMONId = message.readUint16();
	}
	storeMessage(parameterMonitoringDefinitionReport);
}

void OnBoardMonitoringService::reportOutOfLimits(Message& message) {
	message.assertTC(ServiceType, ReportOutOfLimits);
	outOfLimitsReport();
}

void OnBoardMonitoringService::outOfLimitsReport() {
	Message outOfLimitsReport(ServiceType, MessageType::OutOfLimitsReport, Message::TM, 0);
	uint16_t numberOfTransitions = 0;
	ParameterService parameterService = ParameterService();

	for (auto& transition : CheckTransitionList) {
		if (ParameterMonitoringCheckTypes.find(transition.first)->second == ExpectedValueCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == ExpectedValue &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue)) {
				numberOfTransitions++;
			}
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == LimitCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == WithinLimits &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == AboveHighLimit &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit)) {
				numberOfTransitions++;
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowLimit &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit)) {
				numberOfTransitions++;
			}
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == DeltaCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == WithinLimits &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == AboveHighThreshold &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold)) {
				numberOfTransitions++;
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowThreshold &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold)) {
				numberOfTransitions++;
			}
		}
	}

	outOfLimitsReport.appendUint16(numberOfTransitions);

	for (auto& transition : CheckTransitionList) {
		if (ParameterMonitoringCheckTypes.find(transition.first)->second == ExpectedValueCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == ExpectedValue &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue)) {
				outOfLimitsReport.appendEnum16(transition.first);
				outOfLimitsReport.appendEnum16(MonitoredParameterIds.at(transition.first));
				outOfLimitsReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint8(ExpectedValueCheckParameters.find(transition.first)->second.mask);
				outOfLimitsReport.appendUint16(
				    parameterService.getParameter(MonitoredParameterIds.at(transition.first))
				        ->get()
				        .getValueAsDouble());
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(
				    ExpectedValueCheckParameters.find(transition.first)->second.expectedValue);
				outOfLimitsReport.appendEnum8(transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnum8(transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			}
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == LimitCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == WithinLimits &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit) ||
			    (transition.second.at(firstTransitionIndex) == AboveHighLimit &&
			     transition.second.at(secondTransitionIndex) == BelowLowLimit)) {
				outOfLimitsReport.appendEnum16(transition.first);
				outOfLimitsReport.appendEnum16(MonitoredParameterIds.at(transition.first));
				outOfLimitsReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(
				    parameterService.getParameter(MonitoredParameterIds.at(transition.first))
				        ->get()
				        .getValueAsDouble());
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(LimitCheckParameters.find(transition.first)->second.lowLimit);
				outOfLimitsReport.appendEnum8(transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnum8(transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowLimit &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit)) {
				outOfLimitsReport.appendEnum16(transition.first);
				outOfLimitsReport.appendEnum16(MonitoredParameterIds.at(transition.first));
				outOfLimitsReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(
				    parameterService.getParameter(MonitoredParameterIds.at(transition.first))
				        ->get()
				        .getValueAsDouble());
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(LimitCheckParameters.find(transition.first)->second.highLimit);
				outOfLimitsReport.appendEnum8(transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnum8(transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			}
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == DeltaCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == WithinLimits &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold) ||
			    (transition.second.at(firstTransitionIndex) == AboveHighThreshold &&
			     transition.second.at(secondTransitionIndex) == BelowLowThreshold)) {
				outOfLimitsReport.appendEnum16(transition.first);
				outOfLimitsReport.appendEnum16(MonitoredParameterIds.at(transition.first));
				outOfLimitsReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(
				    parameterService.getParameter(MonitoredParameterIds.at(transition.first))
				        ->get()
				        .getValueAsDouble());
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(DeltaCheckParameters.find(transition.first)->second.lowDeltaThreshold);
				outOfLimitsReport.appendEnum8(transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnum8(transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowThreshold &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold)) {
				outOfLimitsReport.appendEnum16(transition.first);
				outOfLimitsReport.appendEnum16(MonitoredParameterIds.at(transition.first));
				outOfLimitsReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(
				    parameterService.getParameter(MonitoredParameterIds.at(transition.first))
				        ->get()
				        .getValueAsDouble());
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(DeltaCheckParameters.find(transition.first)->second.highDeltaThreshold);
				outOfLimitsReport.appendEnum8(transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnum8(transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			}
		}
	}
	storeMessage(outOfLimitsReport);
}

void OnBoardMonitoringService::checkTransitionReport() {
	Message checkTransitionReport(ServiceType, CheckTransitionReport, Message::TM, 0);
	checkTransitionReport.appendUint16(CheckTransitionList.size());
	ParameterService parameterService = ParameterService();
	for (auto& transition : CheckTransitionList) {
		if (ParameterMonitoringCheckTypes.find(transition.first)->second == ExpectedValueCheck) {
			checkTransitionReport.appendEnum16(transition.first);
			checkTransitionReport.appendEnum16(MonitoredParameterIds.at(transition.first));
			checkTransitionReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint8(ExpectedValueCheckParameters.find(transition.first)->second.mask);
			checkTransitionReport.appendUint16(
			    parameterService.getParameter(MonitoredParameterIds.at(transition.first))->get().getValueAsDouble());
			// TODO:Evaluate if the conversion from double to uint is ok.
			// TODO: Find out what the limit crossed should be here.
			if (transition.second.at(secondTransitionIndex) == UnexpectedValue) {
				checkTransitionReport.appendUint16(
				    ExpectedValueCheckParameters.find(transition.first)->second.expectedValue);
			} else {
				checkTransitionReport.appendByte(0);
			}
			checkTransitionReport.appendEnum8(transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnum8(transition.second.at(secondTransitionIndex));
			// TODO: Find out how to get the transition time.
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == LimitCheck) {
			checkTransitionReport.appendEnum16(transition.first);
			checkTransitionReport.appendEnum16(MonitoredParameterIds.at(transition.first));
			checkTransitionReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint16(
			    parameterService.getParameter(MonitoredParameterIds.at(transition.first))->get().getValueAsDouble());
			// TODO:Evaluate if the conversion from double to uint is ok.
			// TODO: Find out what the limit crossed should be here.
			if (transition.second.at(secondTransitionIndex) == BelowLowLimit) {
				checkTransitionReport.appendUint16(LimitCheckParameters.find(transition.first)->second.lowLimit);
			} else if (transition.second.at(secondTransitionIndex) == AboveHighLimit) {
				checkTransitionReport.appendUint16(LimitCheckParameters.find(transition.first)->second.highLimit);
			} else {
				checkTransitionReport.appendByte(0);
			}
			checkTransitionReport.appendEnum8(transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnum8(transition.second.at(secondTransitionIndex));
			// TODO: Find out how to get the transition time.
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == DeltaCheck) {
			checkTransitionReport.appendEnum16(transition.first);
			checkTransitionReport.appendEnum16(MonitoredParameterIds.at(transition.first));
			checkTransitionReport.appendEnum8(ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint16(
			    parameterService.getParameter(MonitoredParameterIds.at(transition.first))->get().getValueAsDouble());
			// TODO:Evaluate if the conversion from double to uint is ok.
			// TODO: Find out what the limit crossed should be here.
			if (transition.second.at(secondTransitionIndex) == BelowLowThreshold) {
				checkTransitionReport.appendUint16(
				    DeltaCheckParameters.find(transition.first)->second.lowDeltaThreshold);
			} else if (transition.second.at(secondTransitionIndex) == AboveHighThreshold) {
				checkTransitionReport.appendUint16(
				    DeltaCheckParameters.find(transition.first)->second.highDeltaThreshold);
			} else {
				checkTransitionReport.appendByte(0);
			}
			checkTransitionReport.appendEnum8(transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnum8(transition.second.at(secondTransitionIndex));
			// TODO: Find out how to get the transition time.
		}
	}
}

void OnBoardMonitoringService::reportStatusOfParameterMonitoringDefinition(Message& message) {
	message.assertTC(ServiceType, ReportStatusOfParameterMonitoringDefinition);
	parameterMonitoringDefinitionStatusReport();
}

void OnBoardMonitoringService::parameterMonitoringDefinitionStatusReport() {
	Message parameterMonitoringDefinitionStatusReport(
	    ServiceType, MessageType::ParameterMonitoringDefinitionStatusReport, Message::TM, 0);
	parameterMonitoringDefinitionStatusReport.appendUint16(ParameterMonitoringList.size());
	for (auto& currentParameter : ParameterMonitoringList) {
		parameterMonitoringDefinitionStatusReport.appendEnum16(currentParameter.first);
		parameterMonitoringDefinitionStatusReport.appendEnumerated(
		    1, ParameterMonitoringStatus.at(currentParameter.first));
	}
	storeMessage(parameterMonitoringDefinitionStatusReport);
}

void OnBoardMonitoringService::execute(Message& message) {
	switch (message.messageType) {
		case EnableParameterMonitoringDefinitions:
			enableParameterMonitoringDefinitions(message);
			break;
		case DisableParameterMonitoringDefinitions:
			disableParameterMonitoringDefinitions(message);
			break;
		case ChangeMaximumTransitionReportingDelay:
			changeMaximumTransitionReportingDelay(message);
			break;
		case DeleteAllParameterMonitoringDefinitions:
			deleteAllParameterMonitoringDefinitions(message);
			break;
		case AddParameterMonitoringDefinitions:
			addParameterMonitoringDefinitions(message);
			break;
		case DeleteParameterMonitoringDefinitions:
			deleteParameterMonitoringDefinitions(message);
			break;
		case ModifyParameterMonitoringDefinitions:
			modifyParameterMonitoringDefinitions(message);
			break;
		case ReportParameterMonitoringDefinitions:
			reportParameterMonitoringDefinitions(message);
			break;
		case ParameterMonitoringDefinitionReport:
			parameterMonitoringDefinitionReport(message);
			break;
		case ReportOutOfLimits:
			reportOutOfLimits(message);
			break;
		case OutOfLimitsReport:
			outOfLimitsReport();
			break;
		case CheckTransitionReport:
			checkTransitionReport();
			break;
		case ReportStatusOfParameterMonitoringDefinition:
			reportStatusOfParameterMonitoringDefinition(message);
			break;
		case ParameterMonitoringDefinitionStatusReport:
			parameterMonitoringDefinitionStatusReport();
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif