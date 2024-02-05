#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure)) {
		return;
	}

	const ParameterReportStructureId idToCreate = request.read<ParameterReportStructureId>();
	if (hasAlreadyExistingStructError(idToCreate, request)) {
		return;
	}
	if (hasExceededMaxNumOfHousekeepingStructsError(request)) {
		return;
	}
	HousekeepingStructure newStructure;
	newStructure.structureId = idToCreate;
	newStructure.collectionInterval = request.read<CollectionInterval>();
	newStructure.periodicGenerationActionStatus = false;

	uint16_t const numOfSimplyCommutatedParams = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParams; i++) {
		const ParameterId newParamId = request.read<ParameterId>();
		if (hasAlreadyExistingParameterError(newStructure, newParamId, request)) {
			continue;
		}
		newStructure.simplyCommutatedParameterIds.push_back(newParamId);
	}
	housekeepingStructures.insert({idToCreate, newStructure});
}

void HousekeepingService::deleteHousekeepingReportStructure(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeleteHousekeepingReportStructure)) {
		return;
	}
	uint8_t const numOfStructuresToDelete = request.readUint8();
	for (uint8_t i = 0; i < numOfStructuresToDelete; i++) {
		ParameterReportStructureId const structureId = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructExecutionError(structureId, request)) {
			continue;
		}

		if (hasRequestedDeletionOfEnabledHousekeepingError(structureId, request)) {
			continue;
		}
		housekeepingStructures.erase(structureId);
	}
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::EnablePeriodicHousekeepingParametersReport)) {
		return;
	}

	uint8_t const numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		const ParameterReportStructureId structIdToEnable = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructError(structIdToEnable, request)) {
			continue;
		}
		setPeriodicGenerationActionStatus(structIdToEnable, true);
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DisablePeriodicHousekeepingParametersReport)) {
		return;
	}

	uint8_t const numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		const ParameterReportStructureId structIdToDisable = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructError(structIdToDisable, request)) {
			continue;
		}
		setPeriodicGenerationActionStatus(structIdToDisable, false);
	}
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportHousekeepingStructures)) {
		return;
	}

	uint8_t const numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		const ParameterReportStructureId structureId = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructExecutionError(structureId, request)) {
			continue;
		}

		housekeepingStructureReport(structureId);
	}
}

void HousekeepingService::housekeepingStructureReport(ParameterReportStructureId structIdToReport) {
	auto housekeepingStructure = housekeepingStructures.find(structIdToReport);
	if (hasNonExistingStructInternalError(structIdToReport)) {
		return;
	}
	Message structReport = createTM(MessageType::HousekeepingStructuresReport);
	structReport.append<ParameterReportStructureId>(structIdToReport);

	structReport.appendBoolean(housekeepingStructure->second.periodicGenerationActionStatus);
	structReport.append<CollectionInterval>(housekeepingStructure->second.collectionInterval);
	structReport.appendUint16(housekeepingStructure->second.simplyCommutatedParameterIds.size());

	for (auto parameterId: housekeepingStructure->second.simplyCommutatedParameterIds) {
		structReport.append<ParameterId>(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::housekeepingParametersReport(ParameterReportStructureId structureId) {
	if (hasNonExistingStructInternalError(structureId)) {
		return;
	}

	const auto& housekeepingStructure = getStruct(structureId)->get();

	Message housekeepingReport = createTM(MessageType::HousekeepingParametersReport);

	housekeepingReport.append<ParameterReportStructureId>(structureId);
	for (auto id: housekeepingStructure.simplyCommutatedParameterIds) {
		if (auto parameter = Services.parameterManagement.getParameter(id)) {
			parameter->get().appendValueToMessage(housekeepingReport);
		}
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::GenerateOneShotHousekeepingReport)) {
		return;
	}

	uint8_t const numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		const ParameterReportStructureId structureId = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructExecutionError(structureId, request)) {
			continue;
		}

		housekeepingParametersReport(structureId);
	}
}

void HousekeepingService::appendParametersToHousekeepingStructure(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::AppendParametersToHousekeepingStructure)) {
		return;
	}

	const ParameterReportStructureId targetStructId = request.read<ParameterReportStructureId>();
	if (hasNonExistingStructExecutionError(targetStructId, request)) {
		return;
	}
	auto& housekeepingStructure = getStruct(targetStructId)->get(); // NOLINT(bugprone-unchecked-optional-access) // nolint as we check next line
	if (hasRequestedAppendToEnabledHousekeepingError(housekeepingStructure, request)) {
		return;
	}
	uint16_t const numOfSimplyCommutatedParameters = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParameters; i++) {
		if (hasExceededMaxNumOfSimplyCommutatedParamsError(housekeepingStructure, request)) {
			return;
		}
		const ParameterId newParamId = request.read<ParameterId>();
		if (!Services.parameterManagement.parameterExists(newParamId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}
		if (hasAlreadyExistingParameterError(housekeepingStructure, newParamId, request)) {
			continue;
		}
		housekeepingStructure.simplyCommutatedParameterIds.push_back(newParamId);
	}
}

void HousekeepingService::modifyCollectionIntervalOfStructures(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ModifyCollectionIntervalOfStructures)) {
		return;
	}

	uint8_t const numOfTargetStructs = request.readUint8();
	for (uint8_t i = 0; i < numOfTargetStructs; i++) {
		const ParameterReportStructureId targetStructId = request.read<ParameterReportStructureId>();
		const CollectionInterval newCollectionInterval = request.read<CollectionInterval>();
		if (hasNonExistingStructExecutionError(targetStructId, request)) {
			continue;
		}
		setCollectionInterval(targetStructId, newCollectionInterval);
	}
}

void HousekeepingService::reportHousekeepingPeriodicProperties(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportHousekeepingPeriodicProperties)) {
		return;
	}

	uint8_t numOfValidIds = 0;
	uint8_t const numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		const ParameterReportStructureId structIdToReport = request.read<ParameterReportStructureId>();
		if (structExists(structIdToReport)) {
			numOfValidIds++;
		}
	}
	Message periodicPropertiesReport = createTM(MessageType::HousekeepingPeriodicPropertiesReport);
	periodicPropertiesReport.appendUint8(numOfValidIds);
	request.resetRead();
	request.readUint8();

	for (uint8_t i = 0; i < numOfStructIds; i++) {
		const ParameterReportStructureId structIdToReport = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructExecutionError(structIdToReport, request)) {
			continue;
		}
		appendPeriodicPropertiesToMessage(periodicPropertiesReport, structIdToReport);
	}
	storeMessage(periodicPropertiesReport);
}

void HousekeepingService::appendPeriodicPropertiesToMessage(Message& report, ParameterReportStructureId structureId) {
	report.append<ParameterReportStructureId>(structureId);
	report.appendBoolean(getPeriodicGenerationActionStatus(structureId));
	report.append<CollectionInterval>(getCollectionInterval(structureId));
}

void HousekeepingService::execute(Message& message) {
	switch (message.messageType) {
		case CreateHousekeepingReportStructure:
			createHousekeepingReportStructure(message);
			break;
		case DeleteHousekeepingReportStructure:
			deleteHousekeepingReportStructure(message);
			break;
		case EnablePeriodicHousekeepingParametersReport:
			enablePeriodicHousekeepingParametersReport(message);
			break;
		case DisablePeriodicHousekeepingParametersReport:
			disablePeriodicHousekeepingParametersReport(message);
			break;
		case ReportHousekeepingStructures:
			reportHousekeepingStructures(message);
			break;
		case GenerateOneShotHousekeepingReport:
			generateOneShotHousekeepingReport(message);
			break;
		case AppendParametersToHousekeepingStructure:
			appendParametersToHousekeepingStructure(message);
			break;
		case ModifyCollectionIntervalOfStructures:
			modifyCollectionIntervalOfStructures(message);
			break;
		case ReportHousekeepingPeriodicProperties:
			reportHousekeepingPeriodicProperties(message);
			break;
	}
}

