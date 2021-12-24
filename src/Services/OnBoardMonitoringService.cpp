#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include <Message.hpp>
#include "Services/OnBoardMonitoringService.hpp"
#include "etl/map.h"

#define firstTransitionIndex 0
#define secondTransitionIndex 1

void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, EnableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = true;
	// TODO: Evaluate if more error reports are necessary
	uint16_t numOfParameters = systemParameters.parametersArray.size();
	for (uint16_t i = 0; i < numOfParameters; i++) {
		uint16_t currentId = message.readUint16();
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			return;
		}
		// TODO: Examine why merging the following two if statements in 1, does not work.
		if (auto currentParameter = systemParameters.getParameter(currentId)) {
			if (ParameterMonitoringList.find(currentId) == ParameterMonitoringList.end()) {
				RepetitionNumber.find(currentParameter->get())->second = 0;
				ParameterMonitoringStatus.find(currentParameter->get())->second = true;
			} else {
				ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
		}
	}
}

void OnBoardMonitoringService::disableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DisableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = false;
	// TODO: Evaluate if more error reports are necessary
	uint16_t numOfParameters = systemParameters.parametersArray.size();
	for (uint16_t i = 0; i < numOfParameters; i++) {
		uint16_t currentId = message.readUint16();
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			return;
		}
		if (auto currentParameter = systemParameters.getParameter(currentId)) {
			if (ParameterMonitoringList.find(currentId) != ParameterMonitoringList.end()) {
				ParameterMonitoringStatus.find(currentParameter->get())->second = false;
				ParameterMonitoringCheckingStatus.find(currentParameter->get())->second = Unchecked;
			} else {
				ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
		}
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
		CheckTransitionList.clear();
	}
}

