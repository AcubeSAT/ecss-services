#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include "Message.hpp"
#include "ServicePool.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include "etl/map.h"
#include "etl/vector.h"

void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, EnableParameterMonitoringDefinitions)) {
		return;
	}

	uint16_t numberOfPMONDefinitions = message.readUint16();
	for (uint16_t i = 0; i < numberOfPMONDefinitions; i++) {
		PMONId currentId = message.read<PMONId>();
		auto definition = parameterMonitoringList.find(currentId);
		if (definition == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
			continue;
		}
		definition->second.get().repetitionNumber = 0;
		definition->second.get().monitoringEnabled = true;
	}
}

void OnBoardMonitoringService::disableParameterMonitoringDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, DisableParameterMonitoringDefinitions)) {
		return;
	}

	uint16_t numberOfPMONDefinitions = message.readUint16();
	for (uint16_t i = 0; i < numberOfPMONDefinitions; i++) {
		PMONId currentId = message.read<PMONId>();
		auto definition = parameterMonitoringList.find(currentId);
		if (definition == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
			continue;
		}
		definition->second.get().monitoringEnabled = false;
		definition->second.get().checkingStatus = PMONBase::Unchecked;
	}
}

void OnBoardMonitoringService::changeMaximumTransitionReportingDelay(Message& message) {
	if (!message.assertTC(ServiceType, ChangeMaximumTransitionReportingDelay)) {
		return;
	}
	maximumTransitionReportingDelay = message.readUint16();
}

void OnBoardMonitoringService::deleteAllParameterMonitoringDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, DeleteAllParameterMonitoringDefinitions)) {
		return;
	}
	if (parameterMonitoringFunctionStatus) {
		ErrorHandler::reportError(
		    message,
		    ErrorHandler::ExecutionStartErrorType::InvalidRequestToDeleteAllParameterMonitoringDefinitions);
		return;
	}
	parameterMonitoringList.clear();
}

void OnBoardMonitoringService::addParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, AddParameterMonitoringDefinitions);

	uint16_t numberOfIds = message.readUint16();
	uint16_t currentPMONId = message.readEnum16();
	uint16_t currentMonitoredParameterId = message.readEnum16();
	uint16_t currentPMONRepetitionNumber = message.readUint16();
	uint16_t currentCheckType = message.readEnum8();

	// Create vectors for each check type
	etl::vector<PMONLimitCheck, 12> limitChecks; // 12 for now
	etl::vector<PMONExpectedValueCheck, 12> expectedValueChecks;
	etl::vector<PMONDeltaCheck, 12> deltaChecks;

	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (parameterMonitoringList.find(currentPMONId) != parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::AddAlreadyExistingParameter);
			continue;
		}

		if (parameterMonitoringList.full()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ParameterMonitoringListIsFull);
			continue;
		}

		if (auto parameterToBeAdded = Services.parameterManagement.getParameter(currentMonitoredParameterId)) {
			if (currentCheckType == PMONBase::LimitCheck) {
				double lowLimit = message.readDouble();
				uint16_t belowLowLimitEventId = message.readEnum16();
				double highLimit = message.readDouble();
				uint16_t aboveHighLimitEventId = message.readEnum16();
				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}
				PMONLimitCheck limitCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                          lowLimit, belowLowLimitEventId, highLimit, aboveHighLimitEventId);
				limitChecks.push_back(limitCheck);
				addPMONDefinition(currentPMONId, limitCheck);
			} else if (currentCheckType == PMONBase::ExpectedValueCheck) {
				uint64_t mask = message.readUint64();
				double expectedValue = message.readDouble();
				uint16_t unExpectedValueEvent = message.readEnum16();
				PMONExpectedValueCheck expectedValueCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                                          expectedValue, mask, unExpectedValueEvent);
				expectedValueChecks.push_back(expectedValueCheck);
				addPMONDefinition(currentPMONId, expectedValueCheck);
			} else if (currentCheckType == PMONBase::DeltaCheck) {
				double lowDeltaThreshold = message.readDouble();
				uint16_t belowLowThresholdEventId = message.readEnum16();
				double highDeltaThreshold = message.readDouble();
				uint16_t aboveHighThresholdEventId = message.readEnum16();
				uint16_t numberOfConsecutiveDeltaChecks = message.readUint16();
				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}
				PMONDeltaCheck deltaCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                          numberOfConsecutiveDeltaChecks, lowDeltaThreshold, belowLowThresholdEventId, highDeltaThreshold, aboveHighThresholdEventId);
				deltaChecks.push_back(deltaCheck);
				addPMONDefinition(currentPMONId, deltaCheck);
			}
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
		}

		currentPMONId = message.readEnum16();
		currentMonitoredParameterId = message.readEnum16();
		currentPMONRepetitionNumber = message.readUint16();
		currentCheckType = message.readEnum8();
	}
}


