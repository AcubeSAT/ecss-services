#include "Services/HousekeepingService.hpp"

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
		for (auto &currentParameterId : housekeepingStructures[requestedId].simplyCommutatedIds) {
			systemStatistics.statisticsArray[currentParameterId].get().appendSampleToMessage(housekeepingReport, 0);
		}

		// Append data from super commutated parameters
		for (auto &currentSet : housekeepingStructures[requestedId].superCommutatedIds) {
			uint16_t numOfCurrSetSamples = currentSet.first;
			uint16_t numOfParamsInCurrSet = currentSet.second.size();

			for (int j = 0; j < numOfCurrSetSamples; j++) {  // As per 6.3.3.3(c) from the ECSS standard
				for (int k = 0; k < numOfParamsInCurrSet; k++) {

					uint16_t currentParameterId = currentSet.second.at(k);
					systemStatistics.statisticsArray[currentParameterId].get().appendSampleToMessage(housekeepingReport, j);
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
			housekeepingStructures[structIdToEnable].periodicGenerationActionStatus = true;
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
			housekeepingStructures[structIdToDisable].periodicGenerationActionStatus = false;
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
	}

	HousekeepingStructure newStructure;

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
		newStructure.superCommutatedIds[i].first = numOfCurrentSetSamples;
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
		if (housekeepingStructures.find(currStructureId) != housekeepingStructures.end()) {
			if (not housekeepingStructures[currStructureId].periodicGenerationActionStatus) {
				housekeepingStructures.erase(currStructureId);
			} else {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure);
			}
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}
}

void HousekeepingService::housekeepingStructureReport(uint16_t structIdToReport) {

	Message structReport(ServiceType,MessageType::HousekeepingParametersReport, Message::TM, 1);

	if (housekeepingStructures.find(structIdToReport) != housekeepingStructures.end()) {

		structReport.appendUint16(structIdToReport);
		structReport.appendBoolean(housekeepingStructures[structIdToReport].periodicGenerationActionStatus);

		structReport.appendUint16(housekeepingStructures[structIdToReport].collectionInterval);
		structReport.appendUint16(housekeepingStructures[structIdToReport].numOfSimplyCommutatedParams);

		for (auto currParamId : housekeepingStructures[structIdToReport].simplyCommutatedIds) {
			structReport.appendUint16(currParamId);
		}
		structReport.appendUint16(housekeepingStructures[structIdToReport].numOfSuperCommutatedParameterSets);

		for (auto &currSet : housekeepingStructures[structIdToReport].superCommutatedIds) {
			structReport.appendUint16(currSet.first);
			structReport.appendUint16(currSet.second.size());

			for (auto &currParamId : currSet.second) {
				structReport.appendUint16(currParamId);
			}
		}
	} else {
		/**
		 * @todo: Raise an error (RequestedNonExistingParameter)
		 */
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
	if (housekeepingStructures.find(targetStructId) != housekeepingStructures.end()) {
		if (not housekeepingStructures[targetStructId].periodicGenerationActionStatus) {

			/**
			 * @todo: check if resources allocated by the host are exceeded.
			 */

			// Append simply commutated parameters
			uint16_t numOfSimplyCommParams = newParams.readUint16();
			for (int i = 0; i < numOfSimplyCommParams; i++) {
				uint16_t newParamId = newParams.readUint16();
				if (newParamId < systemParameters.parametersArray.size()) {
					if (existsInVector(newParamId, housekeepingStructures[targetStructId].containedParameterIds)) {
						housekeepingStructures[targetStructId].numOfSimplyCommutatedParams++;
						housekeepingStructures[targetStructId].containedParameterIds.push_back(newParamId);
						housekeepingStructures[targetStructId].simplyCommutatedIds.push_back(newParamId);
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
				housekeepingStructures[targetStructId].numOfSuperCommutatedParameterSets++;
				etl::vector<uint16_t, ECSS_MAX_PARAMETERS> currSetIdsVec;

				for (int j = 0; j < numOfCurrSetIds; j++) {
					uint16_t newParamId = newParams.readUint16();
					if (newParamId < systemParameters.parametersArray.size()) {
						if (existsInVector(newParamId, housekeepingStructures[targetStructId].containedParameterIds)) {
							housekeepingStructures[targetStructId].containedParameterIds.push_back(newParamId);
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
				housekeepingStructures[targetStructId].superCommutatedIds.push_back(
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
		if (housekeepingStructures.find(targetStructId) != housekeepingStructures.end()) {
			uint16_t newCollectionInterval = request.readUint16();
			housekeepingStructures[targetStructId].collectionInterval = newCollectionInterval;
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
			periodicPropertiesReport.appendBoolean(housekeepingStructures[structIdToReport]
			                                           .periodicGenerationActionStatus);
			periodicPropertiesReport.appendUint16(housekeepingStructures[structIdToReport].collectionInterval);
			numOfValidIds++;
		} else {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
		}
	}

	periodicPropertiesReport.resetRead();
	periodicPropertiesReport.appendUint16(numOfValidIds);
	storeMessage(periodicPropertiesReport);

}

bool HousekeepingService::existsInVector(uint16_t targetId, etl::vector <uint16_t, 5> vec) {

	for (auto &it : vec) {
		if (it == targetId) {
			return true;
		}
	}
	return false;
}
