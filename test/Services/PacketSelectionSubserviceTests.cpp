#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"
#include "catch2/catch_all.hpp"
#include "ForwardingAndPacketHelper.hpp"

StorageAndRetrievalService& storageAndRetrieval1 = Services.storageAndRetrieval;
PacketSelectionSubservice& packetSelection = storageAndRetrieval1.packetSelection;

String<ECSSPacketStoreIdSize> addPacketStoreToPacketSelection() {
	uint8_t packetStoreData[ECSSPacketStoreIdSize];
	uint8_t name[4] = "ps1";
	std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);

	std::copy(name, name + 3, packetStoreData);
	String<ECSSPacketStoreIdSize> packetStoreID(packetStoreData);
	storageAndRetrieval1.addPacketStore(packetStoreID, PacketStore());
	return packetStoreID;
}

void resetAppProcessConfigurationPacketSelection() {
	packetSelection.packetStoreAppProcessConfig.clear();
	packetSelection.controlledApplications.clear();
	storageAndRetrieval1.resetPacketStores();
	REQUIRE(packetSelection.packetStoreAppProcessConfig.empty());
}

TEST_CASE("Add report types to the packet selection subservice") {
	SECTION("Successful addition of report types to the Application Process Configuration") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(packetSelection.packetStoreAppProcessConfig.size() == 1);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;

		for (auto appID: ForwardingAndPacketHelper::applications) {
			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = ForwardingAndPacketHelper::services[j];
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(definitions.find(appServicePair) != definitions.end());
				REQUIRE(definitions[appServicePair].size() == 2);
				auto& messages = (j == 0) ? ForwardingAndPacketHelper::messages1 : ForwardingAndPacketHelper::messages2;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(definitions[appServicePair].begin(),
					                  definitions[appServicePair].end(),
					                  messages[k]) != definitions[appServicePair].end());
				}
			}
		}

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested packet store, not present in the storage and retrieval subservice") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 1);
		REQUIRE(packetSelection.packetStoreAppProcessConfig.empty());

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested Application Process is not controlled by the service") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		ForwardingAndPacketHelper::validReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		REQUIRE(packetSelection.packetStoreAppProcessConfig.empty());

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validReportTypes(request);

		for (uint8_t i = 1; i < ECSSMaxServiceTypeDefinitions + 1; i++) {
			packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions[std::make_pair(applicationID, i)];
		}
		CHECK(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() == ECSSMaxServiceTypeDefinitions);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);
		REQUIRE(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max service types already reached") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = ForwardingAndPacketHelper::services[0]; // st03
		uint8_t serviceType2 = ForwardingAndPacketHelper::services[1]; // st05

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::validReportTypes(request);

		auto& applicationProcessConfig = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			applicationProcessConfig[std::make_pair(applicationID, i)];
		}
		CHECK(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 1);
		REQUIRE(applicationProcessConfig.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All report types already allowed") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = ForwardingAndPacketHelper::services[0]; // st03
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::validReportTypes(request);

		for (auto message: AllReportTypes::MessagesOfService.at(serviceType)) {
			packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions[std::make_pair(applicationID, serviceType)]
			    .push_back(message);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) ==
		      2);
		REQUIRE(
		    packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions[std::make_pair(applicationID, serviceType)]
		        .size() == AllReportTypes::MessagesOfService.at(serviceType).size());

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max report types already reached") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType1 = ForwardingAndPacketHelper::services[0]; // st03
		uint8_t serviceType2 = ForwardingAndPacketHelper::services[1]; // st05

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::validReportTypes(request);

		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;

		auto appServicePair1 = std::make_pair(applicationID, serviceType1);
		auto appServicePair2 = std::make_pair(applicationID, serviceType2);
		REQUIRE(definitions[appServicePair1].empty());
		REQUIRE(definitions[appServicePair2].empty());

		auto numOfMessages1 = AllReportTypes::MessagesOfService.at(serviceType1).size();
		auto numOfMessages2 = AllReportTypes::MessagesOfService.at(serviceType2).size();

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

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested addition of duplicate report type definitions") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		ForwardingAndPacketHelper::duplicateReportTypes(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		REQUIRE(definitions.size() == 2);

		for (auto appID: ForwardingAndPacketHelper::applications) {
			for (auto& serviceType: ForwardingAndPacketHelper::services) {
				auto appServicePair = std::make_pair(appID, serviceType);
				REQUIRE(definitions.find(appServicePair) != definitions.end());
				REQUIRE(definitions[appServicePair].size() == 1);
				REQUIRE(std::find(definitions[appServicePair].begin(),
				                  definitions[appServicePair].end(),
				                  ForwardingAndPacketHelper::messages1[0]) != definitions[appServicePair].end());
			}
		}

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validInvalidReportTypes(request);

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions[std::make_pair(applicationID3, i)];
		}
		CHECK(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() ==
		      ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 3);

		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		REQUIRE(definitions.size() == 20);
		for (auto serviceType: ForwardingAndPacketHelper::allServices) {
			REQUIRE(definitions.find(std::make_pair(applicationID1, serviceType)) != definitions.end());
		}

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		for (auto serviceType: ForwardingAndPacketHelper::services) {
			REQUIRE(definitions[std::make_pair(applicationID1, serviceType)].size() ==
			        AllReportTypes::MessagesOfService.at(serviceType).size());
		}

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validInvalidAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);

		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
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

		for (auto& serviceType: ForwardingAndPacketHelper::allServices) {
			REQUIRE(definitions[std::make_pair(applicationID1, serviceType)].size() ==
			        AllReportTypes::MessagesOfService.at(serviceType).size());
		}
		for (auto& serviceType: ForwardingAndPacketHelper::services) {
			REQUIRE(definitions[std::make_pair(applicationID2, serviceType)].size() ==
			        AllReportTypes::MessagesOfService.at(serviceType).size());
		}

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		REQUIRE(definitions.size() == ECSSMaxServiceTypeDefinitions);

		for (auto serviceType: ForwardingAndPacketHelper::allServices) {
			REQUIRE(std::equal(definitions[std::make_pair(applicationID1, serviceType)].begin(),
			                   definitions[std::make_pair(applicationID1, serviceType)].end(),
			                   AllReportTypes::MessagesOfService.at(serviceType).begin()));
		}

		resetAppProcessConfigurationPacketSelection();
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
		ForwardingAndPacketHelper::validInvalidAllReportsOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;

		REQUIRE(definitions.size() == 2 * ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}
}
