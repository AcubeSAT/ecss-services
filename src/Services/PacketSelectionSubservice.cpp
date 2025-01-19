#include "Services/PacketSelectionSubservice.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Services/StorageAndRetrievalService.hpp"

String<ECSSPacketStoreIdSize> PacketSelectionSubservice::readPacketStoreId(Message& message) {
	etl::array<uint8_t, ECSSPacketStoreIdSize> packetStoreId{};
	message.readString(packetStoreId.data(), ECSSPacketStoreIdSize);
	return packetStoreId.data();
}

bool PacketSelectionSubservice::packetStoreExists(const String<ECSSPacketStoreIdSize>& packetStoreId) {
	return packetStores.find(packetStoreId) != packetStores.end();
}

void PacketSelectionSubservice::addReportTypesToAppProcessConfiguration(Message& request) {
	if (!request.assertTC(StorageAndRetrievalService::ServiceType,
	StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration)) {
		return;
	}

	const auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}

	const uint8_t numOfApplications = request.readUint8();

	for (uint8_t appIndex = 0; appIndex < numOfApplications; appIndex++) {
		ApplicationProcessId applicationID = request.readUint8();
		const uint8_t numOfServices = request.readUint8();

		if (not packetStoreAppProcessConfig[packetStoreID].checkApplicationOfAppProcessConfig(request,
		applicationID, numOfServices, controlledApplications)) {
			continue;
		}

		if (numOfServices == 0) {
			packetStoreAppProcessConfig[packetStoreID].addAllReportsOfApplication(applicationID);
			continue;
		}

		for (uint8_t serviceIndex = 0; serviceIndex < numOfServices; serviceIndex++) {
			ServiceTypeNum serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (not packetStoreAppProcessConfig[packetStoreID].checkService(request, applicationID, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				packetStoreAppProcessConfig[packetStoreID].addAllReportsOfService(applicationID, serviceType);
				continue;
			}

			for (uint8_t messageIndex = 0; messageIndex < numOfMessages; messageIndex++) {
				uint8_t messageType = request.readUint8();

				if (not packetStoreAppProcessConfig[packetStoreID].checkMessage(request, applicationID,
				serviceType, messageType)) {
					continue;
				}
				auto appServicePair = std::make_pair(applicationID, serviceType);
				packetStoreAppProcessConfig[packetStoreID].definitions[appServicePair].push_back(messageType);
			}
		}
	}
}

void PacketSelectionSubservice::deleteAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, ApplicationProcessId applicationID) {
	for (const auto& [fst, snd]: AllReportTypes::MessagesOfService) {
		ServiceTypeNum serviceType = fst;
		deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::deleteAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	for (const auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		packetStoreAppProcessConfig[packetStoreID].definitions.erase(appServicePair);
	}
}

void PacketSelectionSubservice::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	if (!request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::DeleteReportTypesTFromAppProcessConfiguration)) {
		return;
	}
	const auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		return;
	}

	const auto numOfApplication = request.readUint8();
	if (numOfApplication == 0U) {
		auto definition = packetStoreAppProcessConfig[packetStoreID].definitions;
		while (!definition.empty()) {
			definition.erase(definition.begin());
		}
	}

	for (uint8_t appIndex = 0; appIndex < numOfApplication; appIndex++) {
		ApplicationProcessId applicationID = request.readUint8();
		const uint8_t numOfServices = request.readUint8();
		if (numOfServices == 0) {
			deleteAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t serviceIndex = 0; serviceIndex < numOfServices; serviceIndex++) {
			ServiceTypeNum serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (numOfMessages == 0) {
				deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
			}
			for (uint8_t messageIndex = 0; messageIndex < numOfMessages; messageIndex++) {
				uint8_t messageType = request.readUint8();
				auto appServicePair = std::make_pair(applicationID, serviceType);
				auto* const messageTypeIndex = etl::find(packetStoreAppProcessConfig[packetStoreID].definitions.at(appServicePair).begin(), packetStoreAppProcessConfig[packetStoreID].definitions.at(appServicePair).end(), messageType);

				packetStoreAppProcessConfig[packetStoreID].definitions.at(appServicePair).erase(messageTypeIndex);
			}
		}
	}
}

void PacketSelectionSubservice::reportApplicationProcess(Message& request) {
	if (not request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::ReportApplicationProcess)) {
		return;
	}
	const auto packetStoreID = readPacketStoreId(request);
	if (!packetStoreExists(packetStoreID)) {
		return;
	}
	Message report = createTM(StorageAndRetrievalService::MessageType::ApplicationProcessReport);


	report.appendString(packetStoreID);
	const uint8_t numberOfApplications = packetStoreAppProcessConfig[packetStoreID].definitions.size();
	report.appendUint8(numberOfApplications);


	for (auto application = packetStoreAppProcessConfig[packetStoreID].definitions.begin(); application != packetStoreAppProcessConfig[packetStoreID].definitions.end(); application++) {
		auto applicationsID = application->first.first;
		report.appendUint8(applicationsID);

		uint8_t numberOfServiceTypes = 0;
		etl::array<uint8_t, ECSSMaxApplicationsServicesCombinations> serviceTypes = {};
		for (auto getApplication = packetStoreAppProcessConfig[packetStoreID].definitions.begin(); getApplication != packetStoreAppProcessConfig[packetStoreID].definitions.end(); getApplication++) {
			if (applicationsID == getApplication->first.first) {
				serviceTypes[numberOfServiceTypes] = getApplication->first.second;
				numberOfServiceTypes++;
			}
		}

		report.appendUint8(numberOfServiceTypes);

		for (ServiceTypeNum serviceTypeIndex = 0; serviceTypeIndex < numberOfServiceTypes; serviceTypeIndex++) {
			report.appendUint8(serviceTypes[serviceTypeIndex]);

			const uint8_t numberOfMessages = packetStoreAppProcessConfig[packetStoreID].definitions.size();
			for (uint8_t messageIndex = 0; messageIndex < numberOfMessages; messageIndex++) {
				auto appServicePair = std::make_pair(applicationsID, serviceTypes[serviceTypeIndex]);
				report.appendUint8(packetStoreAppProcessConfig[packetStoreID].definitions.at(appServicePair)[messageIndex]);
			}
		}
	}
}

