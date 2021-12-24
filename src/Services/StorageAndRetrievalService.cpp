#include <iostream>
#include "Services/StorageAndRetrievalService.hpp"

StorageAndRetrievalService::StorageAndRetrievalService()
    : packetSelectionSubservice(*this, 5, ECSS_MAX_EVENT_DEFINITION_IDS,
                                ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS,
                                ECSS_MAX_SERVICE_TYPE_DEFINITIONS) {
	serviceType = StorageAndRetrievalService::ServiceType;
}

StorageAndRetrievalService::PacketSelectionSubservice::PacketSelectionSubservice(
    StorageAndRetrievalService& parent, uint16_t numOfControlledAppProcs, uint16_t maxEventDefIds,
    uint16_t maxHousekeepingStructIds, uint16_t maxReportTypeDefs, uint16_t maxServiceTypeDefs)
    : mainService(parent), numOfControlledAppProcesses(numOfControlledAppProcs), maxEventDefinitionIds(maxEventDefIds),
      maxHousekeepingStructureIds(maxHousekeepingStructIds), maxReportTypeDefinitions(maxReportTypeDefs),
      maxServiceTypeDefinitions(maxServiceTypeDefs), supportsSubsamplingRate(true) {}

bool StorageAndRetrievalService::PacketSelectionSubservice::appIsControlled(uint16_t applicationId, Message& request) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnControlledApplicationProcessId);
		return false;
	}
	return true;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxReportDefinitions(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId,
    Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	        .serviceTypeDefinitions[serviceId]
	        .size() >= maxReportTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxServiceDefinitions(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.size() >=
	    maxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noReportDefinitionInService(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId,
    Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	        .serviceTypeDefinitions[serviceId]
	        .empty()) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinitionInService);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noServiceDefinitionInApplication(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinitionInApp);
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::addReportDefinition(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId,
    uint16_t reportId) {
	applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	    .serviceTypeDefinitions[serviceId]
	    .push_back(reportId);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addAllReportDefinitionsOfApplication(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId) {
	applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.clear();
}

void StorageAndRetrievalService::PacketSelectionSubservice::addAllReportDefinitionsOfService(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId) {
	applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions[serviceId].clear();
}

bool StorageAndRetrievalService::PacketSelectionSubservice::checkApplicationForReportTypes(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	return (not appIsControlled(applicationId, request) ||
	        noServiceDefinitionInApplication(packetStoreId, applicationId, request) ||
	        exceedsMaxServiceDefinitions(packetStoreId, applicationId, request));
}

bool StorageAndRetrievalService::PacketSelectionSubservice::checkService(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId,
    Message& request) {
	return (noReportDefinitionInService(packetStoreId, applicationId, serviceId, request) ||
	        exceedsMaxReportDefinitions(packetStoreId, applicationId, serviceId, request));
}

int StorageAndRetrievalService::PacketSelectionSubservice::reportExistsInService(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId,
    uint16_t reportId) {
	uint16_t position = 0;
	for (auto& id :
	     applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions[serviceId]) {
		if (id == reportId) {
			return position;
		}
		position++;
	}
	return -1;
}

// bool StorageAndRetrievalService::PacketSelectionSubservice::serviceHasReportDefinitions(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId) {
//	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
//	        .serviceTypeDefinitions[serviceId]
//	        .empty()) {
//		return false;
//	}
//	return true;
//}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteReportDefinitionsOfService(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId, int index) {
	auto& tempList = applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions;

	auto iter = tempList[serviceId].begin() + index;
	tempList[serviceId].erase(iter);

	if (tempList[serviceId].empty()) {
		deleteServiceDefinitionsOfApp(packetStoreId, applicationId, false, serviceId);

		if (tempList.empty()) {
			applicationProcessConfiguration.definitions[packetStoreId].erase(applicationId);
		}
	}
}

// bool StorageAndRetrievalService::PacketSelectionSubservice::appHasServiceDefinitions(
//     String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
//	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
//		return false;
//	}
//	return true;
// }

void StorageAndRetrievalService::PacketSelectionSubservice::deleteServiceDefinitionsOfApp(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, bool deleteAll,
    uint16_t serviceId) {
	if (deleteAll) {
		applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.clear();
	} else {
		applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.erase(
		    serviceId);
		if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
			applicationProcessConfiguration.definitions[packetStoreId].erase(applicationId);
		}
	}
}

