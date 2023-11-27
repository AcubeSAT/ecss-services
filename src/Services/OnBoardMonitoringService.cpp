#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include "Message.hpp"
#include "ServicePool.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include "ECSS_Definitions.hpp"
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

	etl::vector<PMONLimitCheck, MaximumNumberOfChecksLimitCheck> limitChecks;
	etl::vector<PMONExpectedValueCheck, MaximumNumberOfChecksExpectedValueCheck> expectedValueChecks;
	etl::vector<PMONDeltaCheck, MaximumNumberOfChecksDeltaCheck> deltaChecks;

	for (uint16_t i = 0; i < numberOfIds; i++) {

		PMONId currentPMONId = message.read<PMONId>();
		MonitoredParameterId currentMonitoredParameterId = message.read<MonitoredParameterId>();
		RepetitionNumber currentPMONRepetitionNumber = message.read<RepetitionNumber>();
		uint16_t currentCheckType = message.readEnum8();

		auto parameterToBeAdded = Services.parameterManagement.getParameter(currentMonitoredParameterId);
		if (!parameterToBeAdded) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}

		if (parameterMonitoringList.find(currentPMONId) != parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::AddAlreadyExistingParameter);
			continue;
		}

		if (parameterMonitoringList.full()) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ParameterMonitoringListIsFull);
			continue;
		}

		if (static_cast<PMONBase::CheckType>(currentCheckType) == PMONBase::CheckType::Limit) {
			LowLimit lowLimit = message.read<LowLimit>();
			BelowLowLimitEvent belowLowLimitEventId = message.read<BelowLowLimitEvent>();
			HighLimit highLimit = message.read<HighLimit>();
			AboveHighLimitEvent aboveHighLimitEventId = message.read<AboveHighLimitEvent>();
			if (highLimit <= lowLimit) {
				ErrorHandler::reportError(
				    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
				continue;
			}
			PMONLimitCheck limitCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
			                          lowLimit, belowLowLimitEventId, highLimit, aboveHighLimitEventId);
			limitChecks.push_back(limitCheck);
			addPMONDefinition(currentPMONId, limitCheck);
		} else if (static_cast<PMONBase::CheckType>(currentCheckType) == PMONBase::CheckType::ExpectedValue) {
			Mask mask = message.read<Mask>();
			Expected_Value expectedValue = message.read<Expected_Value>();
			UnexpectedValueEvent unExpectedValueEvent = message.read<UnexpectedValueEvent>();
			PMONExpectedValueCheck expectedValueCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
			                                          expectedValue, mask, unExpectedValueEvent);
			expectedValueChecks.push_back(expectedValueCheck);
			addPMONDefinition(currentPMONId, expectedValueCheck);
		} else if (static_cast<PMONBase::CheckType>(currentCheckType) == PMONBase::CheckType::Delta) {
			LowDeltaThreshold lowDeltaThreshold = message.read<LowDeltaThreshold>();
			BelowLowThresholdEvent belowLowThresholdEventId = message.read<BelowLowThresholdEvent>();
			HighDeltaThreshold highDeltaThreshold = message.read<HighDeltaThreshold>();
			AboveHighThresholdEvent aboveHighThresholdEventId = message.read<AboveHighThresholdEvent>();
			NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = message.read<NumberOfConsecutiveDeltaChecks>();
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
	}
}


