#include <iostream>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"
#include "catch2/catch_all.hpp"

//typedef String<ECSSPacketStoreIdSize> PacketStoreId;
etl::map<String<ECSSPacketStoreIdSize>, PacketStore, ECSSMaxPacketStores> packetStores;
StorageAndRetrievalService& storageAndRetrieval1 = Services.storageAndRetrieval;
PacketSelectionSubservice packetSelection = PacketSelectionSubservice(packetStores);

uint8_t applicationProcessesOfPacketSelectionSubservice[] = {1};
uint8_t servicesOfPacketSelectionSubservice[] = {3, 5};
uint8_t messages1OfPacketSelectionSubservice[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2OfPacketSelectionSubservice[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

void validReportTypesOfPacketSelectionSubservice(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (auto appID: applicationProcessesOfPacketSelectionSubservice) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesOfPacketSelectionSubservice[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1OfPacketSelectionSubservice : messages2OfPacketSelectionSubservice;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}


String<ECSSPacketStoreIdSize> addPacketStoreToPacketSelection() {
	uint8_t packetStoreData[ECSSPacketStoreIdSize];
	uint8_t name[4] = "ps1";
	std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);

	std::copy(name, name + 3, packetStoreData);
	String<ECSSPacketStoreIdSize> packetStoreID(packetStoreData);
	storageAndRetrieval1.addPacketStore(packetStoreID, PacketStore());
	return packetStoreID;
}


void resetAppProcessConfigurationOfPacketSelectionSubservice() {
	packetSelection.applicationProcessConfiguration.clear();
	packetSelection.controlledApplications.clear();
	storageAndRetrieval1.resetPacketStores();
	REQUIRE(packetSelection.applicationProcessConfiguration.empty());
}

TEST_CASE("Add report types to the packet selection subservice") {

	SECTION("Successful addition of report types to the Application Process Configuration") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypesOfPacketSelectionSubservice(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(packetSelection.applicationProcessConfiguration.size() == 1);
		auto& definitions = packetSelection.applicationProcessConfiguration[packetStoreID].definitions;

		for (auto appID: applicationProcessesOfPacketSelectionSubservice) {
			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = servicesOfPacketSelectionSubservice[j];
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(definitions.find(appServicePair) != definitions.end());
				REQUIRE(definitions[appServicePair].size() == 2);
				uint8_t* messages = (j == 0) ? messages1OfPacketSelectionSubservice : messages2OfPacketSelectionSubservice;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(definitions[appServicePair].begin(),
					                  definitions[appServicePair].end(),
					                  messages[k]) != definitions[appServicePair].end());
				}
			}
		}

		resetAppProcessConfigurationOfPacketSelectionSubservice();
		ServiceTests::reset();
		Services.reset();
	}
}
