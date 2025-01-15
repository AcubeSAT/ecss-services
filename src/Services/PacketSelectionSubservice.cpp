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

void PacketSelectionSubservice::addAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	for (const auto& service: AllReportTypes::MessagesOfService) {
		uint8_t serviceType = service.first;
		addAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::addAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	for (const auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration[packetStoreID].definitions[appServicePair].push_back(messageType);
	}
}

uint8_t PacketSelectionSubservice::countServicesOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	auto& definitions = applicationProcessConfiguration[packetStoreID].definitions;
	return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](const auto& definition) { return applicationID == definition.first.first; });
}

uint8_t PacketSelectionSubservice::countReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return applicationProcessConfiguration[packetStoreID].definitions[appServicePair].size();
}

bool PacketSelectionSubservice::isAppControlled(const Message& request, uint8_t applicationId) {
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

bool PacketSelectionSubservice::allServiceTypesAllowed(const Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	if (countServicesOfApplication(packetStoreID, applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool PacketSelectionSubservice::maxServiceTypesReached(const Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
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

bool PacketSelectionSubservice::maxReportTypesReached(const Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
                                                      uint8_t serviceType) {
	if (countReportsOfService(packetStoreID, applicationID, serviceType) >= AllReportTypes::MessagesOfService.at(serviceType).size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool PacketSelectionSubservice::checkMessage(const Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType,
                                             uint8_t messageType) {
	return !(maxReportTypesReached(request, packetStoreID, applicationID, serviceType) or
	         reportExistsInAppProcessConfiguration(packetStoreID, applicationID, serviceType, messageType));
}

bool PacketSelectionSubservice::reportExistsInAppProcessConfiguration(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType,
                                                                      const uint8_t messageType) {
	const auto appServicePair = std::make_pair(applicationID, serviceType);
	return std::find(applicationProcessConfiguration[packetStoreID].definitions[appServicePair].begin(),
	                 applicationProcessConfiguration[packetStoreID].definitions[appServicePair].end(),
	                 messageType) != applicationProcessConfiguration[packetStoreID].definitions[appServicePair].end();
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
		uint8_t applicationID = request.readUint8();
		const uint8_t numOfServices = request.readUint8();

		if (not checkApplicationOfAppProcessConfig(request, packetStoreID, applicationID, numOfServices)) {
			continue;
		}

		if (numOfServices == 0) {
			addAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t serviceIndex = 0; serviceIndex < numOfServices; serviceIndex++) {
			uint8_t serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (not checkService(request, packetStoreID, applicationID, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				addAllReportsOfService(packetStoreID, applicationID, serviceType);
				continue;
			}

			for (uint8_t messageIndex = 0; messageIndex < numOfMessages; messageIndex++) {
				uint8_t messageType = request.readUint8();

				if (not checkMessage(request, packetStoreID, applicationID, serviceType, messageType)) {
					continue;
				}
				auto appServicePair = std::make_pair(applicationID, serviceType);
				applicationProcessConfiguration[packetStoreID].definitions[appServicePair].push_back(messageType);
			}
		}
	}
}

void PacketSelectionSubservice::deleteAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	for (const auto& [fst, snd]: AllReportTypes::MessagesOfService) {
		uint8_t serviceType = fst;
		deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::deleteAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	for (const auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration[packetStoreID].definitions.erase(appServicePair);
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
		auto definition = applicationProcessConfiguration[packetStoreID].definitions;
		while (!definition.empty()) {
			definition.erase(definition.begin());
		}
	}

	for (uint8_t appIndex = 0; appIndex < numOfApplication; appIndex++) {
		uint8_t applicationID = request.readUint8();
		const uint8_t numOfServices = request.readUint8();
		if (numOfServices == 0) {
			deleteAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t serviceIndex = 0; serviceIndex < numOfServices; serviceIndex++) {
			uint8_t serviceType = request.readUint8();
			const uint8_t numOfMessages = request.readUint8();

			if (numOfMessages == 0) {
				deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
			}
			for (uint8_t messageIndex = 0; messageIndex < numOfMessages; messageIndex++) {
				uint8_t messageType = request.readUint8();
				auto appServicePair = std::make_pair(applicationID, serviceType);
				auto* const messageTypeIndex = etl::find(applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).begin(), applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).end(), messageType);

				applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).erase(messageTypeIndex);
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
	const uint8_t numberOfApplications = applicationProcessConfiguration[packetStoreID].definitions.size();
	report.appendUint8(numberOfApplications);


	for (auto application = applicationProcessConfiguration[packetStoreID].definitions.begin(); application != applicationProcessConfiguration[packetStoreID].definitions.end(); application++) {
		auto applicationsID = application->first.first;
		report.appendUint8(applicationsID);

		uint8_t numberOfServiceTypes = 0;
		etl::array<uint8_t, ECSSMaxApplicationsServicesCombinations> serviceTypes = {};
		for (auto getApplication = applicationProcessConfiguration[packetStoreID].definitions.begin(); getApplication != applicationProcessConfiguration[packetStoreID].definitions.end(); getApplication++) {
			if (applicationsID == getApplication->first.first) {
				serviceTypes[numberOfServiceTypes] = getApplication->first.second;
				numberOfServiceTypes++;
			}
		}

		report.appendUint8(numberOfServiceTypes);

		for (uint8_t serviceTypeIndex = 0; serviceTypeIndex < numberOfServiceTypes; serviceTypeIndex++) {
			report.appendUint8(serviceTypes[serviceTypeIndex]);

			const uint8_t numberOfMessages = applicationProcessConfiguration[packetStoreID].definitions.size();
			for (uint8_t messageIndex = 0; messageIndex < numberOfMessages; messageIndex++) {
				auto appServicePair = std::make_pair(applicationsID, serviceTypes[serviceTypeIndex]);
				report.appendUint8(applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair)[messageIndex]);
			}
		}
	}
}

