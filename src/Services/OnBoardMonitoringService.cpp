#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include "Message.hpp"
#include "ServicePool.hpp"
#include "Services/OnBoardMonitoringService.hpp"

void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, EnableParameterMonitoringDefinitions)) {
		return;
	}

	uint16_t const numberOfPMONDefinitions = message.readUint16();
	for (uint16_t i = 0; i < numberOfPMONDefinitions; i++) {
		const PMONDefinitionId currentId = message.read<PMONDefinitionId>();
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
		const PMONDefinitionId currentId = message.read<PMONDefinitionId>();
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

bool OnBoardMonitoringService::canReadCriteria(const Message& message, PMON::CheckType checkType) {
	uint16_t criteriaSize = 0;
	switch (checkType) {
		case PMON::CheckType::Limit:
			criteriaSize = 2 * (sizeof(PMONLimit) + sizeof(EventDefinitionId));
			break;
		case PMON::CheckType::ExpectedValue:
			criteriaSize = sizeof(PMONBitMask) + sizeof(PMONExpectedValue) + sizeof(EventDefinitionId);
			break;
		case PMON::CheckType::Delta:
			criteriaSize = 2 * (sizeof(DeltaThreshold) + sizeof(EventDefinitionId)) + sizeof(NumberOfConsecutiveDeltaChecks);
			break;
		default:
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::PMONCheckTypeInvalid);
			return false;
	}
	return ErrorHandler::assertRequest(message.readPosition + criteriaSize <= message.dataSize, message, ErrorHandler::MessageTooShort);
}

bool OnBoardMonitoringService::canAddDefinition(const Message& message, PMONDefinitionId PMONId, ParameterId monitoredParameterId) const {
	if (parameterMonitoringList.full()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ParameterMonitoringListIsFull);
		return false;
	}
	if (parameterMonitoringList.contains(PMONId)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::AddAlreadyExistingParameter);
		return false;
	}
	if (!Services.parameterManagement.parameterExists(monitoredParameterId)) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
		return false;
	}
	return true;
}

void OnBoardMonitoringService::addParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, AddParameterMonitoringDefinitions);

	uint16_t numberOfIds = message.readUint16();

	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (not ErrorHandler::assertRequest(message.readPosition + InstructionHeaderSize <= message.dataSize, message, ErrorHandler::MessageTooShort)) {
			return;
		}

		PMONDefinitionId currentPMONId = message.read<PMONDefinitionId>();
		ParameterId currentMonitoredParameterId = message.read<ParameterId>();
		PMONRepetitionNumber currentPMONRepetitionNumber = message.read<PMONRepetitionNumber>();
		const auto currentCheckType = message.read<PMON::CheckType>();
		if (not canReadCriteria(message, currentCheckType)) {
			return;
		}
		const bool canBeAdded = canAddDefinition(message, currentPMONId, currentMonitoredParameterId);

		switch (currentCheckType) {
			case PMON::CheckType::Limit: {
				PMONLimit lowLimit = message.read<PMONLimit>();
				EventDefinitionId belowLowLimitEventId = message.read<EventDefinitionId>();
				PMONLimit highLimit = message.read<PMONLimit>();
				EventDefinitionId aboveHighLimitEventId = message.read<EventDefinitionId>();
				if (not canBeAdded) {
					continue;
				}
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
				if (not canBeAdded) {
					continue;
				}
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
				if (not canBeAdded) {
					continue;
				}
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
			default:
				return;
		}
	}
}

