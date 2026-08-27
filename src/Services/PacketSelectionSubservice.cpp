#include <limits>
#include "Services/PacketSelectionSubservice.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Services/StorageAndRetrievalService.hpp"

PacketSelectionSubservice::PacketStoreId PacketSelectionSubservice::readPacketStoreId(Message& message) {
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
	auto& configuration = packetStoreAppProcessConfig[packetStoreID];

	const uint8_t numOfApplications = request.readUint8();

	for (uint8_t app = 0; app < numOfApplications; app++) {
		const ApplicationProcessId applicationID = request.read<ApplicationProcessId>();
		const uint8_t numOfServices = request.readUint8();

		if (not configuration.checkApplicationOfAppProcessConfigValid(request, applicationID, numOfServices,
		controlledApplications)) {
			continue;
		}

		if (numOfServices == 0) {
			configuration.addAllReportsOfApplication(request, applicationID);
			continue;
		}

		for (uint8_t service = 0; service < numOfServices; service++) {
			const ServiceTypeNum serviceType = request.read<ServiceTypeNum>();
			const uint8_t numOfMessages = request.readUint8();

			if (not configuration.checkServiceCanBeAdded(request, applicationID, numOfMessages, serviceType)) {
				continue;
			}

			if (numOfMessages == 0) {
				configuration.addAllReportsOfService(request, applicationID, serviceType);
				continue;
			}

			for (uint8_t message = 0; message < numOfMessages; message++) {
				const MessageTypeNum messageType = request.read<MessageTypeNum>();

				if (not configuration.checkMessageCanBeAdded(request, applicationID, serviceType, messageType)) {
					continue;
				}
				configuration.addReport(applicationID, serviceType, messageType);
			}
		}
	}
}

void PacketSelectionSubservice::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	if (!request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration)) {
		return;
	}
	const auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}
	auto& configuration = packetStoreAppProcessConfig[packetStoreID];

	const uint8_t numOfApplications = request.readUint8();
	if (numOfApplications == 0U) {
		configuration.definitions.clear();
		return;
	}

	for (uint8_t app = 0; app < numOfApplications; app++) {
		const ApplicationProcessId applicationID = request.read<ApplicationProcessId>();
		const uint8_t numOfServices = request.readUint8();

		if (not configuration.checkApplicationInConfiguration(request, applicationID, numOfServices)) {
			continue;
		}

		if (numOfServices == 0) {
			configuration.deleteApplicationProcess(applicationID);
			continue;
		}

		for (uint8_t service = 0; service < numOfServices; service++) {
			const ServiceTypeNum serviceType = request.read<ServiceTypeNum>();
			const uint8_t numOfMessages = request.readUint8();

			if (not configuration.checkServiceTypeInConfiguration(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				configuration.deleteServiceType(applicationID, serviceType);
				continue;
			}

			for (uint8_t message = 0; message < numOfMessages; message++) {
				const MessageTypeNum messageType = request.read<MessageTypeNum>();

				if (not configuration.checkReportTypeInConfiguration(request, applicationID, serviceType, messageType)) {
					continue;
				}
				configuration.deleteReportType(applicationID, serviceType, messageType);
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
	auto& configuration = packetStoreAppProcessConfig[packetStoreID];
	const auto& definitions = configuration.definitions;

	Message report = createTM(StorageAndRetrievalService::MessageType::ApplicationProcessReport);
	report.appendFixedString(packetStoreID);

	uint8_t numberOfApplications = 0;
	ApplicationProcessId previousAppID = std::numeric_limits<ApplicationProcessId>::max();
	for (const auto& [appServiceKey, reportTypes]: definitions) {
		if (appServiceKey.first != previousAppID) {
			previousAppID = appServiceKey.first;
			numberOfApplications++;
		}
	}
	report.appendUint8(numberOfApplications);

	// The map is sorted by key, so all entries of the same application process are accessed consecutively.
	previousAppID = std::numeric_limits<ApplicationProcessId>::max();
	for (const auto& [appServiceKey, reportTypes]: definitions) {
		const ApplicationProcessId applicationID = appServiceKey.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
			report.append<ApplicationProcessId>(applicationID);
			report.appendUint8(configuration.countServicesOfApplication(applicationID));
		}

		report.append<ServiceTypeNum>(appServiceKey.second);
		report.appendUint8(reportTypes.size());
		for (const auto& messageType: reportTypes) {
			report.append<MessageTypeNum>(messageType);
		}
	}
	storeMessage(report);
}
