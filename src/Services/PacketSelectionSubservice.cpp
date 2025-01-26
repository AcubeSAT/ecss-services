#include "Services/PacketSelectionSubservice.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Services/StorageAndRetrievalService.hpp"

PacketStoreId PacketSelectionSubservice::readPacketStoreId(Message& message) {
	etl::array<uint8_t, ECSSPacketStoreIdSize> packetStoreId{};
	message.readString(packetStoreId.data(), ECSSPacketStoreIdSize);
	return packetStoreId.data();
}

bool PacketSelectionSubservice::packetStoreExists(const PacketStoreId& packetStoreId) {
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

	for (uint8_t app = 0; app < numOfApplications; app++) {
		ApplicationProcessId applicationID = request.read<ApplicationProcessId>();
		const uint8_t numOfServices = request.readUint8();

		if (not packetStoreAppProcessConfig[packetStoreID].isApplicationOfAppProcessConfigValid(request,
		applicationID, numOfServices, controlledApplications)) {
			continue;
		}

		if (numOfServices == 0) {
			packetStoreAppProcessConfig[packetStoreID].addAllReportsOfApplication(request, applicationID);
			continue;
		}

		for (uint8_t service = 0; service < numOfServices; service++) {
			ServiceTypeNum serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (not packetStoreAppProcessConfig[packetStoreID].canServiceBeAdded(request, applicationID,
			numOfMessages, serviceType)) {
				continue;
			}

			if (numOfMessages == 0) {
				packetStoreAppProcessConfig[packetStoreID].addAllReportsOfService(request, applicationID, serviceType);
				continue;
			}

			for (uint8_t message = 0; message < numOfMessages; message++) {
				uint8_t messageType = request.readUint8();

				if (not packetStoreAppProcessConfig[packetStoreID].canMessageBeAdded(request, applicationID,
				serviceType, messageType)) {
					continue;
				}
				auto appServicePair = std::make_pair(applicationID, serviceType);
				packetStoreAppProcessConfig[packetStoreID].definitions[appServicePair].push_back(messageType);
			}
		}
	}
}

void PacketSelectionSubservice::deleteAllReportsOfApplication(const PacketStoreId& packetStoreID, ApplicationProcessId applicationID) {
	for (const auto& [first, _]: AllReportTypes::getMessagesOfService()) {
		ServiceTypeNum serviceType = first;
		deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::deleteAllReportsOfService(const PacketStoreId& packetStoreID, ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	for (const auto& messageType: AllReportTypes::getMessagesOfService().at(serviceType)) {
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
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}

	const auto numOfApplications = request.readUint8();
	if (numOfApplications == 0U) {
		packetStoreAppProcessConfig[packetStoreID].definitions.clear();
	}

	for (uint8_t app = 0; app < numOfApplications; app++) {
		ApplicationProcessId applicationID = request.readUint8();
		const uint8_t numOfServices = request.readUint8();
		if (numOfServices == 0) {
			deleteAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t service = 0; service < numOfServices; service++) {
			ServiceTypeNum serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (numOfMessages == 0) {
				deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
			}
			for (uint8_t message = 0; message < numOfMessages; message++) {
				uint8_t messageType = request.readUint8();
				auto appServicePair = std::make_pair(applicationID, serviceType);
				auto* const messageTypeIndex = etl::find(packetStoreAppProcessConfig[packetStoreID].definitions[appServicePair].begin(), packetStoreAppProcessConfig[packetStoreID].definitions.at(appServicePair).end(), messageType);

				packetStoreAppProcessConfig[packetStoreID].definitions[appServicePair].erase(messageTypeIndex);
			}
		}
	}
}

void PacketSelectionSubservice::reportApplicationProcess(Message& request) {
	if (not request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::ReportApplicationProcess)) {
		return;
	}
	const auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}
	Message report = createTM(StorageAndRetrievalService::MessageType::ApplicationProcessReport);

	report.appendFixedString(packetStoreID);

    etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses> uniqueApps;
    for (const auto& [appServicePair, _] : packetStoreAppProcessConfig[packetStoreID].definitions) {
        auto appId = appServicePair.first;
        if (etl::find(uniqueApps.begin(), uniqueApps.end(), appId) == uniqueApps.end()) {
            uniqueApps.push_back(appId);
        }
    }
    const uint8_t numberOfApplications = uniqueApps.size();	
	report.appendUint8(numberOfApplications);

	for (const auto& applicationID : uniqueApps) {
        report.append<ApplicationProcessId>(applicationID);

		uint8_t numberOfServiceTypes = etl::count_if(packetStoreAppProcessConfig[packetStoreID].definitions.begin(), packetStoreAppProcessConfig[packetStoreID].definitions.end(), [applicationID](const auto& definition) {
			return definition.first.first == applicationID;
		});
		report.appendUint8(numberOfServiceTypes);

        for (const auto& [appServicePair, _] : packetStoreAppProcessConfig[packetStoreID].definitions) {
            if (appServicePair.first == applicationID) {
                auto serviceType = appServicePair.second;
				report.append<ServiceTypeNum>(serviceType);

				const auto& messages = packetStoreAppProcessConfig[packetStoreID].definitions[appServicePair];
            
				report.appendUint8(messages.size());
				for (const auto& messageType : messages) {
					report.append<MessageTypeNum>(messageType);
				}
            }
        }
    }
}