bool HousekeepingService::existsInVector(const etl::vector<uint16_t, ECSSMaxSimplyCommutatedParameters>& ids,
                                         ParameterId parameterId) {
	return std::find(std::begin(ids), std::end(ids), parameterId) != std::end(ids);
}

Time::DefaultCUC
HousekeepingService::reportPendingStructures(Time::DefaultCUC currentTime, Time::DefaultCUC previousTime, Time::DefaultCUC expectedDelay) {
	Time::DefaultCUC nextCollection((std::numeric_limits<uint32_t>::max()) * Time::DefaultCUC::Ratio::num / Time::DefaultCUC::Ratio ::den); // NOLINT(misc-const-correctness)

	for (const auto& housekeepingStructure: housekeepingStructures) {
		if (!housekeepingStructure.second.periodicGenerationActionStatus) {
			continue;
		}
		if (housekeepingStructure.second.collectionInterval == 0) {
			housekeepingParametersReport(housekeepingStructure.second.structureId);
			nextCollection = Time::DefaultCUC(0);
			continue;
		}
		if (currentTime.asTAIseconds() != 0 and (currentTime.asTAIseconds() % housekeepingStructure.second.collectionInterval ==
		                                             0 or
		                                         (previousTime.asTAIseconds() + expectedDelay.asTAIseconds()) % housekeepingStructure.second
		                                                                                                            .collectionInterval ==
		                                             0)) {
			housekeepingParametersReport(housekeepingStructure.second.structureId);
		}
		const Time::DefaultCUC structureTimeToCollection(housekeepingStructure.second
		                                                     .collectionInterval -
		                                                 currentTime.asTAIseconds() % housekeepingStructure.second.collectionInterval);
		if (nextCollection > structureTimeToCollection) {
			nextCollection = structureTimeToCollection;
		}
	}

	return nextCollection;
}

bool HousekeepingService::hasNonExistingStructExecutionError(ParameterReportStructureId id, const Message& request) {
	if (!structExists(id)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return true;
	}
	return false;
}

bool HousekeepingService::hasNonExistingStructError(ParameterReportStructureId id, const Message& request) {
	if (!structExists(id)) {
		ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
		return true;
	}
	return false;
}

bool HousekeepingService::hasNonExistingStructInternalError(ParameterReportStructureId id) {
	if (!structExists(id)) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentHousekeeping);
		return true;
	}
	return false;
}
bool HousekeepingService::hasAlreadyExistingParameterError(const HousekeepingStructure& housekeepingStruct, ParameterReportStructureId id, const Message& request) {
	if (existsInVector(housekeepingStruct.simplyCommutatedParameterIds, id)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
		return true;
	}
	return false;
}

bool HousekeepingService::hasAlreadyExistingStructError(ParameterReportStructureId id, const Message& request) {
	if (structExists(id)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return true;
	}
	return false;
}

bool HousekeepingService::hasExceededMaxNumOfHousekeepingStructsError(const Message& request) {
	if (housekeepingStructures.size() >= ECSSMaxHousekeepingStructures) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures);
		return true;
	}
	return false;
}

bool HousekeepingService::hasRequestedAppendToEnabledHousekeepingError(const HousekeepingStructure& housekeepingStruct, const Message& request) {
	if (housekeepingStruct.periodicGenerationActionStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping);
		return true;
	}
	return false;
}

bool HousekeepingService::hasRequestedDeletionOfEnabledHousekeepingError(ParameterReportStructureId id, const Message& request) {
	if (getPeriodicGenerationActionStatus(id)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfEnabledHousekeeping);
		return true;
	}
	return false;
}

bool HousekeepingService::hasExceededMaxNumOfSimplyCommutatedParamsError(const HousekeepingStructure& housekeepingStruct, const Message& request) {
	if (housekeepingStruct.simplyCommutatedParameterIds.size() >= ECSSMaxSimplyCommutatedParameters) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters);
		return true;
	}
	return false;
}