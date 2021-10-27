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

void HousekeepingService::housekeepingParametersReport(Message& structId) {

	ErrorHandler::assertRequest(structId.packetType == Message::TC, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.messageType == MessageType::ReportHousekeepingParameters,
	                            structId, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.serviceType == ServiceType, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message housekeepingReport(ServiceType,MessageType::HousekeepingParametersReport, Message::TM, 1);

	uint16_t requestedId = structId.readUint16();
	if (housekeepingStructures.find(requestedId) != housekeepingStructures.end()) {

		housekeepingReport.appendUint16(requestedId);

		// Append data from simply commutated parameters
		for (auto &currentParameterId : housekeepingStructures.at(requestedId).simplyCommutatedIds) {
			systemHousekeeping.housekeepingParameters.at(currentParameterId).get().appendSampleToMessage
			    (housekeepingReport,0);
		}

		// Append data from super commutated parameters
		for (auto &currentSet : housekeepingStructures.at(requestedId).superCommutatedIds) {
			uint16_t numOfCurrSetSamples = currentSet.first;
			uint16_t numOfParamsInCurrSet = currentSet.second.size();

			for (int j = 0; j < numOfCurrSetSamples; j++) {  // As per 6.3.3.3(c) from the ECSS standard
				for (int k = 0; k < numOfParamsInCurrSet; k++) {

					uint16_t currentParameterId = currentSet.second.at(k);
					systemHousekeeping.housekeepingParameters.at(currentParameterId).get().appendSampleToMessage
					    (housekeepingReport, j);
				}
			}
		}
	} else {
		ErrorHandler::reportError(structId, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnablePeriodicHousekeepingParametersReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToEnable = request.readUint16();
		if (housekeepingStructures.find(structIdToEnable) != housekeepingStructures.end()) {
			housekeepingStructures.at(structIdToEnable).periodicGenerationActionStatus = true;
		} else {
			ErrorHandler::reportError(request,ErrorHandler::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DisablePeriodicHousekeepingParametersReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructIds = request.readUint16();
	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToDisable = request.readUint16();
		if (housekeepingStructures.find(structIdToDisable) != housekeepingStructures.end()) {
			housekeepingStructures.at(structIdToDisable).periodicGenerationActionStatus = false;
		} else {
			ErrorHandler::reportError(request,ErrorHandler::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::createHousekeepingReportStructure(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::CreateHousekeepingReportStructure,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t idToCreate = request.readUint16();
	if (housekeepingStructures.find(idToCreate) != housekeepingStructures.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
		return;
	}

	HousekeepingStructure newStructure;
	newStructure.simplyCommutatedIds = {};
	newStructure.superCommutatedIds = {};

	/**
	 * @todo: Check if the new struct creation exceeds the resources allocated by the host.
	 */

	newStructure.structureId = idToCreate;
	newStructure.collectionInterval = request.readUint16();
	newStructure.periodicGenerationActionStatus = false;

	uint16_t numOfSimplyCommutatedParams = request.readUint16();
	newStructure.numOfSimplyCommutatedParams = numOfSimplyCommutatedParams;

	for (uint16_t i = 0; i < numOfSimplyCommutatedParams; i++) {
		uint16_t newParamId = request.readUint16();
		newStructure.simplyCommutatedIds.push_back(newParamId);
		newStructure.containedParameterIds.push_back(newParamId);
	}

	auto numOfSuperSets = request.readUint16();
	newStructure.numOfSuperCommutatedParameterSets = numOfSuperSets;

	for (int i = 0; i < numOfSuperSets; i++) {
		uint16_t numOfCurrentSetSamples = request.readUint16();
		etl::vector <uint16_t, ECSS_MAX_PARAMETERS> ids;
		newStructure.superCommutatedIds.push_back(std::make_pair(numOfCurrentSetSamples, ids));
		uint16_t numOfCurrentSetIds = request.readUint16();

		for (int j = 0; j < numOfCurrentSetIds; j++) {
			uint16_t newParamId = request.readUint16();
			newStructure.superCommutatedIds[i].second.push_back(newParamId);
			newStructure.containedParameterIds.push_back(newParamId);
		}
	}
	housekeepingStructures.insert({idToCreate, newStructure});
}

void HousekeepingService::deleteHousekeepingReportStructure(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteHousekeepingReportStructure,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructuresToDelete = request.readUint16();
	for (int i = 0; i < numOfStructuresToDelete; i++) {
		uint16_t currStructureId = request.readUint16();
		if (housekeepingStructures.find(currStructureId) == housekeepingStructures.end()) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
			continue;
		}
		if (housekeepingStructures.at(currStructureId).periodicGenerationActionStatus) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure);
			continue;
		}
		housekeepingStructures.erase(currStructureId);
	}
}

void HousekeepingService::housekeepingStructureReport(uint16_t structIdToReport) {

	Message structReport(ServiceType,MessageType::HousekeepingParametersReport, Message::TM, 1);

	structReport.appendUint16(structIdToReport);
	structReport.appendBoolean(housekeepingStructures.at(structIdToReport).periodicGenerationActionStatus);
	structReport.appendUint16(housekeepingStructures.at(structIdToReport).collectionInterval);
	structReport.appendUint16(housekeepingStructures.at(structIdToReport).numOfSimplyCommutatedParams);

	for (auto currParamId : housekeepingStructures.at(structIdToReport).simplyCommutatedIds) {
		structReport.appendUint16(currParamId);
	}
	structReport.appendUint16(housekeepingStructures.at(structIdToReport).numOfSuperCommutatedParameterSets);

	for (auto &currSet : housekeepingStructures.at(structIdToReport).superCommutatedIds) {
		structReport.appendUint16(currSet.first);
		structReport.appendUint16(currSet.second.size());

		for (auto &currParamId : currSet.second) {
			structReport.appendUint16(currParamId);
		}
	}
	storeMessage(structReport);
}

void HousekeepingService::reportHousekeepingStructures(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportHousekeepingStructures,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint16_t currStructId = request.readUint16();
		if (housekeepingStructures.find(currStructId) != housekeepingStructures.end()) {
			housekeepingStructureReport(currStructId);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::generateOneShotHousekeepingReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::GenerateOneShotHousekeepingReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructsToReport = request.readUint16();
	for (int i = 0; i < numOfStructsToReport; i++) {
		uint16_t currStructId = request.readUint16();
		if (housekeepingStructures.find(currStructId) != housekeepingStructures.end()) {
			Message structIdToReport(ServiceType,MessageType::ReportHousekeepingParameters,Message::TC,1);
			structIdToReport.appendUint16(currStructId);
			housekeepingParametersReport(structIdToReport);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::appendParametersToHousekeepingStructure(Message& newParams) {

	ErrorHandler::assertRequest(newParams.packetType == Message::TC, newParams,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParams.messageType == MessageType::AppendParametersToHousekeepingStructure,
	                            newParams, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParams.serviceType == ServiceType, newParams,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t targetStructId = newParams.readUint16();
	if (housekeepingStructures.find(targetStructId) == housekeepingStructures.end()) {
		ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		return;
	}
	if (housekeepingStructures.at(targetStructId).periodicGenerationActionStatus) {
	    ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::RequestedAppendToPeriodicStructure);
		return;
    }

	/**
	 * @todo: check if resources allocated by the host are exceeded.
	 */

	// Append simply commutated parameters
	uint16_t numOfSimplyCommParams = newParams.readUint16();
	for (int i = 0; i < numOfSimplyCommParams; i++) {
		uint16_t newParamId = newParams.readUint16();

		if (newParamId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
			return;
		}
		if (existsInVector(newParamId, housekeepingStructures.at(targetStructId).containedParameterIds)) {
		    ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
			return;
	    }
		housekeepingStructures.at(targetStructId).numOfSimplyCommutatedParams++;
		housekeepingStructures.at(targetStructId).containedParameterIds.push_back(newParamId);
		housekeepingStructures.at(targetStructId).simplyCommutatedIds.push_back(newParamId);
	}

	// Append super commutated parameters
	uint16_t numOfNewSets = newParams.readUint16();
	for (int i = 0; i < numOfNewSets; i++) {
		uint16_t numOfCurrSetSamples = newParams.readUint16();
		uint16_t numOfCurrSetIds = newParams.readUint16();
		housekeepingStructures.at(targetStructId).numOfSuperCommutatedParameterSets++;
		etl::vector<uint16_t, ECSS_MAX_PARAMETERS> currSetIdsVec;
		uint16_t validIdsCounter = 0;

		for (int j = 0; j < numOfCurrSetIds; j++) {
			uint16_t newParamId = newParams.readUint16();
			if (newParamId >= systemParameters.parametersArray.size()) {
				ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
				return;
			}
			if (existsInVector(newParamId, housekeepingStructures.at(targetStructId).containedParameterIds)) {
				ErrorHandler::reportError(newParams, ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
				return;
			}
			housekeepingStructures.at(targetStructId).containedParameterIds.push_back(newParamId);
			currSetIdsVec.push_back(newParamId);
			validIdsCounter++;
		}
		if (validIdsCounter > 0) {  // There must be at least one new added ID in order to insert new set.
			housekeepingStructures.at(targetStructId).superCommutatedIds.push_back(std::make_pair
			                                                                             (numOfCurrSetSamples,
			                                                                         currSetIdsVec));
		}
	}
}

void HousekeepingService::modifyCollectionIntervalOfStructures(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ModifyCollectionIntervalOfStructures,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfTargetStructs = request.readUint16();
	for (int i = 0; i < numOfTargetStructs; i++) {
		uint16_t targetStructId = request.readUint16();
		if (housekeepingStructures.find(targetStructId) != housekeepingStructures.end()) {
			uint16_t newCollectionInterval = request.readUint16();
			housekeepingStructures.at(targetStructId).collectionInterval = newCollectionInterval;
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::housekeepingPeriodicPropertiesReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportHousekeepingPeriodicProperties,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message periodicPropertiesReport(ServiceType,MessageType::HousekeepingPeriodicPropertiesReport, Message::TM, 1);

	uint16_t numOfValidIds = 0;
	uint16_t numOfStructIds = request.readUint16();
	periodicPropertiesReport.appendUint16(numOfStructIds);

	for (int i = 0; i < numOfStructIds; i++) {
		uint16_t structIdToReport = request.readUint16();
		if (housekeepingStructures.find(structIdToReport) != housekeepingStructures.end()) {
			periodicPropertiesReport.appendUint16(structIdToReport);
			periodicPropertiesReport.appendBoolean(housekeepingStructures.at(structIdToReport).periodicGenerationActionStatus);
			periodicPropertiesReport.appendUint16(housekeepingStructures.at(structIdToReport).collectionInterval);
			numOfValidIds++;
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}

	periodicPropertiesReport.resetRead();
	periodicPropertiesReport.appendUint16(numOfValidIds);
	storeMessage(periodicPropertiesReport);
}

bool HousekeepingService::existsInVector(uint16_t targetId, etl::vector <uint16_t, 50> vec) {

	for (auto &it : vec) {
		if (it == targetId) {
			return true;
		}
	}
	return false;
}

void HousekeepingService::execute(Message& message) {
	switch (message.messageType) {
		case 0:
			housekeepingParametersReport(message);
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
		case 25:
			housekeepingParametersReport(message);
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
			housekeepingPeriodicPropertiesReport(message);
			break;
	}
}