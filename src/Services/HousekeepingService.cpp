#include <iostream>
#include "Services/HousekeepingService.hpp"

/**
 * TODO:
 * 		for (every minimum sampling_interval):
 * 			for (every Parameter ID):
 * 				ecss.st20.getValueAsDouble();
 * 				ecss.st04.storeStatistics();
 * 				if (ID is a housekeeping parameter):	<--- Should we do it this way?
 * 					ecss.st03.storeValueInSystemHousekeeping();
 */

void HousekeepingService::reportHousekeepingParameters(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingParameters);
	uint16_t structureId = request.readUint16();
	if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	housekeepingParametersReport(structureId);
}

void HousekeepingService::housekeepingParametersReport(uint16_t structureId) {
	Message housekeepingReport(ServiceType, MessageType::HousekeepingParametersReport, Message::TM, 1);
	housekeepingReport.appendUint16(structureId);
	for (auto& parameter : housekeepingStructures.at(structureId).simplyCommutatedParameters) {
		parameter.second.get().appendValueToMessage(housekeepingReport);
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::EnablePeriodicHousekeepingParametersReport);

	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToEnable = request.readUint16();
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
		uint16_t structIdToDisable = request.readUint16();
		if (housekeepingStructures.find(structIdToDisable) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(structIdToDisable).periodicGenerationActionStatus = false;
	}
}

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure);
	uint16_t idToCreate = request.readUint16();
	if (housekeepingStructures.find(idToCreate) != housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return;
	}
	HousekeepingStructure newStructure;
	/**
	 * @todo: Check if the new struct creation exceeds the resources allocated by the host.
	 */
	newStructure.structureId = idToCreate;
	newStructure.collectionInterval = request.readUint16();
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
		uint16_t currStructureId = request.readUint16();
		if (housekeepingStructures.find(currStructureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		if (housekeepingStructures.at(currStructureId).periodicGenerationActionStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure);
			continue;
		}
		housekeepingStructures.erase(currStructureId);
	}
}

void HousekeepingService::housekeepingStructureReport(uint16_t structIdToReport) {
	Message structReport(ServiceType, MessageType::HousekeepingStructuresReport, Message::TM, 1);

	structReport.appendUint16(structIdToReport);
	structReport.appendBoolean(housekeepingStructures.at(structIdToReport).periodicGenerationActionStatus);
	structReport.appendUint16(housekeepingStructures.at(structIdToReport).collectionInterval);
	structReport.appendUint16(housekeepingStructures.at(structIdToReport).simplyCommutatedParameters.size());
	for (auto& parameter : housekeepingStructures.at(structIdToReport).simplyCommutatedParameters) {
		uint16_t parameterId = parameter.first;
		structReport.appendUint16(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingStructures);

	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint16_t structureId = request.readUint16();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructureReport(structureId);
	}
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {
	request.assertTC(ServiceType, MessageType::GenerateOneShotHousekeepingReport);
	oneShotHousekeepingReport(request);
}

void HousekeepingService::oneShotHousekeepingReport(Message& request) {
	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint16_t structureId = request.readUint16();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingParametersReport(structureId);
	}
}

void HousekeepingService::appendParametersToHousekeepingStructure(Message& newParams) {
	newParams.assertTC(ServiceType, MessageType::AppendParametersToHousekeepingStructure);

	uint16_t targetStructId = newParams.readUint16();
	if (housekeepingStructures.find(targetStructId) == housekeepingStructures.end()) {
		ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	if (housekeepingStructures.at(targetStructId).periodicGenerationActionStatus) {
		ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::RequestedAppendToPeriodicStructure);
		return;
	}
	/**
	 * @todo: check if resources allocated by the host are exceeded.
	 */
	uint16_t numOfSimplyCommParams = newParams.readUint16();
	for (int i = 0; i < numOfSimplyCommParams; i++) {
		uint16_t newParamId = newParams.readUint16();
		if (newParamId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}
		if (housekeepingStructures.at(targetStructId).simplyCommutatedParameters.find(newParamId) !=
		    housekeepingStructures.at(targetStructId).simplyCommutatedParameters.end()) {
			ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
			continue;
		}
		housekeepingStructures.at(targetStructId)
		    .simplyCommutatedParameters.insert({newParamId, systemParameters.getParameter(newParamId)});
	}
}

void HousekeepingService::modifyCollectionIntervalOfStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ModifyCollectionIntervalOfStructures);
	uint16_t numOfTargetStructs = request.readUint16();
	for (int i = 0; i < numOfTargetStructs; i++) {
		uint16_t targetStructId = request.readUint16();
		uint16_t newCollectionInterval = request.readUint16();
		if (housekeepingStructures.find(targetStructId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(targetStructId).collectionInterval = newCollectionInterval;
	}
}

void HousekeepingService::reportHousekeepingPeriodicProperties(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingPeriodicProperties);
	housekeepingPeriodicPropertiesReport(request);
}

void HousekeepingService::housekeepingPeriodicPropertiesReport(Message& request) {
	Message periodicPropertiesReport(ServiceType, MessageType::HousekeepingPeriodicPropertiesReport, Message::TM, 1);
	uint16_t numOfValidIds = 0;
	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToReport = request.readUint16();
		if (housekeepingStructures.find(structIdToReport) != housekeepingStructures.end()) {
			numOfValidIds++;
		}
	}
	periodicPropertiesReport.appendUint16(numOfValidIds);
	request.resetRead();
	request.readUint16();

	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToReport = request.readUint16();
		if (housekeepingStructures.find(structIdToReport) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		periodicPropertiesReport.appendUint16(structIdToReport);
		periodicPropertiesReport.appendBoolean(
		    housekeepingStructures.at(structIdToReport).periodicGenerationActionStatus);
		periodicPropertiesReport.appendUint16(housekeepingStructures.at(structIdToReport).collectionInterval);
	}
	storeMessage(periodicPropertiesReport);
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