void OnBoardMonitoringService::addParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, AddParameterMonitoringDefinitions);
	// TODO: Undertand error types 6.12.3.9.1.e.3-4.
	// TODO: Evaluate if the optional values in TC[12,5] are going to be used.
	uint16_t numberOfIds = message.readUint16();
	// TODO: Move the initialisation of all values out of the loop.
	for (uint16_t i = 0; i < numberOfIds; i++) {
		uint16_t currentPMONId = message.readUint16();
		uint16_t currentParameterId = message.readUint16();
		uint16_t currentParameterRepetitionNumber = message.readUint16();
		// TODO: Find out how to handle enumerated values.
		uint16_t currentCheckType = message.readEnum8();
		if (ParameterMonitoringList.find(currentPMONId) == ParameterMonitoringList.end()) {
			if (ParameterMonitoringList.full()) {
				ErrorHandler::reportError(message,
				                          ErrorHandler::ExecutionStartErrorType::ParameterMonitoringListIsFull);
			} else {
				if (auto parameterToBeAdded = systemParameters.getParameter(currentParameterId)) {
					if (currentCheckType == LimitCheck) {
						// TODO: Find out how we read deduced message values.
						uint8_t lowLimit = message.readUint8();
						// TODO: Evaluate if the below cast is right.
						auto belowLowLimitEventId = static_cast<Event>(message.readEnum8());
						uint8_t highLimit = message.readUint8();
						auto aboveHighLimitEventId = static_cast<Event>(message.readEnum8());
						if (highLimit <= lowLimit) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
							return;
						}
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						ParameterMonitoringIds.insert({parameterToBeAdded->get(), currentPMONId});
						RepetitionCounter.insert({parameterToBeAdded->get(), 0});
						RepetitionNumber.insert({parameterToBeAdded->get(), currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({parameterToBeAdded->get(), false});
						ParameterMonitoringCheckTypes.insert({parameterToBeAdded->get(), LimitCheck});
						ParameterMonitoringCheckingStatus.insert({parameterToBeAdded->get(), Unchecked});
						struct LimitCheck limitCheck = {lowLimit, belowLowLimitEventId, highLimit,
						                                aboveHighLimitEventId};
						LimitCheckParameters.insert({parameterToBeAdded->get(), limitCheck});
					} else if (currentCheckType == ExpectedValueCheck) {
						uint8_t mask = message.readUint8();
						uint8_t expectedValue = message.readUint8();
						auto notExpectedValueEventId = static_cast<Event>(message.readEnum8());
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						ParameterMonitoringIds.insert({parameterToBeAdded->get(), currentPMONId});
						RepetitionCounter.insert({parameterToBeAdded->get(), 0});
						RepetitionNumber.insert({parameterToBeAdded->get(), currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({parameterToBeAdded->get(), false});
						ParameterMonitoringCheckTypes.insert({parameterToBeAdded->get(), ExpectedValueCheck});
						ParameterMonitoringCheckingStatus.insert({parameterToBeAdded->get(), Unchecked});
						struct ExpectedValueCheck expectedValueCheck = {mask, expectedValue, notExpectedValueEventId};
						ExpectedValueCheckParameters.insert({parameterToBeAdded->get(), expectedValueCheck});
					} else if (currentCheckType == DeltaCheck) {
						uint8_t lowDeltaThreshold = message.readUint8();
						auto belowLowThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t highDeltaThreshold = message.readUint8();
						auto aboveHighThresholdEventId = static_cast<Event>(message.readEnum8());
						uint8_t numberOfConsecutiveDeltaChecks = message.readUint8();
						if (highDeltaThreshold <= lowDeltaThreshold) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
							return;
						}
						ParameterMonitoringList.insert({currentPMONId, parameterToBeAdded->get()});
						ParameterMonitoringIds.insert({parameterToBeAdded->get(), currentPMONId});
						RepetitionCounter.insert({parameterToBeAdded->get(), 0});
						RepetitionNumber.insert({parameterToBeAdded->get(), currentParameterRepetitionNumber});
						ParameterMonitoringStatus.insert({parameterToBeAdded->get(), false});
						ParameterMonitoringCheckTypes.insert({parameterToBeAdded->get(), DeltaCheck});
						ParameterMonitoringCheckingStatus.insert({parameterToBeAdded->get(), Unchecked});
						struct DeltaCheck deltaCheck = {lowDeltaThreshold, belowLowThresholdEventId,
						                                aboveHighThresholdEventId, numberOfConsecutiveDeltaChecks};
						DeltaCheckParameters.insert({parameterToBeAdded->get(), deltaCheck});
					}
				} else {
					ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
				}
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::AddAlreadyExistingParameter);
		}
	}
}

void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	uint16_t currentParameterId = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (ParameterMonitoringList.find(currentParameterId) == ParameterMonitoringList.end() ||
		    ParameterMonitoringStatus.at(systemParameters.getParameter(currentParameterId)->get())) {
			ErrorHandler::reportError(message, ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinitionError);
		} else {
			ParameterMonitoringList.erase(currentParameterId);
		}
		currentParameterId = message.readUint16();
	}
}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		uint16_t currentPMONId = message.readUint16();
		uint16_t currentParameterId = message.readUint16();
		uint16_t currentParameterRepetitionNumber = message.readUint16();
		uint16_t currentCheckType = message.readEnum8();
		if (ParameterMonitoringList.find(currentPMONId) != ParameterMonitoringList.end()) {
			if (auto parameterToBeAdded = systemParameters.getParameter(currentParameterId)) {
				// TODO: Find out how to compare the items below.
				if (static_cast<std::reference_wrapper<ParameterBase>>(parameterToBeAdded->get()) !=
				    ParameterMonitoringList.find(currentPMONId)->second) {
					if (currentCheckType == LimitCheck) {
						uint8_t lowLimit = message.readUint8();
						auto belowLowLimitEventId = static_cast<Event>(message.readEnum8());
						uint8_t highLimit = message.readUint8();
						auto aboveHighLimitEventId = static_cast<Event>(message.readEnum8());
						if (highLimit <= lowLimit) {
							ErrorHandler::reportError(
							    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
							return;
						}
						RepetitionCounter.find(ParameterMonitoringList.at(currentPMONId))->second = 0;
						if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
						    LimitCheck) {
							LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.lowLimit =
							    lowLimit;
							LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.belowLowLimitEvent = belowLowLimitEventId;
							LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.highLimit =
							    highLimit;
							LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.aboveHighLimitEvent = aboveHighLimitEventId;
						} else {
							ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second =
							    LimitCheck;
							struct LimitCheck limitCheck = {lowLimit, belowLowLimitEventId, highLimit,
							                                aboveHighLimitEventId};
							LimitCheckParameters.insert({parameterToBeAdded->get(), limitCheck});
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    ExpectedValueCheck) {
								ExpectedValueCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
							}
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    DeltaCheck) {
								DeltaCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
							}
						}

					} else if (currentCheckType == ExpectedValueCheck) {
						uint8_t mask = message.readUint8();
						uint8_t expectedValue = message.readUint8();
						auto notExpectedValueEventId = static_cast<Event>(message.readEnum8());
						RepetitionCounter.find(ParameterMonitoringList.at(currentPMONId))->second = 0;
						if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
						    ExpectedValueCheck) {
							ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.mask =
							    mask;
							ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.expectedValue = expectedValue;
							ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.notExpectedValueEvent = notExpectedValueEventId;
						} else {
							ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second =
							    ExpectedValueCheck;
							struct ExpectedValueCheck expectedValueCheck = {mask, expectedValue,
							                                                notExpectedValueEventId};
							ExpectedValueCheckParameters.insert({parameterToBeAdded->get(), expectedValueCheck});
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    LimitCheck) {
								LimitCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
							}
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    DeltaCheck) {
								DeltaCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
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
							return;
						}
						RepetitionCounter.find(ParameterMonitoringList.at(currentPMONId))->second = 0;
						if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
						    DeltaCheck) {
							DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.lowDeltaThreshold = lowDeltaThreshold;
							DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.belowLowThresholdEvent = belowLowThresholdEventId;
							DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.highDeltaThreshold = highDeltaThreshold;
							DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.aboveHighThresholdEvent = aboveHighThresholdEventId;
							DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
							    ->second.numberOfConsecutiveDeltaChecks = numberOfConsecutiveDeltaChecks;
						} else {
							ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second =
							    DeltaCheck;
							struct DeltaCheck deltaCheck = {lowDeltaThreshold, belowLowThresholdEventId,
							                                aboveHighThresholdEventId, numberOfConsecutiveDeltaChecks};
							DeltaCheckParameters.insert({parameterToBeAdded->get(), deltaCheck});
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    LimitCheck) {
								LimitCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
							}
							if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
							    ExpectedValueCheck) {
								ExpectedValueCheckParameters.erase(ParameterMonitoringList.at(currentPMONId));
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
		parameterMonitoringDefinitionReport.appendEnumerated(16, currentPMONId);
		// TODO:Find out how to get the monitored parameter id.
		parameterMonitoringDefinitionReport.appendEnumerated(
		    1, ParameterMonitoringStatus.find(ParameterMonitoringList.at(currentPMONId))->second);
		parameterMonitoringDefinitionReport.appendEnumerated(
		    16, RepetitionNumber.find(ParameterMonitoringList.at(currentPMONId))->second);
		parameterMonitoringDefinitionReport.appendEnumerated(
		    8, ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second);
		if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second == LimitCheck) {
			parameterMonitoringDefinitionReport.appendUint16(
			    LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.lowLimit);
			parameterMonitoringDefinitionReport.appendEnumerated(
			    8, LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.belowLowLimitEvent);
			parameterMonitoringDefinitionReport.appendUint16(
			    LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.highLimit);
			parameterMonitoringDefinitionReport.appendEnumerated(
			    8, LimitCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.aboveHighLimitEvent);
		} else if (ParameterMonitoringCheckTypes.find(ParameterMonitoringList.at(currentPMONId))->second ==
		           ExpectedValueCheck) {
			parameterMonitoringDefinitionReport.appendUint8(
			    ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.mask);
			parameterMonitoringDefinitionReport.appendUint16(
			    ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.expectedValue);
			parameterMonitoringDefinitionReport.appendEnumerated(
			    8, ExpectedValueCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
			           ->second.notExpectedValueEvent);
		} else {
			parameterMonitoringDefinitionReport.appendUint16(
			    DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.lowDeltaThreshold);
			parameterMonitoringDefinitionReport.appendEnumerated(
			    8, DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.belowLowThresholdEvent);
			parameterMonitoringDefinitionReport.appendUint16(
			    DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.highDeltaThreshold);
			parameterMonitoringDefinitionReport.appendEnumerated(
			    8,
			    DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))->second.aboveHighThresholdEvent);
			parameterMonitoringDefinitionReport.appendUint16(
			    DeltaCheckParameters.find(ParameterMonitoringList.at(currentPMONId))
			        ->second.numberOfConsecutiveDeltaChecks);
		}
	}
	storeMessage(parameterMonitoringDefinitionReport);
}

