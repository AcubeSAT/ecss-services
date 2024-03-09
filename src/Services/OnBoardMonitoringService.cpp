#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include "Message.hpp"
#include "ServicePool.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include <iostream>


void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, EnableParameterMonitoringDefinitions)) {
		return;
	}

	uint16_t const numberOfPMONDefinitions = message.readUint16();
	for (uint16_t i = 0; i < numberOfPMONDefinitions; i++) {
		const ParameterId currentId = message.read<ParameterId>();
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

	uint16_t const numberOfPMONDefinitions = message.readUint16();
	for (uint16_t i = 0; i < numberOfPMONDefinitions; i++) {
		const ParameterId currentId = message.read<ParameterId>();
		auto definition = parameterMonitoringList.find(currentId);
		if (definition == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
			continue;
		}
		definition->second.get().monitoringEnabled = false;
		definition->second.get().checkingStatus = PMON::Unchecked;
	}
}

void OnBoardMonitoringService::changeMaximumTransitionReportingDelay(Message& message) {
	if (!message.assertTC(ServiceType, ChangeMaximumTransitionReportingDelay)) {
		return;
	}
	maximumTransitionReportingDelay = message.readUint16();
}

void OnBoardMonitoringService::deleteAllParameterMonitoringDefinitions(const Message& message) { // NOLINT (readability-convert-member-functions-to-const)
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

	for (uint16_t i = 0; i < numberOfIds; i++) {
		ParameterId currentPMONId = message.read<ParameterId>();
		ParameterId currentMonitoredParameterId = message.read<ParameterId>();
		PMONRepetitionNumber currentPMONRepetitionNumber = message.read<PMONRepetitionNumber>();
		uint16_t currentCheckTypeRaw = message.readEnum8();
		PMON::CheckType currentCheckType = static_cast<PMON::CheckType>(currentCheckTypeRaw);

		auto parameterToBeAdded = Services.parameterManagement.getParameter(currentMonitoredParameterId);
		if (!parameterToBeAdded) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
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

		switch (currentCheckType) {
			case PMON::CheckType::Limit: {
				PMONLimit lowLimit = message.read<PMONLimit>();
				EventDefinitionId belowLowLimitEventId = message.read<EventDefinitionId>();
				PMONLimit highLimit = message.read<PMONLimit>();
				EventDefinitionId aboveHighLimitEventId = message.read<EventDefinitionId>();
				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}
				PMONLimitCheck limitCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                          lowLimit, belowLowLimitEventId, highLimit, aboveHighLimitEventId);
				limitCheck.checkingStatus = PMON::Unchecked;
				limitCheck.monitoringEnabled = false;
				addPMONLimitCheck(currentPMONId, limitCheck);
				break;
			}
			case PMON::CheckType::ExpectedValue: {
				PMONBitMask mask = message.read<PMONBitMask>();
				PMONExpectedValue expectedValue = message.read<PMONExpectedValue>();
				EventDefinitionId unexpectedValueEvent = message.read<EventDefinitionId>();
				PMONExpectedValueCheck expectedValueCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                                          expectedValue, mask, unexpectedValueEvent);
				expectedValueCheck.checkingStatus = PMON::Unchecked;
				expectedValueCheck.monitoringEnabled = false;
				addPMONExpectedValueCheck(currentPMONId, expectedValueCheck);
				break;
			}
			case PMON::CheckType::Delta: {
				DeltaThreshold lowDeltaThreshold = message.read<DeltaThreshold>();
				EventDefinitionId belowLowThresholdEventId = message.read<EventDefinitionId>();
				DeltaThreshold highDeltaThreshold = message.read<DeltaThreshold>();
				EventDefinitionId aboveHighThresholdEventId = message.read<EventDefinitionId>();
				NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = message.read<NumberOfConsecutiveDeltaChecks>();
				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}
				PMONDeltaCheck deltaCheck(currentMonitoredParameterId, currentPMONRepetitionNumber,
				                          numberOfConsecutiveDeltaChecks, lowDeltaThreshold, belowLowThresholdEventId, highDeltaThreshold, aboveHighThresholdEventId);
				deltaCheck.checkingStatus = PMON::Unchecked;
				deltaCheck.monitoringEnabled = false;
				addPMONDeltaCheck(currentPMONId, deltaCheck);
				break;
			}
		}
	}
}

