#include "Services/HousekeepingService.hpp"

void HousekeepingService :: housekeepingParameterReport(Message& structId) {

	Message housekeepingReport(HousekeepingService::ServiceType,
	                         HousekeepingService::MessageType::HousekeepingParameterReport, Message::TM, 1);

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
	}
	else {
		ErrorHandler::reportError(structId, ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure);
	}
	storeMessage(housekeepingReport);
}