bool StorageAndRetrievalService::PacketSelectionSubservice::serviceExistsInApp(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId) {
	auto& serviceTypes =
	    applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions;
	if (serviceTypes.find(serviceId) != serviceTypes.end()) {
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appExistsInApplicationConfiguration(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId) {
	auto& applications = applicationProcessConfiguration.definitions[packetStoreId];
	if (applications.find(applicationId) != applications.end()) {
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::checkApplicationForHousekeeping(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	return (not appIsControlled(applicationId, request) ||
	        noStructureIdsInApplication(packetStoreId, applicationId, request) ||
	        exceedsMaxHousekeepingStructures(packetStoreId, applicationId, request));
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxHousekeepingStructures(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.size() >=
	    maxHousekeepingStructureIds) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxHousekeepingStructureIdsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noStructureIdsInApplication(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, Message& request) {
	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.empty()) {
		ErrorHandler::reportError(
		    request, ErrorHandler::ExecutionStartErrorType::NonExistingHousekeepingStructureIdInDefinition);
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::addHousekeepingStructureId(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint8_t structureId,
    uint16_t subsamplingRate) {
	housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.push_back(
	    structureId);
	housekeepingReportConfiguration.definitions[packetStoreId][applicationId].subsamplingRates.push_back(
	    subsamplingRate);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addHousekeepingStructureId(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint8_t structureId) {
	housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.push_back(
	    structureId);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addAllHousekeepingStructuresOfApplication(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId) {
	housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.clear();
	housekeepingReportConfiguration.definitions[packetStoreId][applicationId].subsamplingRates.clear();
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appExistsInHousekeepingConfiguration(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId) {
	auto& applications = housekeepingReportConfiguration.definitions[packetStoreId];
	if (applications.find(applicationId) != applications.end()) {
		return true;
	}
	return false;
}

int StorageAndRetrievalService::PacketSelectionSubservice::structureIdExistsInApplication(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint8_t structureId) {
	uint16_t position = 0;
	for (auto& id : housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds) {
		if (id == structureId) {
			return position;
		}
		position++;
	}
	return -1;
}

// bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxStructureIds(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
//	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.size() >=
//	    maxHousekeepingStructureIds) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxHousekeepingStructureIdsReached);
//		return true;
//	}
//	return false;
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::noStructureInDefinition(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
//	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.empty()) {
//		ErrorHandler::reportError(
//		    request, ErrorHandler::ExecutionStartErrorType::NonExistingHousekeepingStructureIdInDefinition);
//		return true;
//	}
//	return false;
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::housekeepingDefinitionExists(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
//	if (housekeepingReportConfiguration.definitions[packetStoreId].find(applicationId) !=
//	    housekeepingReportConfiguration.definitions[packetStoreId].end()) {
//		return true;
//	}
//	return false;
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::createHousekeepingDefinition(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
//	etl::vector<std::pair<uint16_t, uint16_t>, ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL> housekeepingStructIds;
//	HousekeepingDefinition newDefinition;
//	newDefinition.housekeepingStructIds = housekeepingStructIds;
//	housekeepingReportConfiguration.definitions[packetStoreId].insert({applicationId, newDefinition});
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::structureExists(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t structureId,
//    uint16_t& index) {
//	uint16_t position = 0;
//	for (auto& id : housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds) {
//		if (id.first == structureId) {
//			index = position;
//			return true;
//		}
//		position++;
//	}
//	return false;
//}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteHousekeepingStructure(
    const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t index) {
	auto& structureIds =
	    housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds;

	auto iterator = structureIds.begin() + index;
	structureIds.erase(iterator);
	if (structureIds.empty()) {
		housekeepingReportConfiguration.definitions[packetStoreId].erase(applicationId);
	}
}

// bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxEventDefinitionIds(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
//	if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.size() >=
//	    maxEventDefinitionIds) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxEventDefinitionIdsReached);
//		return true;
//	}
//	return false;
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::noEventInDefinition(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
//	if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty()) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NoEventDefinitionExistsInApp);
//		return true;
//	}
//	return false;
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::eventBlockingDefinitionExists(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
//	if (eventReportConfiguration.definitions[packetStoreId].find(applicationId) !=
//	    eventReportConfiguration.definitions[packetStoreId].end()) {
//		return true;
//	}
//	return false;
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::createEventReportBlockingDefinition(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
//	etl::vector<uint16_t, ECSS_MAX_EVENT_DEFINITION_IDS> eventDefinitionIds;
//	EventDefinition newDefinition;
//	newDefinition.eventDefinitionIds = eventDefinitionIds;
//	eventReportConfiguration.definitions[packetStoreId].insert({applicationId, newDefinition});
//}
//
// bool StorageAndRetrievalService::PacketSelectionSubservice::eventDefinitionIdExists(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t eventId, uint16_t& index) {
//	uint16_t position = 0;
//	for (auto& id : eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds) {
//		if (id == eventId) {
//			index = position;
//			return true;
//		}
//		position++;
//	}
//	return false;
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::createEventDefinitionId(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t eventId) {
//	eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.push_back(eventId);
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::deleteEventDefinitionIds(
//    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, bool deleteAll, uint16_t index) {
//	if (deleteAll and
//	    (not eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty())) {
//		eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.clear();
//	} else {
//		auto iterator =
//		    eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.begin() + index;
//		eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.erase(iterator);
//		if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty()) {
//			eventReportConfiguration.definitions[packetStoreId].erase(applicationId);
//		}
//	}
//}

String<ECSS_MAX_PACKET_STORE_ID_SIZE>
StorageAndRetrievalService::PacketSelectionSubservice::readPacketStoreId(Message& request) {
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	return packetStoreId;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::packetStoreExists(const String<10>& packetStoreId) {
	if (mainService.packetStores.find(packetStoreId) != mainService.packetStores.end()) {
		return true;
	}
	return false;
}

/**
 * @todo: skip bytes when invalid request.
 */
void StorageAndRetrievalService::PacketSelectionSubservice::addReportTypesToAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();

	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t applicationId = request.readUint16();
		//		if (checkApplicationForReportTypes(packetStoreId, applicationId, request)) {
		//			continue;
		//		}
		uint16_t numOfServiceIds = request.readUint16();

		if (numOfServiceIds == 0) {
			addAllReportDefinitionsOfApplication(packetStoreId, applicationId);
			continue;
		}

		for (int j = 0; j < numOfServiceIds; j++) {
			uint16_t serviceId = request.readUint16();
			//			if (checkService(packetStoreId, applicationId, serviceId, request)) {
			//				continue;
			//			}
			uint16_t numOfMessageTypes = request.readUint16();

			if (numOfMessageTypes == 0) {
				addAllReportDefinitionsOfService(packetStoreId, applicationId, serviceId);
				continue;
			}

			for (int k = 0; k < numOfMessageTypes; k++) {
				uint16_t messageType = request.readUint16();
				addReportDefinition(packetStoreId, applicationId, serviceId, messageType);
			}
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteReportTypesFromAppProcessConfiguration(
    Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteReportTypesFromAppProcessConfiguration);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();

	if (numOfApplicationIds == 0) {
		applicationProcessConfiguration.definitions.clear();
		// todo: indicate that its empty and not full.
		return;
	}

	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t applicationId = request.readUint16();
		if (not appExistsInApplicationConfiguration(packetStoreId, applicationId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::NonExistingApplicationInDefinition);
			continue;
		}
		uint16_t numOfServiceIds = request.readUint16();

		if (numOfServiceIds == 0) {
			applicationProcessConfiguration.definitions[packetStoreId].erase(applicationId);
			continue;
		}

		for (int j = 0; j < numOfServiceIds; j++) {
			uint16_t serviceId = request.readUint16();
			if (not serviceExistsInApp(packetStoreId, applicationId, serviceId)) {
				ErrorHandler::reportError(request,
				                          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinitionInApp);
				continue;
			}
			uint16_t numOfMessageTypes = request.readUint16();

			if (numOfMessageTypes == 0) {
				applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.erase(
				    serviceId);
				continue;
			}

			for (int k = 0; k < numOfMessageTypes; k++) {
				uint16_t messageType = request.readUint16();
				auto position = reportExistsInService(packetStoreId, applicationId, serviceId, messageType);
				if (position == -1) {
					ErrorHandler::reportError(
					    request, ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinitionInService);
					continue;
				}
				deleteReportDefinitionsOfService(packetStoreId, applicationId, serviceId, position);
			}
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::appConfigurationContentReport(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportAppConfigurationContent);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}

	Message report(ServiceType, MessageType::AppConfigurationContentReport, Message::TM, 1);

	report.appendOctetString(packetStoreId);
	uint16_t numOfApplications = applicationProcessConfiguration.definitions[packetStoreId].size();
	report.appendUint16(numOfApplications);

	for (auto& application : applicationProcessConfiguration.definitions[packetStoreId]) {
		uint16_t applicationId = application.first;
		uint16_t numOfServices = application.second.serviceTypeDefinitions.size();
		report.appendUint16(applicationId);
		report.appendUint16(numOfServices);

		for (auto& service : application.second.serviceTypeDefinitions) {
			uint16_t serviceId = service.first;
			uint16_t numOfMessageTypes = service.second.size();
			report.appendUint16(serviceId);
			report.appendUint16(numOfMessageTypes);

			for (auto messageType : service.second) {
				report.appendUint16(messageType);
			}
		}
	}

	mainService.storeMessage(report);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addStructuresToHousekeepingConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::AddStructuresToHousekeepingConfiguration);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplications = request.readUint16();

	for (int i = 0; i < numOfApplications; i++) {
		uint16_t applicationId = request.readUint16();
		uint8_t numOfStructures = request.readUint8();

		if (checkApplicationForHousekeeping(packetStoreId, applicationId, request)) {
			continue;
		}

		if (numOfStructures == 0) {
			addAllHousekeepingStructuresOfApplication(packetStoreId, applicationId);
			continue;
		}
		for (uint8_t j = 0; j < numOfStructures; j++) {
			uint8_t structureId = request.readUint8();
			if (supportsSubsamplingRate) {
				uint16_t subsamplingRate = request.readUint16();
				addHousekeepingStructureId(packetStoreId, applicationId, structureId, subsamplingRate);
				continue;
			}
			addHousekeepingStructureId(packetStoreId, applicationId, structureId);
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteStructuresFromHousekeepingConfiguration(
    Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteStructuresFromHousekeepingConfiguration);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplications = request.readUint16();

	if (numOfApplications == 0) {
		housekeepingReportConfiguration.definitions.clear();
		return;
	}

	for (int i = 0; i < numOfApplications; i++) {
		uint16_t applicationId = request.readUint16();
		uint8_t numOfStructures = request.readUint8();

		if (not appExistsInHousekeepingConfiguration(packetStoreId, applicationId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::NonExistingApplicationInDefinition);
			continue;
		}
		if (numOfStructures == 0) {
			housekeepingReportConfiguration.definitions[packetStoreId].erase(applicationId);
			continue;
		}

		for (uint8_t j = 0; j < numOfStructures; j++) {
			uint8_t structureId = request.readUint8();
			int position = structureIdExistsInApplication(packetStoreId, applicationId, structureId);
			if (position == -1) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::NonExistingHousekeepingStructureIdInDefinition);
				continue;
			}
			deleteHousekeepingStructure(packetStoreId, applicationId, position);
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::housekeepingConfigurationContentReport(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportHousekeepingConfigurationContent);

	auto packetStoreId = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreId)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}

	Message report(ServiceType, MessageType::HousekeepingConfigurationContentReport, Message::TM, 1);
	report.appendOctetString(packetStoreId);

	uint16_t numOfApplications = housekeepingReportConfiguration.definitions[packetStoreId].size();
	report.appendUint16(numOfApplications);

	for (auto& application : housekeepingReportConfiguration.definitions[packetStoreId]) {
		uint16_t applicationId = application.first;
		uint8_t numOfStructures = application.second.housekeepingStructIds.size();
		report.appendUint16(applicationId);
		report.appendUint8(numOfStructures);

		for (int i = 0; i < numOfStructures; i++) {
			uint8_t structureId = application.second.housekeepingStructIds[i];
			report.appendUint8(structureId);
			if (supportsSubsamplingRate) {
				uint16_t subsamplingRate = application.second.subsamplingRates[i];
				report.appendUint16(subsamplingRate);
			}
		}
	}
	mainService.storeMessage(report);
}

// void StorageAndRetrievalService::PacketSelectionSubservice::addEventDefinitionsToEventReportConfiguration(
//    Message& request) {
//	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.messageType == MessageType::AddEventDefinitionsToEventReportConfiguration,
//	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//
//	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
//	request.readOctetString(packetStoreData);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
//	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
//		return;
//	}
//	uint16_t numOfApplicationIds = request.readUint16();
//	for (int i = 0; i < numOfApplicationIds; i++) {
//		uint16_t applicationId = request.readUint16();
//		if (not appIsControlled(applicationId, request)) {
//			continue;
//		}
//		if (exceedsMaxEventDefinitionIds(packetStoreId, applicationId, request)) {
//			continue;
//		}
//		if (noEventInDefinition(packetStoreId, applicationId, request)) {
//			continue;
//		}
//		if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
//			createEventReportBlockingDefinition(packetStoreId, applicationId);
//		}
//		uint16_t numOfEventIds = request.readUint16();
//		if (!numOfEventIds) {
//			if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
//				createEventReportBlockingDefinition(packetStoreId, applicationId);
//			}
//			deleteEventDefinitionIds(packetStoreId, applicationId, true, 0);
//			return;
//		}
//		for (int j = 0; j < numOfEventIds; j++) {
//			uint16_t eventId = request.readUint16();
//			uint16_t garbage = 0;
//			if (not eventDefinitionIdExists(packetStoreId, applicationId, eventId, garbage)) {
//				createEventDefinitionId(packetStoreId, applicationId, eventId);
//			}
//		}
//	}
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::deleteEventDefinitionsFromEventReportConfiguration(
//    Message& request) {
//	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteEventDefinitionsFromEventReportConfiguration,
//	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
//	request.readOctetString(packetStoreData);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
//	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
//		return;
//	}
//	uint16_t numOfApplicationIds = request.readUint16();
//	if (!numOfApplicationIds) {
//		eventReportConfiguration.definitions[packetStoreId].clear();
//		return;
//	}
//	for (int i = 0; i < numOfApplicationIds; i++) {
//		uint16_t applicationId = request.readUint16();
//		if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
//			ErrorHandler::reportError(request,
//			                          ErrorHandler::ExecutionStartErrorType::NonExistingEventReportBlockingDefinition);
//			continue;
//		}
//		uint16_t numOfEventIds = request.readUint16();
//		if (!numOfEventIds) {
//			eventReportConfiguration.definitions[packetStoreId].erase(applicationId);
//			return;
//		}
//		for (int j = 0; j < numOfEventIds; j++) {
//			uint16_t eventId = request.readUint16();
//			uint16_t index = 0;
//			if (not eventDefinitionIdExists(packetStoreId, applicationId, eventId, index)) {
//				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingEventDefinitionId);
//				continue;
//			}
//			deleteEventDefinitionIds(packetStoreId, applicationId, false, index);
//		}
//	}
//}
//
// void StorageAndRetrievalService::PacketSelectionSubservice::eventConfigurationContentReport(Message& request) {
//	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.messageType == MessageType::ReportEventConfigurationContent, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
//	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
//
//	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
//	request.readOctetString(packetStoreData);
//	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
//	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
//		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
//		return;
//	}
//	Message contentReport(ServiceType, MessageType::EventConfigurationContentReport, Message::TM, 1);
//	contentReport.appendString(packetStoreId);
//	uint16_t numOfApplications = eventReportConfiguration.definitions[packetStoreId].size();
//	contentReport.appendUint16(numOfApplications);
//	for (auto& app : eventReportConfiguration.definitions[packetStoreId]) {
//		contentReport.appendUint16(app.first);
//		contentReport.appendUint16(app.second.eventDefinitionIds.size());
//		for (auto& id : app.second.eventDefinitionIds) {
//			contentReport.appendUint16(id);
//		}
//	}
//	mainService.storeMessage(contentReport);
//}

// void StorageAndRetrievalService::PacketSelectionSubservice::createAppDefinition(
//     const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId) {
//	typedef etl::vector<uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
//	etl::map<uint16_t, reportTypes, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypes;
//	ApplicationProcessDefinition newAppDefinition;
//	newAppDefinition.serviceTypeDefinitions = serviceTypes;
//	applicationProcessConfiguration.definitions[packetStoreId].insert({applicationId, newAppDefinition});
// }
//

//
// void StorageAndRetrievalService::PacketSelectionSubservice::createServiceDefinition(
//     const String<ECSS_MAX_PACKET_STORE_ID_SIZE>& packetStoreId, uint16_t applicationId, uint16_t serviceId) {
//	etl::vector<uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> reportTypes;
//	applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.insert(
//	    {serviceId, reportTypes});
// }
