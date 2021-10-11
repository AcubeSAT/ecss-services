#include "Services/HousekeepingService.hpp"

void HousekeepingService::housekeepingParametersReport(Message& structId) {

	ErrorHandler::assertRequest(structId.packetType == Message::TC, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.messageType == MessageType::ReportHousekeepingParameters,
	                            structId, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.serviceType == ServiceType, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message housekeepingReport(ServiceType,MessageType::HousekeepingParametersReport, Message::TM, 1);

	uint16_t requestedId = structId.readUint16();
	if (existingStructIds.find(requestedId) != existingStructIds.end()) {

		housekeepingReport.appendUint16(requestedId);

		// Append data from simply commutated parameters
		for (int i = 0; i < housekeepingStructuresArray[requestedId].numOfSimplyCommutatedParams; i++) {
			uint16_t currentParameterId = housekeepingStructuresArray[requestedId].simplyCommutatedIdsVec.at(i);
			systemStatistics.statisticsArray[currentParameterId].get().getSample(housekeepingReport, 0);
		}

		// Append data from super commutated parameters
		for (int i = 0; i < housekeepingStructuresArray[requestedId].numOfSuperCommutatedParameterSets; i++) {
			uint16_t numOfCurrSetSamples = housekeepingStructuresArray[requestedId].superCommutatedIdsVec.at(i).first;
			uint16_t numOfParamsInCurrSet = housekeepingStructuresArray[requestedId].superCommutatedIdsVec.at(i).second.size();

			for (int j = 0; j < numOfCurrSetSamples; j++) {  // As per note-2 page 81
				for (int k = 0; k < numOfParamsInCurrSet; k++) {

					uint16_t currentParameterId = housekeepingStructuresArray[requestedId].superCommutatedIdsVec.at
					                              (i).second.at(k);
					systemStatistics.statisticsArray[currentParameterId].get().getSample(housekeepingReport, j);
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
	ErrorHandler::assertRequest(request.messageType == MessageType::EnableHousekeepingParametersReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t structIdToEnable = request.readUint16();
	if (existingStructIds.find(structIdToEnable) != existingStructIds.end()) {
		housekeepingStructuresArray[structIdToEnable].periodicGenerationActionStatus = true;
	} else {
		ErrorHandler::reportError(request,ErrorHandler::RequestedNonExistingStructure);
	}
}

void HousekeepingService::disablePeriodicHousekeepingParametersReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DisableHousekeepingParametersReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t structIdToDisable = request.readUint16();
	if (existingStructIds.find(structIdToDisable) != existingStructIds.end()) {
		housekeepingStructuresArray[structIdToDisable].periodicGenerationActionStatus = false;
	} else {
		ErrorHandler::reportError(request,ErrorHandler::RequestedNonExistingStructure);
	}
}

void HousekeepingService::createHousekeepingReportStructure(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::CreateHousekeepingReportStructure,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	HousekeepingStructure newStructure;

	uint16_t idToCreate = request.readUint16();
	if (existingStructIds.find(idToCreate) != existingStructIds.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure);
	} else {
		existingStructIds.insert(idToCreate);
	}

	/**
	 * @todo: Check if the new struct creation exceeds the resources allocated by the host.
	 */

	newStructure.structureId = idToCreate;
	uint16_t requestedInterval = request.readUint16();
	newStructure.collectionInterval = requestedInterval;
	newStructure.periodicGenerationActionStatus = false;

	uint16_t numOfSimplyCommutatedParams = request.readUint16();
	newStructure.numOfSimplyCommutatedParams = numOfSimplyCommutatedParams;

	for (int i = 0; i < numOfSimplyCommutatedParams; i++) {
		uint16_t newParamId = request.readUint16();
		newStructure.simplyCommutatedIdsVec.push_back(newParamId);
		newStructure.containedParameterIds.insert(newParamId);
	}

	int numOfSets = request.readUint16();
	newStructure.numOfSuperCommutatedParameterSets = numOfSets;

	for (int i = 0; i < numOfSets; i++) {
		uint16_t numOfCurrentSetSamples = request.readUint16();
		newStructure.superCommutatedIdsVec[i].first = numOfCurrentSetSamples;
		uint16_t numOfCurrentSetIds = request.readUint16();

		for (int j = 0; j < numOfCurrentSetIds; j++) {
			uint16_t newParamId = request.readUint16();
			newStructure.superCommutatedIdsVec[i].second.push_back(newParamId);
			newStructure.containedParameterIds.insert(newParamId);
		}
	}

	housekeepingStructuresArray[idToCreate] = newStructure;
}

void HousekeepingService::deleteHousekeepingReportStructure(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteHousekeepingReportStructure,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfStructuresToDelete = request.readUint16();
	for (int i = 0; i < numOfStructuresToDelete; i++) {
		uint16_t currStructureId = request.readUint16();
		if (existingStructIds.find(currStructureId) != existingStructIds.end()) {
			if (not housekeepingStructuresArray[currStructureId].periodicGenerationActionStatus) {
				existingStructIds.erase(currStructureId);
			} else {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure);
			}
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::housekeepingStructureReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::HousekeepingStructuresReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message structReport(ServiceType,MessageType::HousekeepingParametersReport, Message::TM, 1);
	uint16_t structIdToReport = request.readUint16();

	if (existingStructIds.find(structIdToReport) != existingStructIds.end()) {

		structReport.appendUint16(structIdToReport);
		if (supportsPeriodicGeneration) {
			structReport.appendBoolean(housekeepingStructuresArray[structIdToReport].periodicGenerationActionStatus);
		}
		structReport.appendUint16(housekeepingStructuresArray[structIdToReport].collectionInterval);
		structReport.appendUint16(housekeepingStructuresArray[structIdToReport].numOfSimplyCommutatedParams);

		for (auto &currParamId : housekeepingStructuresArray[structIdToReport].simplyCommutatedIdsVec) {
			structReport.appendUint16(currParamId);
		}
		structReport.appendUint16(housekeepingStructuresArray[structIdToReport].numOfSuperCommutatedParameterSets);

		for (auto &currSet : housekeepingStructuresArray[structIdToReport].superCommutatedIdsVec) {
			structReport.appendUint16(currSet.first);
			structReport.appendUint16(currSet.second.size());

			for (auto &currParamId : currSet.second) {
				structReport.appendUint16(currParamId);
			}
		}
	} else {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
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
		if (existingStructIds.find(currStructId) != existingStructIds.end()) {
			Message structIdToReport(ServiceType,MessageType::HousekeepingStructuresReport,Message::TC,1);
			structIdToReport.appendUint16(currStructId);
			housekeepingStructureReport(structIdToReport);
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
		if (existingStructIds.find(currStructId) != existingStructIds.end()) {
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
	if (existingStructIds.find(targetStructId) != existingStructIds.end()) {
		if (not housekeepingStructuresArray[targetStructId].periodicGenerationActionStatus) {

			/**
			 * @todo: check if resources allocated by the host are exceeded.
			 */

			// Append simply commutated parameters
			uint16_t numOfSimplyCommParams = newParams.readUint16();
			for (int i = 0; i < numOfSimplyCommParams; i++) {
				uint16_t newParamId = newParams.readUint16();
				if (newParamId < systemParameters.parametersArray.size()) {
					if (housekeepingStructuresArray[targetStructId].containedParameterIds.find(newParamId) ==
					    housekeepingStructuresArray[targetStructId].containedParameterIds.end()) {
						housekeepingStructuresArray[targetStructId].numOfSimplyCommutatedParams++;
						housekeepingStructuresArray[targetStructId].containedParameterIds.insert(newParamId);
						housekeepingStructuresArray[targetStructId].simplyCommutatedIdsVec.push_back(newParamId);
					} else {
						ErrorHandler::reportError(newParams,
						                          ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
					}
				} else {
					ErrorHandler::reportError(newParams,
					                          ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
				}
			}

			// Append super commutated parameters
			uint16_t numOfNewSets = newParams.readUint16();
			for (int i = 0; i < numOfNewSets; i++) {
				uint16_t numOfCurrSetSamples = newParams.readUint16();
				uint16_t numOfCurrSetIds = newParams.readUint16();
				housekeepingStructuresArray[targetStructId].numOfSuperCommutatedParameterSets++;
				etl::vector<uint16_t, ECSS_MAX_PARAMETERS> currSetIdsVec;

				for (int j = 0; j < numOfCurrSetIds; j++) {
					uint16_t newParamId = newParams.readUint16();
					if (newParamId < systemParameters.parametersArray.size()) {
						if (housekeepingStructuresArray[targetStructId].containedParameterIds.find(newParamId) ==
						    housekeepingStructuresArray[targetStructId].containedParameterIds.end()) {
							housekeepingStructuresArray[targetStructId].containedParameterIds.insert(newParamId);
							currSetIdsVec.push_back(newParamId);
						} else {
							ErrorHandler::reportError(newParams,
							                          ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter);
						}
					} else {
						ErrorHandler::reportError(newParams,
						                          ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter);
					}
				}
				housekeepingStructuresArray[targetStructId].superCommutatedIdsVec.push_back(
				    std::make_pair(numOfCurrSetSamples, currSetIdsVec));
			}
		} else {
			ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::RequestedAppendToPeriodicStructure);
		}
	} else {
		ErrorHandler::reportError(newParams,ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
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
		if (existingStructIds.find(targetStructId) != existingStructIds.end()) {
			uint16_t newCollectionInterval = request.readUint16();
			housekeepingStructuresArray[targetStructId].collectionInterval = newCollectionInterval;
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
		if (existingStructIds.find(structIdToReport) != existingStructIds.end()) {
			periodicPropertiesReport.appendUint16(structIdToReport);
			periodicPropertiesReport.appendBoolean(housekeepingStructuresArray[structIdToReport].periodicGenerationActionStatus);
			periodicPropertiesReport.appendUint16(housekeepingStructuresArray[structIdToReport].collectionInterval);
			numOfValidIds++;
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}

	periodicPropertiesReport.resetRead();
	periodicPropertiesReport.appendUint16(numOfValidIds);
	storeMessage(periodicPropertiesReport);

}