void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		PMONId currentPMONId = message.read<PMONId>();
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end() ||
		    getPMONDefinition(currentPMONId).get().monitoringEnabled) {
			ErrorHandler::reportError(message, ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinition);
			continue;
		}
		parameterMonitoringList.erase(currentPMONId);
		}
	}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);

	uint16_t numberOfIds = message.readUint16();

	for (uint16_t i = 0; i < numberOfIds; i++) {

		PMONId currentPMONId = message.read<PMONId>();
		MonitoredParameterId currentMonitoredParameterId = message.read<MonitoredParameterId>();
		RepetitionNumber currentPMONRepetitionNumber = message.read<RepetitionNumber>();
		uint16_t currentCheckType = message.readEnum8();

		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::ModifyParameterNotInTheParameterMonitoringList);
			return;
		}

		if (getPMONDefinition(currentPMONId).get().monitoredParameterId != currentMonitoredParameterId) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::
			                                       DifferentParameterMonitoringDefinitionAndMonitoredParameter);
			return;
		}

		auto parameterToBeModified = Services.parameterManagement.getParameter(currentMonitoredParameterId);
		if (not parameterToBeModified) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			return;
		}

		switch (static_cast<PMONBase::CheckType>(currentCheckType)) {
			case PMONBase::CheckType::Limit: {
				LowLimit lowLimit = message.read<LowLimit>();
				BelowLowLimitEvent belowLowLimitEventId = message.read<BelowLowLimitEvent>();
				HighLimit highLimit = message.read<HighLimit>();
				AboveHighLimitEvent aboveHighLimitEventId = message.read<AboveHighLimitEvent>();

				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}

				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONLimitCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, lowLimit, belowLowLimitEventId, highLimit, aboveHighLimitEventId);
				addPMONDefinition(currentPMONId, monitoringDefinition);
				break;
			}

			case PMONBase::PMONBase::CheckType::ExpectedValue: {
				Mask mask = message.read<Mask>();
				Expected_Value expectedValue = message.read<Expected_Value>();
				UnexpectedValueEvent unExpectedValueEvent = message.read<UnexpectedValueEvent>();

				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONExpectedValueCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, expectedValue, mask, unExpectedValueEvent);
				addPMONDefinition(currentPMONId, monitoringDefinition);
				break;
			}

			case PMONBase::CheckType::Delta: {
				LowDeltaThreshold lowDeltaThreshold = message.read<LowDeltaThreshold>();
				BelowLowThresholdEvent belowLowThresholdEventId = message.read<BelowLowThresholdEvent>();
				HighDeltaThreshold highDeltaThreshold = message.read<HighDeltaThreshold>();
				AboveHighThresholdEvent aboveHighThresholdEventId = message.read<AboveHighThresholdEvent>();
				NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = message.read<NumberOfConsecutiveDeltaChecks>();

				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}

				getPMONDefinition(currentPMONId).get().repetitionCounter = 0;
				getPMONDefinition(currentPMONId).get().repetitionNumber = currentPMONRepetitionNumber;
				getPMONDefinition(currentPMONId).get().checkingStatus = PMONBase::Unchecked;
				parameterMonitoringList.erase(currentPMONId);
				auto monitoringDefinition = PMONDeltaCheck(currentMonitoredParameterId, currentPMONRepetitionNumber, numberOfConsecutiveDeltaChecks, lowDeltaThreshold, belowLowThresholdEventId, highDeltaThreshold, aboveHighThresholdEventId);
				addPMONDefinition(currentPMONId, monitoringDefinition);
				break;
			}
		}
	}
}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	parameterMonitoringDefinitionReport(message);
}

void OnBoardMonitoringService::parameterMonitoringDefinitionReport(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	Message parameterMonitoringDefinitionReport(ServiceType, MessageType::ParameterMonitoringDefinitionReport,
	                                            Message::TM, ApplicationId);
	parameterMonitoringDefinitionReport.appendUint16(maximumTransitionReportingDelay);
	uint16_t numberOfIds = message.readUint16();
	parameterMonitoringDefinitionReport.appendUint16(numberOfIds);
	for (uint16_t i = 0; i < numberOfIds; i++) {
		auto currentPMONId = message.read<PMONId>();
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ReportParameterNotInTheParameterMonitoringList);
			continue;
		}
		parameterMonitoringDefinitionReport.append<PMONId>(currentPMONId);
		parameterMonitoringDefinitionReport.append<MonitoredParameterId>(getPMONDefinition(currentPMONId).get().monitoredParameterId);
		parameterMonitoringDefinitionReport.appendEnum8(static_cast<uint8_t>(getPMONDefinition(currentPMONId).get().monitoringEnabled));
		parameterMonitoringDefinitionReport.append<RepetitionNumber>(getPMONDefinition(currentPMONId).get().repetitionNumber);
		parameterMonitoringDefinitionReport.appendEnum8(static_cast<uint8_t>(getPMONDefinition(currentPMONId).get().checkType));
		if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::CheckType::Limit) {
			parameterMonitoringDefinitionReport.append<LowLimit>(getPMONDefinition(currentPMONId).get().getLowLimit());
			parameterMonitoringDefinitionReport.append<BelowLowLimitEvent>(getPMONDefinition(currentPMONId).get().getBelowLowLimitEvent());
			parameterMonitoringDefinitionReport.append<HighLimit>(getPMONDefinition(currentPMONId).get().getHighLimit());
			parameterMonitoringDefinitionReport.append<AboveHighLimitEvent>(getPMONDefinition(currentPMONId).get().getAboveHighLimitEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::CheckType::ExpectedValue) {
			parameterMonitoringDefinitionReport.append<Mask>(getPMONDefinition(currentPMONId).get().getMask());
			parameterMonitoringDefinitionReport.append<Expected_Value>(getPMONDefinition(currentPMONId).get().getExpectedValue());
			parameterMonitoringDefinitionReport.append<UnexpectedValueEvent>(getPMONDefinition(currentPMONId).get().getUnexpectedValueEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMONBase::CheckType::Delta) {
			parameterMonitoringDefinitionReport.append<LowDeltaThreshold>(getPMONDefinition(currentPMONId).get().getLowDeltaThreshold());
			parameterMonitoringDefinitionReport.append<BelowLowThresholdEvent>(getPMONDefinition(currentPMONId).get().getBelowLowThresholdEvent());
			parameterMonitoringDefinitionReport.append<HighDeltaThreshold>(getPMONDefinition(currentPMONId).get().getHighDeltaThreshold());
			parameterMonitoringDefinitionReport.append<AboveHighThresholdEvent>(getPMONDefinition(currentPMONId).get().getAboveHighThresholdEvent());
			parameterMonitoringDefinitionReport.append<NumberOfConsecutiveDeltaChecks>(getPMONDefinition(currentPMONId).get().getNumberOfConsecutiveDeltaChecks());
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
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif