#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include <Message.hpp>
#include "Services/OnBoardMonitoringService.hpp"
#include "etl/map.h"

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
				RepetitionCounter.find(currentParameter->get())->second = 0;
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
}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
}

void OnBoardMonitoringService::parameterMonitoringDefinitionReport(Message& message) {
	message.assertTC(ServiceType, ParameterMonitoringDefinitionReport);
}

void OnBoardMonitoringService::reportOutOfLimits(Message& message) {
	message.assertTC(ServiceType, ReportOutOfLimits);
}

void OnBoardMonitoringService::outOfLimitsReport() {}

void OnBoardMonitoringService::checkTransitionReport() {}

void OnBoardMonitoringService::reportStatusOfParameterMonitoringDefinition(Message& message) {
	message.assertTC(ServiceType, ReportStatusOfParameterMonitoringDefinition);
	parameterMonitoringDefinitionStatusReport();
}

void OnBoardMonitoringService::parameterMonitoringDefinitionStatusReport() {
	// TODO: Check if application id must be 1
	Message parameterMonitoringDefinitionStatusReport(
	    ServiceType, MessageType::ParameterMonitoringDefinitionStatusReport, Message::TM, 0);
	parameterMonitoringDefinitionStatusReport.appendUint16(ParameterMonitoringList.size());
	for (auto& currentParameter : ParameterMonitoringList) {
		parameterMonitoringDefinitionStatusReport.appendEnumerated(16, currentParameter.first);
		parameterMonitoringDefinitionStatusReport.appendEnumerated(
		    16, ParameterMonitoringStatus.at(currentParameter.second));
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