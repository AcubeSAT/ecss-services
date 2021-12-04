#include <iostream>
#include "Services/HousekeepingService.hpp"

void HousekeepingService::createHousekeepingReportStructure(Message& request) {
	request.assertTC(ServiceType, MessageType::CreateHousekeepingReportStructure);

	uint8_t idToCreate = request.readUint8();
	if (housekeepingStructures.find(idToCreate) != housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return;
	}
	if (housekeepingStructures.size() >= ECSS_MAX_HOUSEKEEPING_STRUCTS) {
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
		if (std::find(std::begin(newStructure.simplyCommutatedParameterIds),
		              std::end(newStructure.simplyCommutatedParameterIds),
		              newParamId) != std::end(newStructure.simplyCommutatedParameterIds)) {
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
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		if (housekeepingStructures.at(structureId).periodicGenerationActionStatus) {
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
		if (housekeepingStructures.find(structIdToEnable) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(structIdToEnable).periodicGenerationActionStatus = true;
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {
	request.assertTC(ServiceType, MessageType::DisablePeriodicHousekeepingParametersReport);

	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToDisable = request.readUint8();
		if (housekeepingStructures.find(structIdToDisable) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructures.at(structIdToDisable).periodicGenerationActionStatus = false;
	}
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingStructures);

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
		uint8_t structureId = request.readUint8();
		if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		housekeepingStructureReport(structureId);
	}
}

void HousekeepingService::housekeepingStructureReport(uint8_t structIdToReport) {
	Message structReport(ServiceType, MessageType::HousekeepingStructuresReport, Message::TM, 1);

	structReport.appendUint8(structIdToReport);
	auto housekeepingStructure = housekeepingStructures.find(structIdToReport);
	if (housekeepingStructure == housekeepingStructures.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentHousekeeping);
		return;
	}
	structReport.appendBoolean(housekeepingStructure->second.periodicGenerationActionStatus);
	structReport.appendUint32(housekeepingStructure->second.collectionInterval);
	structReport.appendUint16(housekeepingStructure->second.simplyCommutatedParameterIds.size());

	for (auto parameterId : housekeepingStructure->second.simplyCommutatedParameterIds) {
		structReport.appendUint16(parameterId);
	}
	storeMessage(structReport);
}

void HousekeepingService::housekeepingParametersReport(uint8_t structureId) {
	if (housekeepingStructures.find(structureId) == housekeepingStructures.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentHousekeeping);
		return;
	}
	Message housekeepingReport(ServiceType, MessageType::HousekeepingParametersReport, Message::TM, 1);

	housekeepingReport.appendUint8(structureId);
	for (auto& id : housekeepingStructures.at(structureId).simplyCommutatedParameterIds) {
		systemParameters.getParameter(id).appendValueToMessage(housekeepingReport);
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {
	request.assertTC(ServiceType, MessageType::GenerateOneShotHousekeepingReport);

	uint8_t numOfStructsToReport = request.readUint8();
	for (uint8_t i = 0; i < numOfStructsToReport; i++) {
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

	uint16_t numOfSimplyCommParams = request.readUint16();
	for (uint16_t i = 0; i < numOfSimplyCommParams; i++) {
		if (housekeepingStructures.at(targetStructId).simplyCommutatedParameterIds.size() >=
		    ECSS_MAX_SIMPLY_COMMUTATED_PARAMETERS) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters);
			return;
		}
		uint16_t newParamId = request.readUint16();
		if (newParamId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			continue;
		}
		auto& housekeepingStructure = housekeepingStructures.at(targetStructId);
		if (std::find(std::begin(housekeepingStructure.simplyCommutatedParameterIds),
		              std::end(housekeepingStructure.simplyCommutatedParameterIds),
		              newParamId) != std::end(housekeepingStructure.simplyCommutatedParameterIds)) {
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
	uint8_t numOfValidIds = 0;
	uint8_t numOfStructIds = request.readUint8();
	for (uint8_t i = 0; i < numOfStructIds; i++) {
		uint8_t structIdToReport = request.readUint8();
		if (housekeepingStructures.find(structIdToReport) != housekeepingStructures.end()) {
			numOfValidIds++;
		}
	}
	periodicPropertiesReport.appendUint8(numOfValidIds);
	request.resetRead();
	request.readUint8();

	for (uint8_t i = 0; i < numOfStructIds; i++) {
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