void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);
	uint16_t numberOfIds = message.readUint16();
	for (uint16_t i = 0; i < numberOfIds; i++) {
		PMONDefinitionId currentPMONId = message.read<PMONDefinitionId>();

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

etl::optional<etl::reference_wrapper<PMON>> OnBoardMonitoringService::findDefinitionToModify(const Message& message, PMONDefinitionId PMONId, ParameterId monitoredParameterId, PMON::CheckType checkType) const {
	const auto definition = parameterMonitoringList.find(PMONId);
	if (definition == parameterMonitoringList.end()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::ModifyParameterNotInTheParameterMonitoringList);
		return {};
	}
	if (definition->second.get().monitoredParameterId != monitoredParameterId) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::DifferentParameterMonitoringDefinitionAndMonitoredParameter);
		return {};
	}
	if (definition->second.get().checkType != checkType) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::DifferentParameterMonitoringDefinitionAndCheckType);
		return {};
	}
	return definition->second;
}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);

	uint16_t numberOfIds = message.readUint16();

	for (uint16_t i = 0; i < numberOfIds; i++) {
		if (not ErrorHandler::assertRequest(message.readPosition + InstructionHeaderSize <= message.dataSize, message, ErrorHandler::MessageTooShort)) {
			return;
		}

		PMONDefinitionId currentPMONId = message.read<PMONDefinitionId>();
		ParameterId currentMonitoredParameterId = message.read<ParameterId>();
		PMONRepetitionNumber currentPMONRepetitionNumber = message.read<PMONRepetitionNumber>();
		const auto currentCheckType = message.read<PMON::CheckType>();
		if (not canReadCriteria(message, currentCheckType)) {
			return;
		}
		const auto definition = findDefinitionToModify(message, currentPMONId, currentMonitoredParameterId, currentCheckType);

		switch (currentCheckType) {
			case PMON::CheckType::Limit: {
				PMONLimit lowLimit = message.read<PMONLimit>();
				EventDefinitionId belowLowLimitEventId = message.read<EventDefinitionId>();
				PMONLimit highLimit = message.read<PMONLimit>();
				EventDefinitionId aboveHighLimitEventId = message.read<EventDefinitionId>();

				if (not definition) {
					continue;
				}

				if (highLimit <= lowLimit) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighLimitIsLowerThanLowLimit);
					continue;
				}

				auto& limitCheck = static_cast<PMONLimitCheck&>(definition->get());
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

				if (not definition) {
					continue;
				}

				auto& expectedValueCheck = static_cast<PMONExpectedValueCheck&>(definition->get());
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

				if (not definition) {
					continue;
				}

				if (highDeltaThreshold <= lowDeltaThreshold) {
					ErrorHandler::reportError(
					    message, ErrorHandler::ExecutionStartErrorType::HighThresholdIsLowerThanLowThreshold);
					continue;
				}

				auto& deltaCheck = static_cast<PMONDeltaCheck&>(definition->get());
				deltaCheck.numberOfConsecutiveDeltaChecks = numberOfConsecutiveDeltaChecks;
				deltaCheck.lowDeltaThreshold = lowDeltaThreshold;
				deltaCheck.belowLowThresholdEvent = belowLowThresholdEventId;
				deltaCheck.highDeltaThreshold = highDeltaThreshold;
				deltaCheck.aboveHighThresholdEvent = aboveHighThresholdEventId;
				break;
			}

			default:
				return;
		}

		PMON& pmon = definition->get();
		pmon.repetitionCounter = 0;
		pmon.repetitionNumber = currentPMONRepetitionNumber;
		pmon.checkingStatus = PMON::Unchecked;
	}
}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);
	Message pmonDefinitionReport(ServiceType, MessageType::ParameterMonitoringDefinitionReport, Message::TM, ApplicationId);
	pmonDefinitionReport.appendUint16(maximumTransitionReportingDelay);
	uint16_t numberOfIds = message.readUint16();
	pmonDefinitionReport.appendUint16(numberOfIds);

	for (uint16_t i = 0; i < numberOfIds; i++) {
		auto currentPMONId = message.read<PMONDefinitionId>();

		auto it = parameterMonitoringList.find(currentPMONId);
		if (it == parameterMonitoringList.end()) {
			ErrorHandler::reportError(message, ErrorHandler::ReportParameterNotInTheParameterMonitoringList);
			continue;
		}

		PMON& pmon = it->second.get();

		pmonDefinitionReport.append<PMONDefinitionId>(currentPMONId);
		pmonDefinitionReport.append<ParameterId>(pmon.monitoredParameterId);
		pmonDefinitionReport.appendBoolean(pmon.monitoringEnabled);
		pmonDefinitionReport.append<PMONRepetitionNumber>(pmon.repetitionNumber);

		auto checkTypeValue = pmon.checkType;
		pmonDefinitionReport.append<PMON::CheckType>(checkTypeValue);

		switch (pmon.checkType) {
			case PMON::CheckType::Limit: {
				auto& limitCheck = static_cast<PMONLimitCheck&>(pmon);
				pmonDefinitionReport.append<PMONLimit>(limitCheck.getLowLimit());
				pmonDefinitionReport.append<EventDefinitionId>(limitCheck.getBelowLowLimitEvent());
				pmonDefinitionReport.append<PMONLimit>(limitCheck.getHighLimit());
				pmonDefinitionReport.append<EventDefinitionId>(limitCheck.getAboveHighLimitEvent());

				break;
			}
			case PMON::CheckType::ExpectedValue: {
				auto& expectedValueCheck = static_cast<PMONExpectedValueCheck&>(pmon);
				pmonDefinitionReport.append<PMONBitMask>(expectedValueCheck.getMask());
				pmonDefinitionReport.append<PMONExpectedValue>(expectedValueCheck.getExpectedValue());
				pmonDefinitionReport.append<EventDefinitionId>(expectedValueCheck.getUnexpectedValueEvent());

				break;
			}
			case PMON::CheckType::Delta: {
				auto& deltaCheck = static_cast<PMONDeltaCheck&>(pmon);
				pmonDefinitionReport.append<DeltaThreshold>(deltaCheck.getLowDeltaThreshold());
				pmonDefinitionReport.append<EventDefinitionId>(deltaCheck.getBelowLowThresholdEvent());
				pmonDefinitionReport.append<DeltaThreshold>(deltaCheck.getHighDeltaThreshold());
				pmonDefinitionReport.append<EventDefinitionId>(deltaCheck.getAboveHighThresholdEvent());
				pmonDefinitionReport.append<NumberOfConsecutiveDeltaChecks>(deltaCheck.getNumberOfConsecutiveDeltaChecks());

				break;
			}
		}
	}
	storeMessage(pmonDefinitionReport);
}

void OnBoardMonitoringService::checkAll() const {
	for (const auto& entry : parameterMonitoringList) {
		auto& pmon = entry.second.get();
		if (pmon.isMonitoringEnabled()) {
			pmon.performCheck();
		}
	}
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