#include "Services/StorageAndRetrievalService.hpp"

PacketSelectionSubservice::PacketSelectionSubservice(etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& mainServicePacketStores) : packetStores(mainServicePacketStores) {}

String<ECSSPacketStoreIdSize> PacketSelectionSubservice::readPacketStoreId(Message& message) {
	uint8_t packetStoreId[ECSSPacketStoreIdSize];
	message.readString(packetStoreId, ECSSPacketStoreIdSize);
	return packetStoreId;
}

bool PacketSelectionSubservice::packetStoreExists(const String<ECSSPacketStoreIdSize>& packetStoreId) {
	return packetStores.find(packetStoreId) != packetStores.end();
}

void PacketSelectionSubservice::addAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	for (auto& service: AllMessageTypes::messagesOfService) {
		uint8_t serviceType = service.first;
		addAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::addAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	for (auto& messageType: AllMessageTypes::messagesOfService[serviceType]) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration.definitions[packetStoreID][appServicePair].push_back(messageType);
	}
}

uint8_t PacketSelectionSubservice::countServicesOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	auto& definitions = applicationProcessConfiguration.definitions[packetStoreID];
	return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](auto& definition) { return applicationID == definition.first.first; });
}

uint8_t PacketSelectionSubservice::countReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return applicationProcessConfiguration.definitions[packetStoreID][appServicePair].size();
}

bool PacketSelectionSubservice::isAppControlled(Message& request, uint8_t applicationId) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
		return false;
	}
	return true;
}

bool PacketSelectionSubservice::checkApplicationOfAppProcessConfig(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
                                                                   uint8_t numOfServices) {
	if (not isAppControlled(request, applicationID) or allServiceTypesAllowed(request, packetStoreID, applicationID)) {
		for (uint8_t i = 0; i < numOfServices; i++) {
			request.skipBytes(1);
			uint8_t numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool PacketSelectionSubservice::allServiceTypesAllowed(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	if (countServicesOfApplication(packetStoreID, applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool PacketSelectionSubservice::maxServiceTypesReached(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	if (countServicesOfApplication(packetStoreID, applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
		return true;
	}
	return false;
}

bool PacketSelectionSubservice::checkService(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t numOfMessages) {
	if (maxServiceTypesReached(request, packetStoreID, applicationID)) {
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool PacketSelectionSubservice::maxReportTypesReached(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
                                                      uint8_t serviceType) {
	if (countReportsOfService(packetStoreID, applicationID, serviceType) >= AllMessageTypes::messagesOfService[serviceType].size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool PacketSelectionSubservice::checkMessage(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType,
                                             uint8_t messageType) {
	if (maxReportTypesReached(request, packetStoreID, applicationID, serviceType) or
	    reportExistsInAppProcessConfiguration(packetStoreID, applicationID, serviceType, messageType)) {
		return false;
	}
	return true;
}

bool PacketSelectionSubservice::reportExistsInAppProcessConfiguration(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType,
                                                                      uint8_t messageType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return std::find(applicationProcessConfiguration.definitions[packetStoreID][appServicePair].begin(),
	                 applicationProcessConfiguration.definitions[packetStoreID][appServicePair].end(),
	                 messageType) != applicationProcessConfiguration.definitions[packetStoreID][appServicePair].end();
}

void PacketSelectionSubservice::addReportTypesToAppProcessConfiguration(Message& request) {
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration);
	auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}

	uint8_t numOfApplications = request.readUint8();

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not checkApplicationOfAppProcessConfig(request, packetStoreID, applicationID, numOfServices)) {
			continue;
		}

		if (numOfServices == 0) {
			addAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not checkService(request, packetStoreID, applicationID, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				addAllReportsOfService(packetStoreID, applicationID, serviceType);
				continue;
			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();

				if (not checkMessage(request, packetStoreID, applicationID, serviceType, messageType)) {
					continue;
				}
				auto appServicePair = std::make_pair(applicationID, serviceType);
				applicationProcessConfiguration.definitions[packetStoreID][appServicePair].push_back(
				    messageType);
			}
		}
	}
}
