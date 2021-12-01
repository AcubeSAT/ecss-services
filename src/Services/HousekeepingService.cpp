#include "Services/HousekeepingService.hpp"

void HousekeepingService::reportHousekeepingParameters(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingParameters);
	uint8_t structureId = request.readUint8();
	if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	housekeepingParametersReport(structureId);
}

void HousekeepingService::housekeepingParametersReport(uint8_t structureId) {
	Message housekeepingReport(ServiceType, MessageType::HousekeepingParametersReport, Message::TM, 1);
	housekeepingReport.appendUint8(structureId);
	for (const auto& parameter : housekeepingStructures.at(structureId).simplyCommutatedParameters) {
		parameter.second.get().appendValueToMessage(housekeepingReport);
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::EnablePeriodicHousekeepingParametersReport);

	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToEnable = request.readUint8();
		if (housekeepingStructures.find(structIdToEnable) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(structIdToEnable).periodicGenerationActionStatus = true;
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::DisablePeriodicHousekeepingParametersReport);

	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToDisable = request.readUint8();
		if (housekeepingStructures.find(structIdToDisable) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(structIdToDisable).periodicGenerationActionStatus = false;
	}
}

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure);

	uint8_t idToCreate = request.readUint8();
	if (housekeepingStructures.find(idToCreate) != housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return;
	}
	HousekeepingStructure newStructure;
	/**
	 * @todo: Check if the new struct creation exceeds the resources allocated by the host.
	 */
	newStructure.structureId = idToCreate;
	newStructure.collectionInterval = request.readUint32();
	newStructure.periodicGenerationActionStatus = false;

	uint16_t numOfSimplyCommutatedParams = request.readUint16();

	for (uint16_t i = 0; i < numOfSimplyCommutatedParams; i++) {
		uint16_t newParamId = request.readUint16();
		newStructure.simplyCommutatedParameters.insert({newParamId, systemParameters.getParameter(newParamId)});
	}
	housekeepingStructures.insert({idToCreate, newStructure});
}

void HousekeepingService::deleteHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteHousekeepingReportStructure);
	uint16_t numOfStructuresToDelete = request.readUint16();
	for (int i = 0; i < numOfStructuresToDelete; i++) {
		uint8_t structureId = request.readUint8();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		if (housekeepingStructures.at(structureId).periodicGenerationActionStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure);
			continue;
		}
		housekeepingStructures.erase(structureId);
	}
}

void HousekeepingService::housekeepingStructureReport(uint8_t structIdToReport) {
	Message structReport(ServiceType, MessageType::HousekeepingStructuresReport, Message::TM, 1);

	structReport.appendUint8(structIdToReport);
	auto housekeepingStructure = housekeepingStructures.find(structIdToReport);
	assert(housekeepingStructure != housekeepingStructures.end());

	structReport.appendBoolean(housekeepingStructure->second.periodicGenerationActionStatus);
	structReport.appendUint32(housekeepingStructure->second.collectionInterval);
	structReport.appendUint16(housekeepingStructure->second.simplyCommutatedParameters.size());
	for (auto& parameter : housekeepingStructure->second.simplyCommutatedParameters) {
		uint16_t parameterId = parameter.first;
		structReport.appendUint16(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingStructures);

	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint8_t structureId = request.readUint8();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructureReport(structureId);
	}
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {
	request.assertTC(ServiceType, MessageType::GenerateOneShotHousekeepingReport);
	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint8_t structureId = request.readUint8();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingParametersReport(structureId);
	}
}

void HousekeepingService::appendParametersToHousekeepingStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::AppendParametersToHousekeepingStructure);

	uint8_t targetStructId = request.readUint8();
	if (housekeepingStructures.find(targetStructId) == housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	if (housekeepingStructures.at(targetStructId).periodicGenerationActionStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping);
		return;
	}
	/**
	 * @todo: check if resources allocated by the host are exceeded.
	 */
	uint16_t numOfSimplyCommParams = request.readUint16();
	for (int i = 0; i < numOfSimplyCommParams; i++) {
		uint16_t newParamId = request.readUint16();
		if (newParamId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}
		auto& housekeepingStructure = housekeepingStructures.at(targetStructId);
		if (housekeepingStructure.simplyCommutatedParameters.find(newParamId) !=
		    housekeepingStructure.simplyCommutatedParameters.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
			continue;
		}
		housekeepingStructure.simplyCommutatedParameters.insert(
		    {newParamId, systemParameters.getParameter(newParamId)});
	}
}

void HousekeepingService::modifyCollectionIntervalOfStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ModifyCollectionIntervalOfStructures);

	uint16_t numOfTargetStructs = request.readUint16();
	for (int i = 0; i < numOfTargetStructs; i++) {
		uint8_t targetStructId = request.readUint8();
		uint32_t newCollectionInterval = request.readUint32();
		if (housekeepingStructures.find(targetStructId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(targetStructId).collectionInterval = newCollectionInterval;
	}
}

void HousekeepingService::reportHousekeepingPeriodicProperties(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingPeriodicProperties);

	Message periodicPropertiesReport(ServiceType, MessageType::HousekeepingPeriodicPropertiesReport, Message::TM, 1);
	uint16_t numOfValidIds = 0;
	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToReport = request.readUint8();
		if (housekeepingStructures.find(structIdToReport) != housekeepingStructures.end()) {
			numOfValidIds++;
		}
	}
	periodicPropertiesReport.appendUint16(numOfValidIds);
	request.resetRead();
	request.readUint16();

	for (int i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToReport = request.readUint8();
		if (housekeepingStructures.find(structIdToReport) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		appendPeriodicPropertiesToMessage(periodicPropertiesReport, structIdToReport);
	}
	storeMessage(periodicPropertiesReport);
}

void HousekeepingService::appendPeriodicPropertiesToMessage(Message& report, uint8_t structureId) {
	report.appendUint8(structureId);
	report.appendBoolean(housekeepingStructures.at(structureId).periodicGenerationActionStatus);
	report.appendUint32(housekeepingStructures.at(structureId).collectionInterval);
}

void HousekeepingService::execute(Message& message) {
	switch (message.messageType) {
		case 0:
			reportHousekeepingParameters(message);
			break;
		case 1:
			createHousekeepingReportStructure(message);
			break;
		case 3:
			deleteHousekeepingReportStructure(message);
			break;
		case 5:
			enablePeriodicHousekeepingParametersReport(message);
			break;
		case 6:
			disablePeriodicHousekeepingParametersReport(message);
			break;
		case 9:
			reportHousekeepingStructures(message);
			break;
		case 27:
			generateOneShotHousekeepingReport(message);
			break;
		case 29:
			appendParametersToHousekeepingStructure(message);
			break;
		case 31:
			modifyCollectionIntervalOfStructures(message);
			break;
		case 33:
			reportHousekeepingPeriodicProperties(message);
			break;
	}
}