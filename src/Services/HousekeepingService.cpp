#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"


bool HousekeepingService::periodicGenerationActionStatusGetter(uint8_t id){
	return housekeepingStructures.at(id).periodicGenerationActionStatus;
}

HousekeepingStructure& HousekeepingService::structGetter(uint8_t id){
	return housekeepingStructures.at(id);
}

uint32_t HousekeepingService::collectionIntervalGetter(uint8_t id){
	return housekeepingStructures.at(id).collectionInterval;
}

void HousekeepingService::periodicGenerationActionStatusSetter(uint8_t id, bool b){
	housekeepingStructures.at(id).periodicGenerationActionStatus = b;
}

void HousekeepingService::collectionIntervalSetter(uint8_t id, uint32_t i){
	housekeepingStructures.at(id).collectionInterval;
}

bool HousekeepingService::nonExistingStruct(uint8_t id){
	return (housekeepingStructures.find(id) == housekeepingStructures.end());
}

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure);

	uint8_t idToCreate = request.readUint8();
	if (!nonExistingStruct(idToCreate)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return;
	}
	if (housekeepingStructures.size() >= ECSSMaxHousekeepingStructures) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures);
		return;
	}
	HousekeepingStructure newStructure;
	newStructure.structureId = idToCreate;
	newStructure.collectionInterval = request.readUint32();
	newStructure.periodicGenerationActionStatus = false;

	uint16_t numOfSimplyCommutatedParams = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParams; i++) {
		uint16_t newParamId = request.readUint16();
		if (existsInVector(newStructure.simplyCommutatedParameterIds, newParamId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
			continue;
		}
		newStructure.simplyCommutatedParameterIds.push_back(newParamId);
	}
	housekeepingStructures.insert({idToCreate, newStructure});
}

void HousekeepingService::deleteHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteHousekeepingReportStructure);
	uint8_t numOfStructuresToDelete = request.readUint8();
	for (uint8_t i = 0; i < numOfStructuresToDelete; i++) {
		uint8_t structureId = request.readUint8();
		if (nonExistingStruct(structureId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		if (periodicGenerationActionStatusGetter(structureId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfEnabledHousekeeping);
			continue;
		}
		housekeepingStructures.erase(structureId);
	}
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::EnablePeriodicHousekeepingParametersReport);

	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToEnable = request.readUint8();
		if (nonExistingStruct(structIdToEnable)) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		periodicGenerationActionStatusSetter(structIdToEnable,true);
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::DisablePeriodicHousekeepingParametersReport);

	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToDisable = request.readUint8();
		if (nonExistingStruct(structIdToDisable)) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		periodicGenerationActionStatusSetter(structIdToDisable, false);
	}
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingStructures);

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		uint8_t structureId = request.readUint8();
		if (nonExistingStruct(structureId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructureReport(structureId);
	}
}

void HousekeepingService::housekeepingStructureReport(uint8_t structIdToReport) {
	auto housekeepingStructure = housekeepingStructures.find(structIdToReport);
	if (housekeepingStructure == housekeepingStructures.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentHousekeeping);
		return;
	}
	Message structReport = createTM(MessageType::HousekeepingStructuresReport);
	structReport.appendUint8(structIdToReport);

	structReport.appendBoolean(housekeepingStructure->second.periodicGenerationActionStatus);
	structReport.appendUint32(housekeepingStructure->second.collectionInterval);
	structReport.appendUint16(housekeepingStructure->second.simplyCommutatedParameterIds.size());

	for (auto parameterId: housekeepingStructure->second.simplyCommutatedParameterIds) {
		structReport.appendUint16(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::housekeepingParametersReport(uint8_t structureId) {
	if (nonExistingStruct(structureId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentHousekeeping);
		return;
	}

	HousekeepingStructure& housekeepingStructure = structGetter(structureId);

	Message housekeepingReport = createTM(MessageType::HousekeepingParametersReport);

	housekeepingReport.appendUint8(structureId);
	for (auto id: housekeepingStructure.simplyCommutatedParameterIds) {
		if (auto parameter = Services.parameterManagement.getParameter(id)) {
			parameter->get().appendValueToMessage(housekeepingReport);
		}
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {
	request.assertTC(ServiceType, MessageType::GenerateOneShotHousekeepingReport);

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		uint8_t structureId = request.readUint8();
		if (nonExistingStruct(structureId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingParametersReport(structureId);
	}
}

void HousekeepingService::appendParametersToHousekeepingStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::AppendParametersToHousekeepingStructure);

	uint8_t targetStructId = request.readUint8();
	if (nonExistingStruct(targetStructId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	auto& housekeepingStructure = structGetter(targetStructId);
	if (housekeepingStructure.periodicGenerationActionStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping);
		return;
	}
	uint16_t numOfSimplyCommutatedParameters = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParameters; i++) {
		if (housekeepingStructure.simplyCommutatedParameterIds.size() >= ECSSMaxSimplyCommutatedParameters) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters);
			return;
		}
		uint16_t newParamId = request.readUint16();
		if (!Services.parameterManagement.parameterExists(newParamId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}
		if (existsInVector(housekeepingStructure.simplyCommutatedParameterIds, newParamId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
			continue;
		}
		housekeepingStructure.simplyCommutatedParameterIds.push_back(newParamId);
	}
}

void HousekeepingService::modifyCollectionIntervalOfStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ModifyCollectionIntervalOfStructures);

	uint8_t numOfTargetStructs = request.readUint8();
	for (uint8_t i = 0; i < numOfTargetStructs; i++) {
		uint8_t targetStructId = request.readUint8();
		uint32_t newCollectionInterval = request.readUint32();
		if (nonExistingStruct(targetStructId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		collectionIntervalSetter(targetStructId, newCollectionInterval);
	}
}

void HousekeepingService::reportHousekeepingPeriodicProperties(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingPeriodicProperties);

	uint8_t numOfValidIds = 0;
	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToReport = request.readUint8();
		if (!nonExistingStruct(structIdToReport)) {
			numOfValidIds++;
		}
	}
	Message periodicPropertiesReport = createTM(MessageType::HousekeepingPeriodicPropertiesReport);
	periodicPropertiesReport.appendUint8(numOfValidIds);
	request.resetRead();
	request.readUint8();

	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToReport = request.readUint8();
		if (nonExistingStruct(structIdToReport)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		appendPeriodicPropertiesToMessage(periodicPropertiesReport, structIdToReport);
	}
	storeMessage(periodicPropertiesReport);
}

void HousekeepingService::appendPeriodicPropertiesToMessage(Message& report, uint8_t structureId) {
	report.appendUint8(structureId);
	report.appendBoolean(periodicGenerationActionStatusGetter(structureId));
	report.appendUint32(collectionIntervalGetter(structureId));
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
                                         uint16_t parameterId) {
	return std::find(std::begin(ids), std::end(ids), parameterId) != std::end(ids);
}

uint32_t
HousekeepingService::reportPendingStructures(uint32_t currentTime, uint32_t previousTime, uint32_t expectedDelay) {
	uint32_t nextCollection = std::numeric_limits<uint32_t>::max();

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
		uint32_t structureTimeToCollection = housekeepingStructure.second.collectionInterval -
		                                     currentTime % housekeepingStructure.second.collectionInterval;
		if (nextCollection > structureTimeToCollection) {
			nextCollection = structureTimeToCollection;
		}
	}

	return nextCollection;
}
