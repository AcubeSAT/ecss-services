#include "Services/StorageAndRetrievalService.hpp"
#include <iostream>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "catch2/catch.hpp"

StorageAndRetrievalService& storageAndRetrieval1 = Services.storageAndRetrieval;
PacketSelectionSubservice& packetSelection = storageAndRetrieval1.packetSelection;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20};
uint8_t redundantServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20, 1, 3};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

void validReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

void duplicateReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[0]);
			}
		}
	}
}

void validInvalidReportTypes(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 12 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToPick[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

void validAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

void validInvalidAllReportsOfService(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 12 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = servicesToPick[j];
			request.appendUint8(serviceType);
			uint8_t numOfMessages = (i == 0 or i == 1) ? 0 : numOfMessagesPerService;
			request.appendUint8(numOfMessages);
			if (i >= 2) {
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
					request.appendUint8(messages[k]);
				}
			}
		}
	}
}

void validAllReportsOfApp(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);
	}
}

void validInvalidAllReportsOfApp(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	std::fill(std::begin(packetStoreData) + 3, std::end(packetStoreData), 0);
	String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);

	request.appendString(packetStoreId);
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0 or i == 1) ? 0 : 2;
		uint8_t* servicesToPick = (i == 0) ? redundantServices : services;
		request.appendUint8(numOfServicesPerApp);

		if (i >= 2) {
			for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
				uint8_t serviceType = servicesToPick[j];
				request.appendUint8(serviceType);
				uint8_t numOfMessages = (i == 0 or i == 1) ? 0 : numOfMessagesPerService;
				request.appendUint8(numOfMessages);

				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
					request.appendUint8(messages[k]);
				}
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

void resetAppProcessConfiguration() {
	packetSelection.applicationProcessConfiguration.definitions.clear();
	packetSelection.controlledApplications.clear();
	storageAndRetrieval1.resetPacketStores();
	REQUIRE(packetSelection.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Add report types to the packet selection subservice") {
	SECTION("Successful addition of report types to the Application Process Configuration") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions.size() == 1);
		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];

		for (auto appID: applications) {
			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = services[j];
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(definitions.find(appServicePair) != definitions.end());
				REQUIRE(definitions[appServicePair].size() == 2);
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(definitions[appServicePair].begin(),
					                  definitions[appServicePair].end(),
					                  messages[k]) != definitions[appServicePair].end());
				}
			}
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested packet store, not present in the storage and retrieval subservice") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 1);
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested Application Process is not controlled by the service") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All service types already allowed") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		for (uint8_t i = 1; i < ECSSMaxServiceTypeDefinitions + 1; i++) {
			packetSelection.applicationProcessConfiguration.definitions[packetStoreID][std::make_pair(applicationID, i)];
		}
		CHECK(packetSelection.applicationProcessConfiguration.definitions[packetStoreID].size() == ECSSMaxServiceTypeDefinitions);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);
		REQUIRE(packetSelection.applicationProcessConfiguration.definitions[packetStoreID].size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max service types already reached") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = services[0]; // st03
		uint8_t serviceType2 = services[1]; // st05

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& applicationProcessConfig = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			applicationProcessConfig[std::make_pair(applicationID, i)];
		}
		CHECK(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 1);
		REQUIRE(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All report types already allowed") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0]; // st03
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		for (auto message: AllMessageTypes::messagesOfService[serviceType]) {
			packetSelection.applicationProcessConfiguration.definitions[packetStoreID][std::make_pair(applicationID, serviceType)]
			    .push_back(message);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) ==
		      2);
		REQUIRE(
		    packetSelection.applicationProcessConfiguration.definitions[packetStoreID][std::make_pair(applicationID, serviceType)]
		        .size() == AllMessageTypes::messagesOfService[serviceType].size());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max report types already reached") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = services[0]; // st03
		uint8_t serviceType2 = services[1]; // st05

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		validReportTypes(request);

		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];

		auto appServicePair1 = std::make_pair(applicationID, serviceType1);
		auto appServicePair2 = std::make_pair(applicationID, serviceType2);
		REQUIRE(definitions[appServicePair1].empty());
		REQUIRE(definitions[appServicePair2].empty());

		auto numOfMessages1 = AllMessageTypes::messagesOfService[serviceType1].size();
		auto numOfMessages2 = AllMessageTypes::messagesOfService[serviceType2].size();

		for (uint8_t i = 0; i < numOfMessages1 - 1; i++) {
			definitions[appServicePair1].push_back(i);
		}
		for (uint8_t i = 16; i < numOfMessages2 + 15; i++) {
			definitions[appServicePair2].push_back(i);
		}
		REQUIRE(definitions[appServicePair1].size() == numOfMessages1 - 1);
		REQUIRE(definitions[appServicePair2].size() == numOfMessages2 - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 2);
		REQUIRE(definitions.size() == 2);
		REQUIRE(definitions[appServicePair1].size() == numOfMessages1);
		REQUIRE(definitions[appServicePair2].size() == numOfMessages2);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested  addition of duplicate report type definitions") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		duplicateReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];
		REQUIRE(definitions.size() == 2);

		for (auto appID: applications) {
			for (auto& serviceType: services) {
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(definitions.find(appServicePair) != definitions.end());
				REQUIRE(definitions[appServicePair].size() == 1);
				REQUIRE(std::find(definitions[appServicePair].begin(),
				                  definitions[appServicePair].end(),
				                  messages1[0]) != definitions[appServicePair].end());
			}
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid and invalid application-related requests combined") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		uint8_t applicationID3 = 3;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID1);
		packetSelection.controlledApplications.push_back(applicationID3);
		validInvalidReportTypes(request);

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			packetSelection.applicationProcessConfiguration.definitions[packetStoreID][std::make_pair(applicationID3, i)];
		}
		CHECK(packetSelection.applicationProcessConfiguration.definitions[packetStoreID].size() ==
		      ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 3);

		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];
		REQUIRE(definitions.size() == 20);
		for (auto serviceType: allServices) {
			REQUIRE(definitions.find(std::make_pair(applicationID1, serviceType)) != definitions.end());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of a specified service type") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID1);
		validAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];
		for (auto serviceType: services) {
			REQUIRE(definitions[std::make_pair(applicationID1, serviceType)].size() ==
			        AllMessageTypes::messagesOfService[serviceType].size());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of a service type, combined with invalid requests") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID1);
		packetSelection.controlledApplications.push_back(applicationID2);
		validInvalidAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);

		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];
		REQUIRE(definitions.size() == 12);

		int cnt1 = 0;
		int cnt2 = 0;
		for (auto& pair: definitions) {
			if (pair.first.first == applicationID1) {
				cnt1++;
			} else if (pair.first.first == applicationID2) {
				cnt2++;
			}
		}
		REQUIRE(cnt1 == 10);
		REQUIRE(cnt2 == 2);

		for (auto& serviceType: allServices) {
			REQUIRE(definitions[std::make_pair(applicationID1, serviceType)].size() ==
			        AllMessageTypes::messagesOfService[serviceType].size());
		}
		for (auto& serviceType: services) {
			REQUIRE(definitions[std::make_pair(applicationID2, serviceType)].size() ==
			        AllMessageTypes::messagesOfService[serviceType].size());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of an application process") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID1);
		validAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];
		REQUIRE(definitions.size() == ECSSMaxServiceTypeDefinitions);

		for (auto serviceType: allServices) {
			REQUIRE(std::equal(definitions[std::make_pair(applicationID1, serviceType)].begin(),
			                   definitions[std::make_pair(applicationID1, serviceType)].end(),
			                   AllMessageTypes::messagesOfService[serviceType].begin()));
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of an application process, combined with invalid request") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID1);
		packetSelection.controlledApplications.push_back(applicationID2);
		validInvalidAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		auto& definitions = packetSelection.applicationProcessConfiguration.definitions[packetStoreID];

		REQUIRE(definitions.size() == 2 * ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