void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	uint16_t currentPMONId = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end() ||
		    getPMONDefinition(currentPMONId).get().monitoringEnabled) {
			ErrorHandler::reportError(message, ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinition);
			currentPMONId = message.readUint16();
			continue;
		}
		parameterMonitoringList.erase(currentPMONId);
		currentPMONId = message.readUint16();
	}
}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	uint16_t currentPMONId = message.readEnum16();
	uint16_t currentMonitoredParameterId = message.readEnum16();
	uint16_t currentPMONRepetitionNumber = message.readUint16();
	uint16_t currentCheckType = message.readEnum8();

	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::ModifyParameterNotInTheParameterMonitoringList);
			continue;
		}
		//TODO: Implement a parameterExists() function.
		if (auto parameterToBeModified = Services.parameterManagement.getParameter(currentMonitoredParameterId)) {
			if (getPMONDefinition(currentPMONId).get().monitoredParameterId != currentMonitoredParameterId) {
				ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::
				                                       DifferentParameterMonitoringDefinitionAndMonitoredParameter);
				continue;
			}
			if (currentCheckType == PMONBase::LimitCheck) {
				double lowLimit = message.readDouble();
				uint16_t belowLowLimitEventId = message.readEnum16();
				double highLimit = message.readDouble();
				uint16_t aboveHighLimitEventId = message.readEnum16();
				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}
				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONLimitCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, lowLimit, belowLowLimitEventId,
				                                           highLimit, aboveHighLimitEventId);
				addPMONDefinition(currentPMONId, monitoringDefinition);
			} else if (currentCheckType == PMONBase::ExpectedValueCheck) {
				uint64_t mask = message.readUint64();
				double expectedValue = message.readDouble();
				uint16_t unExpectedValueEvent = message.readEnum16();
				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONExpectedValueCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, expectedValue,
				                                                   mask, unExpectedValueEvent);
				addPMONDefinition(currentPMONId, monitoringDefinition);
			} else if (currentCheckType == PMONBase::DeltaCheck) {
				double lowDeltaThreshold = message.readDouble();
				uint16_t belowLowThresholdEventId = message.readEnum16();
				double highDeltaThreshold = message.readDouble();
				uint16_t aboveHighThresholdEventId = message.readEnum16();
				uint16_t numberOfConsecutiveDeltaChecks = message.readUint16();
				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}
				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONDeltaCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, numberOfConsecutiveDeltaChecks,
				                                           lowDeltaThreshold, belowLowThresholdEventId, highDeltaThreshold, aboveHighThresholdEventId);
				addPMONDefinition(currentPMONId, monitoringDefinition);
			}

		} else {
			ErrorHandler::reportError(message, ErrorHandler::GetNonExistingParameter);
			continue;
		}
		currentPMONId = message.readEnum16();
		currentMonitoredParameterId = message.readEnum16();
		currentPMONRepetitionNumber = message.readUint16();
		currentCheckType = message.readEnum8();
	}
}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	parameterMonitoringDefinitionReport(message);
}

void OnBoardMonitoringService::parameterMonitoringDefinitionReport(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	Message parameterMonitoringDefinitionReport(ServiceType, MessageType::ParameterMonitoringDefinitionReport,
	                                            Message::TM, 0);
	// TODO: Check if maximum transition reporting delay is needed.
	parameterMonitoringDefinitionReport.appendUint16(maximumTransitionReportingDelay);
	uint16_t numberOfIds = message.readUint16();
	parameterMonitoringDefinitionReport.appendUint16(numberOfIds);
	for (uint16_t i = 0; i < numberOfIds; i++) {
		auto currentPMONId = message.readEnum16();
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ReportParameterNotInTheParameterMonitoringList);
			continue;
		}
		parameterMonitoringDefinitionReport.appendEnum16(currentPMONId);
		parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().monitoredParameterId);
//		parameterMonitoringDefinitionReport.appendEnum8(getPMONDefinition(currentPMONId).get().monitoringEnabled);
		parameterMonitoringDefinitionReport.appendEnum8(getPMONDefinition(currentPMONId).get().monitoringEnabled ? 1 : 0);
		parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().repetitionNumber);
		parameterMonitoringDefinitionReport.appendEnum8(getPMONDefinition(currentPMONId).get().checkType);
		if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::LimitCheck) {
			parameterMonitoringDefinitionReport.appendDouble(getPMONDefinition(currentPMONId).get().getLowLimit());
			parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().getBelowLowLimitEvent());
			parameterMonitoringDefinitionReport.appendDouble(getPMONDefinition(currentPMONId).get().getHighLimit());
			parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().getAboveHighLimitEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::ExpectedValueCheck) {
			parameterMonitoringDefinitionReport.appendUint64(getPMONDefinition(currentPMONId).get().getMask());
			parameterMonitoringDefinitionReport.appendDouble(
			    getPMONDefinition(currentPMONId).get().getExpectedValue());
			parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().getUnexpectedValueEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::DeltaCheck) {
			parameterMonitoringDefinitionReport.appendDouble(getPMONDefinition(currentPMONId).get().getLowDeltaThreshold());
			parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().getBelowLowThresholdEvent());
			parameterMonitoringDefinitionReport.appendDouble(getPMONDefinition(currentPMONId).get().getHighDeltaThreshold());
			parameterMonitoringDefinitionReport.appendEnum16(getPMONDefinition(currentPMONId).get().getAboveHighThresholdEvent());
			parameterMonitoringDefinitionReport.appendUint16(getPMONDefinition(currentPMONId).get().getNumberOfConsecutiveDeltaChecks());
		}
	}
	storeMessage(parameterMonitoringDefinitionReport);
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
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif