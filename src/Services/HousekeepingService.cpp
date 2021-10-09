
#include "Services/HousekeepingService.hpp"

void HousekeepingService::housekeepingParametersReport(Message& structId) {

	ErrorHandler::assertRequest(structId.packetType == Message::TC, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.messageType == MessageType::ReportHousekeepingParameters,
	                            structId, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(structId.serviceType == ServiceType, structId,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message housekeepingReport(HousekeepingService::ServiceType,
	                         HousekeepingService::MessageType::HousekeepingParametersReport, Message::TM, 1);

	uint16_t requestedId = structId.readUint16();
	if (requestedId <= structureIdCounter) {    //Check if structure with requested ID exists

		housekeepingReport.appendUint16(requestedId);

		// Append data from simply commutated parameters
		for (int i = 0; i < housekeepingStructuresArray[requestedId].simplyCommutatedIdsVec.size(); i++) {

			uint16_t currentParameterId = housekeepingStructuresArray[requestedId].simplyCommutatedIdsVec.at(i);
			std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[currentParameterId].get();
			currentStatistic.get().getSample(housekeepingReport, 0);
		}

		// Append data from super commutated parameters (OH MY FREAKING GOD !!!)
		for (int i = 0; i < housekeepingStructuresArray[requestedId].superCommutatedIdsVec.size(); i++) {

			uint16_t currentParameterChunkSampleCounter = housekeepingStructuresArray[requestedId]
			                                                  .superCommutatedIdsVec.at(i).first;
			uint16_t numOfParametersInChunk = housekeepingStructuresArray[requestedId].superCommutatedIdsVec.at(i)
			                                      .second.size();
			for (int j = 0; j < currentParameterChunkSampleCounter; j++) {  // As per note-2 page 81
				for (int k = 0; k < numOfParametersInChunk; k++) {

					uint16_t currentParameterId = housekeepingStructuresArray[requestedId].superCommutatedIdsVec.at
					                              (i).second.at(k);
					std::reference_wrapper <StatisticBase> currentStatistic = systemStatistics.statisticsArray[currentParameterId].get();
					currentStatistic.get().getSample(housekeepingReport, j);
				}
			}
		}
	} else {
		ErrorHandler::reportError(structId, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
	}
	storeMessage(housekeepingReport);
}

void HousekeepingService::periodicHousekeepingParameterReport() {

	while (1) {

		for (int i = 0; i < housekeepingStructuresArray.size(); i++) {
			uint16_t currStructureId = i;
			if (housekeepingStructuresArray[currStructureId].periodicGenerationActionStatus) {

				Message structId(ServiceType,MessageType::ReportHousekeepingParameters,Message::TC,1);
				housekeepingParametersReport(structId);
			}
		}
		//TODO: here suspend the task for time = collectionInterval. This could be done outside of the function as well.
	}
}

void HousekeepingService::enablePeriodicHousekeepingParametersReport(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnableHousekeepingParametersReport,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t structIdToEnable = request.readUint16();
	if (structIdToEnable <= structureIdCounter) {
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

	uint16_t structIdToEnable = request.readUint16();
	if (structIdToEnable <= structureIdCounter) {
		housekeepingStructuresArray[structIdToEnable].periodicGenerationActionStatus = false;
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
	newStructure.structureId = idToCreate;
	uint16_t requestedInterval = request.readUint16();
	newStructure.collectionInterval = requestedInterval;

	uint16_t numOfSimplyCommutatedParams = request.readUint16();
	newStructure.numOfSimplyCommutatedParams = numOfSimplyCommutatedParams;

	for (int i = 0; i < numOfSimplyCommutatedParams; i++) {
		uint16_t newParamId = request.readUint16();
		newStructure.simplyCommutatedIdsVec.push_back(newParamId);
		newStructure.containedParametersIdVec.push_back(newParamId);
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
			newStructure.containedParametersIdVec.push_back(newParamId);
		}
	}

	housekeepingStructuresArray[idToCreate] = newStructure;
}