void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		ParameterId currentPMONId = message.read<ParameterId>();

		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinition);
			continue;
		}

		if (getPMONDefinition(currentPMONId).get().monitoringEnabled) {
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

		ParameterId currentPMONId = message.read<ParameterId>();
		ParameterId currentMonitoredParameterId = message.read<ParameterId>();
		PMONRepetitionNumber currentPMONRepetitionNumber = message.read<PMONRepetitionNumber>();
		uint16_t currentCheckType = message.readEnum8();

		auto it = parameterMonitoringList.find(currentPMONId);

		if (it == parameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::ModifyParameterNotInTheParameterMonitoringList);
			return;
		}

		if (getPMONDefinition(currentPMONId).get().monitoredParameterId != currentMonitoredParameterId) {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::
			                                       DifferentParameterMonitoringDefinitionAndMonitoredParameter);
			return;
		}

		PMON& pmon = it->second.get();
		pmon.repetitionCounter = 0;
		pmon.repetitionNumber = currentPMONRepetitionNumber;
		pmon.checkingStatus = PMON::Unchecked;

		switch (static_cast<PMON::CheckType>(currentCheckType)) {
			case PMON::CheckType::Limit: {
				PMONLimit lowLimit = message.read<PMONLimit>();
				EventDefinitionId belowLowLimitEventId = message.read<EventDefinitionId>();
				PMONLimit highLimit = message.read<PMONLimit>();
				EventDefinitionId aboveHighLimitEventId = message.read<EventDefinitionId>();

				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}

				auto& limitCheck = dynamic_cast<PMONLimitCheck&>(pmon);
				limitCheck.lowLimit = lowLimit;
				limitCheck.belowLowLimitEvent = belowLowLimitEventId;
				limitCheck.highLimit = highLimit;
				limitCheck.aboveHighLimitEvent = aboveHighLimitEventId;
				break;
			}


			case PMON::CheckType::ExpectedValue: {
				PMONBitMask mask = message.read<PMONBitMask>();
				PMONExpectedValue expectedValue = message.read<PMONExpectedValue>();
				EventDefinitionId unexpectedValueEvent = message.read<EventDefinitionId>();

				auto& expectedValueCheck = dynamic_cast<PMONExpectedValueCheck&>(pmon);
				expectedValueCheck.mask = mask;
				expectedValueCheck.expectedValue = expectedValue;
				expectedValueCheck.unexpectedValueEvent = unexpectedValueEvent;
				break;
			}

			case PMON::CheckType::Delta: {
				DeltaThreshold lowDeltaThreshold = message.read<DeltaThreshold>();
				EventDefinitionId belowLowThresholdEventId = message.read<EventDefinitionId>();
				DeltaThreshold highDeltaThreshold = message.read<DeltaThreshold>();
				EventDefinitionId aboveHighThresholdEventId = message.read<EventDefinitionId>();
				NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = message.read<NumberOfConsecutiveDeltaChecks>();

				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}

				auto& deltaCheck = dynamic_cast<PMONDeltaCheck&>(pmon);
				deltaCheck.numberOfConsecutiveDeltaChecks = numberOfConsecutiveDeltaChecks;
				deltaCheck.lowDeltaThreshold = lowDeltaThreshold;
				deltaCheck.belowLowThresholdEvent = belowLowThresholdEventId;
				deltaCheck.highDeltaThreshold = highDeltaThreshold;
				deltaCheck.aboveHighThresholdEvent = aboveHighThresholdEventId;
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
		auto currentPMONId = message.read<ParameterId>();
		if (parameterMonitoringList.find(currentPMONId) == parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ReportParameterNotInTheParameterMonitoringList);
			continue;
		}
		parameterMonitoringDefinitionReport.append<ParameterId>(currentPMONId);
		parameterMonitoringDefinitionReport.append<ParameterId>(getPMONDefinition(currentPMONId).get().monitoredParameterId);
		parameterMonitoringDefinitionReport.appendEnum8(static_cast<uint8_t>(getPMONDefinition(currentPMONId).get().monitoringEnabled));
		parameterMonitoringDefinitionReport.append<PMONRepetitionNumber>(getPMONDefinition(currentPMONId).get().repetitionNumber);
		if (getPMONDefinition(currentPMONId).get().checkType.has_value()) {
			uint8_t checkTypeValue = static_cast<uint8_t>(getPMONDefinition(currentPMONId).get().checkType.value());
			parameterMonitoringDefinitionReport.appendEnum8(checkTypeValue);
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::PMONCheckTypeMissing);
			continue;
		}
		if (getPMONDefinition(currentPMONId).get().checkType == PMON::CheckType::Limit) {
			parameterMonitoringDefinitionReport.append<PMONLimit>(getPMONDefinition(currentPMONId).get().getLowLimit());
			parameterMonitoringDefinitionReport.append<EventDefinitionId>(getPMONDefinition(currentPMONId).get().getBelowLowLimitEvent());
			parameterMonitoringDefinitionReport.append<PMONLimit>(getPMONDefinition(currentPMONId).get().getHighLimit());
			parameterMonitoringDefinitionReport.append<EventDefinitionId>(getPMONDefinition(currentPMONId).get().getAboveHighLimitEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMON::CheckType::ExpectedValue) {
			parameterMonitoringDefinitionReport.append<PMONBitMask>(getPMONDefinition(currentPMONId).get().getMask());
			parameterMonitoringDefinitionReport.append<PMONExpectedValue>(getPMONDefinition(currentPMONId).get().getExpectedValue());
			parameterMonitoringDefinitionReport.append<EventDefinitionId>(getPMONDefinition(currentPMONId).get().getUnexpectedValueEvent());
		} else if (getPMONDefinition(currentPMONId).get().checkType == PMON::CheckType::Delta) {
			parameterMonitoringDefinitionReport.append<DeltaThreshold>(getPMONDefinition(currentPMONId).get().getLowDeltaThreshold());
			parameterMonitoringDefinitionReport.append<EventDefinitionId>(getPMONDefinition(currentPMONId).get().getBelowLowThresholdEvent());
			parameterMonitoringDefinitionReport.append<DeltaThreshold>(getPMONDefinition(currentPMONId).get().getHighDeltaThreshold());
			parameterMonitoringDefinitionReport.append<EventDefinitionId>(getPMONDefinition(currentPMONId).get().getAboveHighThresholdEvent());
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