void OnBoardMonitoringService::reportOutOfLimits(Message& message) {
	message.assertTC(ServiceType, ReportOutOfLimits);
	outOfLimitsReport();
}

void OnBoardMonitoringService::outOfLimitsReport() {
	Message outOfLimitsReport(ServiceType, MessageType::OutOfLimitsReport, Message::TM, 0);
	// TODO: Find a way to calculate the transitions to be reported before getting in the loop.
	outOfLimitsReport.appendUint16(CheckTransitionList.size());
	for (auto& transition : CheckTransitionList) {
		if (ParameterMonitoringCheckTypes.find(transition.first)->second == ExpectedValueCheck) {
			if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == Invalid &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue) ||
			    (transition.second.at(firstTransitionIndex) == ExpectedValue &&
			     transition.second.at(secondTransitionIndex) == UnexpectedValue)) {
				outOfLimitsReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
				// TODO:Find out how to get the monitored parameter id.
				outOfLimitsReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint8(ExpectedValueCheckParameters.find(transition.first)->second.mask);
				outOfLimitsReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
				// TODO:Replace 1 with the monitored parameter id.
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(
				    ExpectedValueCheckParameters.find(transition.first)->second.expectedValue);
				outOfLimitsReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
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
				outOfLimitsReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
				// TODO:Find out how to get the monitored parameter id.
				outOfLimitsReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
				// TODO:Replace 1 with the monitored parameter id.
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(LimitCheckParameters.find(transition.first)->second.lowLimit);
				outOfLimitsReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowLimit &&
			            transition.second.at(secondTransitionIndex) == AboveHighLimit)) {
				outOfLimitsReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
				// TODO:Find out how to get the monitored parameter id.
				outOfLimitsReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
				// TODO:Replace 1 with the monitored parameter id.
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(LimitCheckParameters.find(transition.first)->second.highLimit);
				outOfLimitsReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
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
				outOfLimitsReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
				// TODO:Find out how to get the monitored parameter id.
				outOfLimitsReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
				// TODO:Replace 1 with the monitored parameter id.
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(DeltaCheckParameters.find(transition.first)->second.lowDeltaThreshold);
				outOfLimitsReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			} else if ((transition.second.at(firstTransitionIndex) == Unchecked &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == Invalid &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == WithinLimits &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold) ||
			           (transition.second.at(firstTransitionIndex) == BelowLowThreshold &&
			            transition.second.at(secondTransitionIndex) == AboveHighThreshold)) {
				outOfLimitsReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
				// TODO:Find out how to get the monitored parameter id.
				outOfLimitsReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
				outOfLimitsReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
				// TODO:Replace 1 with the monitored parameter id.
				// TODO:Evaluate if the conversion from double to uint is ok.
				outOfLimitsReport.appendUint16(DeltaCheckParameters.find(transition.first)->second.highDeltaThreshold);
				outOfLimitsReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
				outOfLimitsReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
				// TODO: Find out how to get the transition time.
			}
		}
	}
	storeMessage(outOfLimitsReport);
}

