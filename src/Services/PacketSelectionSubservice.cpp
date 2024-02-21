#include "Services/PacketSelectionSubservice.hpp"
#include "Helpers/AllReportTypes.hpp"
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
	for (auto& service: AllReportTypes::MessagesOfService) {
		uint8_t serviceType = service.first;
		addAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::addAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	for (auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration[packetStoreID].definitions[appServicePair].push_back(messageType);
	}
}

uint8_t PacketSelectionSubservice::countServicesOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	auto& definitions = applicationProcessConfiguration[packetStoreID].definitions;
	return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](auto& definition) { return applicationID == definition.first.first; });
}

uint8_t PacketSelectionSubservice::countReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return applicationProcessConfiguration[packetStoreID].definitions[appServicePair].size();
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
	if (countReportsOfService(packetStoreID, applicationID, serviceType) >= AllReportTypes::MessagesOfService.at(serviceType).size()) {
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
	return std::find(applicationProcessConfiguration[packetStoreID].definitions[appServicePair].begin(),
	                 applicationProcessConfiguration[packetStoreID].definitions[appServicePair].end(),
	                 messageType) != applicationProcessConfiguration[packetStoreID].definitions[appServicePair].end();
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
				applicationProcessConfiguration[packetStoreID].definitions[appServicePair].push_back(messageType);
			}
		}
	}
}

void PacketSelectionSubservice::deleteAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID) {
	for (auto& service: AllReportTypes::MessagesOfService) {
		uint8_t serviceType = service.first;
		deleteAllReportsOfService(packetStoreID, applicationID, serviceType);
	}
}

void PacketSelectionSubservice::deleteAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType) {
	for (auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration[packetStoreID].definitions.erase(appServicePair);
	}
}
void PacketSelectionSubservice::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::DeleteReportTypesTFromAppProcessConfiguration);
	auto packetStoreID = readPacketStoreId(request);
	if (not packetStoreExists(packetStoreID)) {
		return;
	}

	auto numOfApplication = request.readUint8();
	if (!numOfApplication) {
		auto definition = applicationProcessConfiguration[packetStoreID].definitions;
		for (; !definition.empty();) {
			definition.erase(definition.begin());
		}
	}

	for (uint8_t i = 0; i < numOfApplication; i++) {
		//		if (not checkService(request, packetStoreID, applicationID, numOfMessages)) {
		//			continue;
		//		}


		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();
		if (numOfServices == 0) {
			deleteAllReportsOfApplication(packetStoreID, applicationID);
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (numOfMessages == 0) {
				deleteAllReportsOfService(packetStoreID,applicationID,serviceType);
			}
			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();
				auto appServicePair = std::make_pair(applicationID, serviceType);
				auto messageTypeIndex = etl::find(applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).begin(),applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).end(),messageType);

				applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair).erase(messageTypeIndex);
			}
		}
	}


//	if (!numOfApplication) {
//		auto definition = applicationProcessConfiguration[packetStoreID].definitions;
//		for (; !definition.empty();) {
//			definition.erase(definition.begin());
//		}
//	}
}


void PacketSelectionSubservice::reportApplicationProcess(Message& request) {
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::reportApplicationProcess);
	auto packetStoreID = readPacketStoreId(request);
	if (!packetStoreExists(packetStoreID)) {
		return;
	}
	Message report = createTM(StorageAndRetrievalService::MessageType::applicationProcessReport);


	report.appendString(packetStoreID);
	uint8_t numberOfApplications = applicationProcessConfiguration[packetStoreID].definitions.size();
	report.appendUint8(numberOfApplications);


	for (auto application = applicationProcessConfiguration[packetStoreID].definitions.begin(); application != applicationProcessConfiguration[packetStoreID].definitions.end(); application++) {
		auto applicationsID = application->first.first;
		report.appendUint8(applicationsID);

		uint8_t numberOfServiceTypes = 0;
		etl::array<uint8_t, ECSSMaxApplicationsServicesCombinations> serviceTypes;
		for (auto getApplication = applicationProcessConfiguration[packetStoreID].definitions.begin(); getApplication != applicationProcessConfiguration[packetStoreID].definitions.end(); getApplication++) {
			if (applicationsID == getApplication->first.first) {
				serviceTypes[numberOfServiceTypes] = getApplication->first.second;
				numberOfServiceTypes++;
			}
		}

		report.appendUint8(numberOfServiceTypes);

		for(uint8_t i=0; i<numberOfServiceTypes; i++){
			report.appendUint8(serviceTypes[i]);

			uint8_t numberOfMessages = applicationProcessConfiguration[packetStoreID].definitions.size();
			for(uint8_t j = 0; j<numberOfMessages; j ++){
				auto appServicePair = std::make_pair(applicationsID, serviceTypes[i]);
				report.appendUint8(applicationProcessConfiguration[packetStoreID].definitions.at(appServicePair)[j]);
			}
		}
	}
}

void PacketSelectionSubservice::addStructureIdentifiersToTheHousekeepingParameterReport(Message& request){
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::addStructureIdentifiersToTheHousekeepingParameterReport);
	auto packetStoreID = readPacketStoreId(request);

	uint8_t numberOfApplications = request.readUint8();

	for(uint8_t i=0; i<numberOfApplications; i++){
		uint8_t applicationId = request.readUint8();

		uint8_t numberHousekeepingParameterReports =request.readUint8();

		if(numberHousekeepingParameterReports == 0){

			for(const auto & housekeepingStructures : Services.housekeeping.housekeepingStructures ){
				applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions[housekeepingStructures.first].push_back(housekeepingStructures.second.collectionInterval);
			}
		}
		for(uint8_t j=0; j<numberHousekeepingParameterReports; j++){

			uint8_t housekeepingParameterReportId = request.readUint8();
			CollectionInterval samplingRate = Services.housekeeping.getCollectionInterval(housekeepingParameterReportId);
			applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions[housekeepingParameterReportId].push_back(samplingRate);
		}

	}

}


void PacketSelectionSubservice::deleteStructureIdentifiersToTheHousekeepingParameterReport(Message& request){
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::deleteStructureIdentifiersToTheHousekeepingParameterReport);
	auto packetStoreID = readPacketStoreId(request);

	uint8_t numberOfApplications = request.readUint8();

	if(numberOfApplications == 0){

	}
	for(uint8_t i=0; i<numberOfApplications; i++){
		uint8_t applicationId = request.readUint8();

		uint8_t numberHousekeepingParameterReports =request.readUint8();

		if(numberHousekeepingParameterReports == 0){

						for(const auto & housekeepingStructures : Services.housekeeping.housekeepingStructures ){
							applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions.erase(housekeepingStructures.first);
						}
		}
		for(uint8_t j=0; j<numberHousekeepingParameterReports; j++){


			uint8_t housekeepingParameterReportId = request.readUint8();
			CollectionInterval samplingRate = Services.housekeeping.getCollectionInterval(housekeepingParameterReportId);
			applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions.erase(housekeepingParameterReportId);
		}

	}

}



void PacketSelectionSubservice::addStructureIdentifiersToTheDiagnosticParameterReport(Message& request){
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::addStructureIdentifiersToTheHousekeepingParameterReport);
	auto packetStoreID = readPacketStoreId(request);
	uint8_t numberOfApplications = request.readUint8();


	for(uint8_t i=0; i<numberOfApplications; i++){
		uint8_t applicationId = request.readUint8();

		uint8_t numberDiagnosticParameterReports = request.readUint8();

		if(numberDiagnosticParameterReports == 0){

		}
		for(uint8_t j=0; j<numberDiagnosticParameterReports; j++){

			uint8_t diagnosticParameterReportId = request.readUint8();
			CollectionInterval samplingRate = 0;
//			applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions[housekeepingParameterReportId].push_back(samplingRate);
		}
	}
}

void PacketSelectionSubservice::deleteStructureIdentifiersFromTheDiagnosticParameterReport(Message& request){
	request.assertTC(StorageAndRetrievalService::ServiceType, StorageAndRetrievalService::MessageType::addStructureIdentifiersToTheHousekeepingParameterReport);
	auto packetStoreID = readPacketStoreId(request);
	uint8_t numberOfApplications = request.readUint8();

	if(numberOfApplications == 0){

	}

	for(uint8_t i=0; i<numberOfApplications; i++){
		uint8_t applicationId = request.readUint8();

		uint8_t numberDiagnosticParameterReports = request.readUint8();

		if(numberDiagnosticParameterReports == 0){

		}
		for(uint8_t j=0; j<numberDiagnosticParameterReports; j++){

			uint8_t diagnosticParameterReportId = request.readUint8();
			CollectionInterval samplingRate = 0;
			//			applicationProcessConfiguration[packetStoreID].housekeepingParameterReportDefinitions[housekeepingParameterReportId].push_back(samplingRate);
		}
	}
}