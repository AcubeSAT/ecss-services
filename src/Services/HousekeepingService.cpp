#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure)) {
		return;
	}

	ParameterReportStructureId idToCreate = request.read<ParameterReportStructureId>();
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

	uint16_t numOfSimplyCommutatedParams = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParams; i++) {
		ParameterId newParamId = request.read<ParameterId>();
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
	uint8_t numOfStructuresToDelete = request.readUint8();
	for (uint8_t i = 0; i < numOfStructuresToDelete; i++) {
		ParameterReportStructureId structureId = request.read<ParameterReportStructureId>();
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

	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		ParameterReportStructureId structIdToEnable = request.read<ParameterReportStructureId>();
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

	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		ParameterReportStructureId structIdToDisable = request.read<ParameterReportStructureId>();
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

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		ParameterReportStructureId structureId = request.read<ParameterReportStructureId>();
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
	structReport.append(structIdToReport);

	structReport.appendBoolean(housekeepingStructure->second.periodicGenerationActionStatus);
	structReport.append(housekeepingStructure->second.collectionInterval);
	structReport.appendUint16(housekeepingStructure->second.simplyCommutatedParameterIds.size());

	for (auto parameterId: housekeepingStructure->second.simplyCommutatedParameterIds) {
		structReport.append(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::housekeepingParametersReport(ParameterReportStructureId structureId) {
	if (hasNonExistingStructInternalError(structureId)) {
		return;
	}

	auto& housekeepingStructure = getStruct(structureId)->get();

	Message housekeepingReport = createTM(MessageType::HousekeepingParametersReport);

	housekeepingReport.append(structureId);
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

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		ParameterReportStructureId structureId = request.read<ParameterReportStructureId>();
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

	ParameterReportStructureId targetStructId = request.read<ParameterReportStructureId>();
	if (hasNonExistingStructExecutionError(targetStructId, request)) {
		return;
	}
	auto& housekeepingStructure = getStruct(targetStructId)->get();
	if (hasRequestedAppendToEnabledHousekeepingError(housekeepingStructure, request)) {
		return;
	}
	uint16_t numOfSimplyCommutatedParameters = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParameters; i++) {
		if (hasExceededMaxNumOfSimplyCommutatedParamsError(housekeepingStructure, request)) {
			return;
		}
		ParameterId newParamId = request.read<ParameterId>();
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

	uint8_t numOfTargetStructs = request.readUint8();
	for (uint8_t i = 0; i < numOfTargetStructs; i++) {
		ParameterReportStructureId targetStructId = request.read<ParameterReportStructureId>();
		CollectionInterval newCollectionInterval = request.read<CollectionInterval>();
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
	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		ParameterReportStructureId structIdToReport = request.read<ParameterReportStructureId>();
		if (structExists(structIdToReport)) {
			numOfValidIds++;
		}
	}
	Message periodicPropertiesReport = createTM(MessageType::HousekeepingPeriodicPropertiesReport);
	periodicPropertiesReport.appendUint8(numOfValidIds);
	request.resetRead();
	request.readUint8();

	for (uint8_t i = 0; i < numOfStructIds; i++) {
		ParameterReportStructureId structIdToReport = request.read<ParameterReportStructureId>();
		if (hasNonExistingStructExecutionError(structIdToReport, request)) {
			continue;
		}
		appendPeriodicPropertiesToMessage(periodicPropertiesReport, structIdToReport);
	}
	storeMessage(periodicPropertiesReport);
}

void HousekeepingService::appendPeriodicPropertiesToMessage(Message& report, ParameterReportStructureId structureId) {
	report.append(structureId);
	report.appendBoolean(getPeriodicGenerationActionStatus(structureId));
	report.append(getCollectionInterval(structureId));
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

TimeInt
HousekeepingService::reportPendingStructures(TimeInt currentTime, TimeInt previousTime, TimeInt expectedDelay) {
	TimeInt nextCollection = std::numeric_limits<uint32_t>::max();

	for (auto& housekeepingStructure: housekeepingStructures) {
		if (!housekeepingStructure.second.periodicGenerationActionStatus) {
			continue;
		}
		if (housekeepingStructure.second.collectionInterval == 0) {
			housekeepingParametersReport(housekeepingStructure.second.structureId);
			nextCollection = 0;
			continue;
		}
		if (currentTime != 0 and (currentTime % housekeepingStructure.second.collectionInterval == 0 or
		                          (previousTime + expectedDelay) % housekeepingStructure.second.collectionInterval ==
		                              0)) {
			housekeepingParametersReport(housekeepingStructure.second.structureId);
		}
		TimeInt structureTimeToCollection = housekeepingStructure.second.collectionInterval -
		                                     currentTime % housekeepingStructure.second.collectionInterval;
		if (nextCollection > structureTimeToCollection) {
			nextCollection = structureTimeToCollection;
		}
	}

	return nextCollection;
}

bool HousekeepingService::hasNonExistingStructExecutionError(ParameterReportStructureId id, Message& req) {
	if (!structExists(id)) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return true;
	}
	return false;
}

bool HousekeepingService::hasNonExistingStructError(ParameterReportStructureId id, Message& req) {
	if (!structExists(id)) {
		ErrorHandler::reportError(req, ErrorHandler::RequestedNonExistingStructure);
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
bool HousekeepingService::hasAlreadyExistingParameterError(HousekeepingStructure& housekeepingStruct, ParameterReportStructureId id, Message& req) {
	if (existsInVector(housekeepingStruct.simplyCommutatedParameterIds, id)) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
		return true;
	}
	return false;
}

bool HousekeepingService::hasAlreadyExistingStructError(ParameterReportStructureId id, Message& req) {
	if (structExists(id)) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return true;
	}
	return false;
}

bool HousekeepingService::hasExceededMaxNumOfHousekeepingStructsError(Message& req) {
	if (housekeepingStructures.size() >= ECSSMaxHousekeepingStructures) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures);
		return true;
	}
	return false;
}

bool HousekeepingService::hasRequestedAppendToEnabledHousekeepingError(HousekeepingStructure& housekeepingStruct, Message& req) {
	if (housekeepingStruct.periodicGenerationActionStatus) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping);
		return true;
	}
	return false;
}

bool HousekeepingService::hasRequestedDeletionOfEnabledHousekeepingError(ParameterReportStructureId id, Message& req) {
	if (getPeriodicGenerationActionStatus(id)) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfEnabledHousekeeping);
		return true;
	}
	return false;
}

bool HousekeepingService::hasExceededMaxNumOfSimplyCommutatedParamsError(HousekeepingStructure& housekeepingStruct, Message& req) {
	if (housekeepingStruct.simplyCommutatedParameterIds.size() >= ECSSMaxSimplyCommutatedParameters) {
		ErrorHandler::reportError(req, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters);
		return true;
	}
	return false;
}