void OnBoardMonitoringService::checkTransitionReport() {
	Message checkTransitionReport(ServiceType, CheckTransitionReport, Message::TM, 0);
	checkTransitionReport.appendUint16(CheckTransitionList.size());
	for (auto& transition : CheckTransitionList) {
		if (ParameterMonitoringCheckTypes.find(transition.first)->second == ExpectedValueCheck) {
			checkTransitionReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
			// TODO:Find out how to get the monitored parameter id.
			checkTransitionReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint8(ExpectedValueCheckParameters.find(transition.first)->second.mask);
			checkTransitionReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
			// TODO:Replace 1 with the monitored parameter id.
			// TODO:Evaluate if the conversion from double to uint is ok.
			// TODO: Find out what the limit crossed should be here.
			if (transition.second.at(secondTransitionIndex) == UnexpectedValue) {
				checkTransitionReport.appendUint16(
				    ExpectedValueCheckParameters.find(transition.first)->second.expectedValue);
			} else {
				checkTransitionReport.appendByte(0);
			}
			checkTransitionReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
			// TODO: Find out how to get the transition time.
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == LimitCheck) {
			checkTransitionReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
			// TODO:Find out how to get the monitored parameter id.
			checkTransitionReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
			// TODO:Replace 1 with the monitored parameter id.
			// TODO:Evaluate if the conversion from double to uint is ok.
			// TODO: Find out what the limit crossed should be here.
			if (transition.second.at(secondTransitionIndex) == BelowLowLimit) {
				checkTransitionReport.appendUint16(LimitCheckParameters.find(transition.first)->second.lowLimit);
			} else if (transition.second.at(secondTransitionIndex) == AboveHighLimit) {
				checkTransitionReport.appendUint16(LimitCheckParameters.find(transition.first)->second.highLimit);
			} else {
				checkTransitionReport.appendByte(0);
			}
			checkTransitionReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
			// TODO: Find out how to get the transition time.
		} else if (ParameterMonitoringCheckTypes.find(transition.first)->second == DeltaCheck) {
			checkTransitionReport.appendEnumerated(16, ParameterMonitoringIds.find(transition.first)->second);
			// TODO:Find out how to get the monitored parameter id.
			checkTransitionReport.appendEnumerated(8, ParameterMonitoringCheckTypes.find(transition.first)->second);
			checkTransitionReport.appendUint16(systemParameters.getParameter(1)->get().getValueAsDouble());
			// TODO:Replace 1 with the monitored parameter id.
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
			checkTransitionReport.appendEnumerated(8, transition.second.at(firstTransitionIndex));
			checkTransitionReport.appendEnumerated(8, transition.second.at(secondTransitionIndex));
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
		parameterMonitoringDefinitionStatusReport.appendEnumerated(16, currentParameter.first);
		parameterMonitoringDefinitionStatusReport.appendEnumerated(
		    1, ParameterMonitoringStatus.at(currentParameter.second));
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