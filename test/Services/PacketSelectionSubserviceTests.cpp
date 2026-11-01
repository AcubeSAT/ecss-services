#include <limits>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"
#include "catch2/catch_all.hpp"
#include "ForwardingAndPacketHelper.hpp"

StorageAndRetrievalService& storageAndRetrieval1 = Services.storageAndRetrieval;
PacketSelectionSubservice& packetSelection = storageAndRetrieval1.packetSelection;

String<ECSSPacketStoreIdSize> addPacketStoreToPacketSelection(uint8_t nameSuffix = '1') {
	uint8_t packetStoreData[ECSSPacketStoreIdSize];
	uint8_t name[4] = "ps1";
	name[2] = nameSuffix;
	std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);

	std::copy(name, name + 3, packetStoreData);
	String<ECSSPacketStoreIdSize> packetStoreID(packetStoreData);
	storageAndRetrieval1.addPacketStore(packetStoreID, PacketStore());
	return packetStoreID;
}

void resetAppProcessConfigurationPacketSelection() {
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
		request.appendFixedString(packetStoreID);
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
		uint8_t packetStoreData[ECSSPacketStoreIdSize] = {0};
		String<ECSSPacketStoreIdSize> nonExistentPacketStoreID(packetStoreData);
		request.appendFixedString(nonExistentPacketStoreID);
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
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validReportTypes(request);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		REQUIRE(packetSelection.packetStoreAppProcessConfig.size() == 1);

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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
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

	SECTION("Report type of an already added service type, after the max service types are reached") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		ServiceTypeNum newServiceType = ForwardingAndPacketHelper::services[0]; // st03
		ServiceTypeNum existingServiceType = ForwardingAndPacketHelper::services[1]; // st05

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);

		auto& configuration = packetSelection.packetStoreAppProcessConfig[packetStoreID];
		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 98; i++) {
			configuration.definitions[std::make_pair(applicationID, i)];
		}
		configuration.definitions[std::make_pair(applicationID, existingServiceType)].push_back(ForwardingAndPacketHelper::messages2[0]);
		REQUIRE(configuration.definitions.size() == ECSSMaxServiceTypeDefinitions - 1);

		request.appendFixedString(packetStoreID);
		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(2); // numOfServices
		request.append<ServiceTypeNum>(newServiceType); // reaches the maximum
		request.appendUint8(1); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[0]);
		request.append<ServiceTypeNum>(existingServiceType);
		request.appendUint8(1); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages2[1]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		CHECK(configuration.isReportTypeAdded(applicationID, newServiceType, ForwardingAndPacketHelper::messages1[0]));
		CHECK(configuration.isReportTypeAdded(applicationID, existingServiceType, ForwardingAndPacketHelper::messages2[1]));

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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validInvalidReportTypes(request);

		for (uint8_t i = 100; i < ECSSMaxServiceTypeDefinitions + 99; i++) {
			packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions[std::make_pair(applicationID3, i)];
		}
		CHECK(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() ==
		      ECSSMaxServiceTypeDefinitions - 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 1);
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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validInvalidAllReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 0);

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
		request.appendFixedString(packetStoreID);
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
		request.appendFixedString(packetStoreID);
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

	SECTION("Requesting all report types of a service type again") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validAllReportsOfService(request);

		MessageParser::execute(request);
		request.resetRead();
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		for (auto serviceType: ForwardingAndPacketHelper::services) {
			REQUIRE(definitions[std::make_pair(applicationID, serviceType)].size() ==
			        AllReportTypes::MessagesOfService.at(serviceType).size());
		}

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requesting all report types of a service type replaces its definition") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[0]; // st03
		MessageTypeNum meaninglessMessageType = 200;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		auto& configuration = packetSelection.packetStoreAppProcessConfig[packetStoreID];
		configuration.definitions[std::make_pair(applicationID, serviceType)].push_back(meaninglessMessageType);

		request.appendFixedString(packetStoreID);
		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(1); // numOfServices
		request.append<ServiceTypeNum>(serviceType);
		request.appendUint8(0); // numOfMessages = 0, requesting all message types

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		CHECK(not configuration.isReportTypeAdded(applicationID, serviceType, meaninglessMessageType));
		REQUIRE(configuration.countReportsOfService(applicationID, serviceType) ==
		        AllReportTypes::MessagesOfService.at(serviceType).size());

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requesting all report types of an application process again") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validAllReportsOfApp(request);

		MessageParser::execute(request);
		request.resetRead();
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() == ECSSMaxServiceTypeDefinitions);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested service type that does not exist, with explicit message types") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		ServiceTypeNum nonExistentServiceType = 99;
		ServiceTypeNum validServiceType = ForwardingAndPacketHelper::services[0]; // st03

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		request.appendFixedString(packetStoreID);

		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(2); // numOfServices
		request.append<ServiceTypeNum>(nonExistentServiceType);
		request.appendUint8(2); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[0]);
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[1]);
		request.append<ServiceTypeNum>(validServiceType);
		request.appendUint8(2); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[0]);
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[1]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition) == 1);

		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		REQUIRE(definitions.size() == 1);
		REQUIRE(definitions.find(std::make_pair(applicationID, nonExistentServiceType)) == definitions.end());
		REQUIRE(definitions[std::make_pair(applicationID, validServiceType)].size() == 2);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested service type that does not exist, asking for all message types to be added (N3 = 0)") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		ServiceTypeNum nonExistentServiceType = 99;

		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		request.appendFixedString(packetStoreID);

		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(1); // numOfServices
		request.append<ServiceTypeNum>(nonExistentServiceType);
		request.appendUint8(0); // numOfMessages = 0, requesting all message types

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition) == 1);
		REQUIRE(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.empty());

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delete report types from the packet selection subservice") {
	auto populatePacketSelection = [](const String<ECSSPacketStoreIdSize>& packetStoreID, ApplicationProcessId applicationID) {
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		for (uint8_t serviceIndex = 0; serviceIndex < 2; serviceIndex++) {
			ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[serviceIndex];
			auto& messages = (serviceIndex == 0) ? ForwardingAndPacketHelper::messages1 : ForwardingAndPacketHelper::messages2;
			for (uint8_t messageIndex = 0; messageIndex < 2; messageIndex++) {
				definitions[std::make_pair(applicationID, serviceType)].push_back(messages[messageIndex]);
			}
		}
	};

	SECTION("Requested packet store, not present in the storage and retrieval subservice") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = {0};
		String<ECSSPacketStoreIdSize> nonExistentPacketStoreID(packetStoreData);
		request.appendFixedString(nonExistentPacketStoreID);
		request.appendUint8(0);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore) == 1);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Empty the application process storage-control configuration (N1 = 0)") {
		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		populatePacketSelection(packetStoreID, applicationID);
		REQUIRE(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.size() == 2);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		request.appendUint8(0); // numOfApplications = 0, emptying the configuration

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions.empty());

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete a report type, with a deletion resulting in an empty service type definition") {
		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		populatePacketSelection(packetStoreID, applicationID);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[0]; // st03
		auto appServicePair = std::make_pair(applicationID, serviceType);
		REQUIRE(definitions[appServicePair].size() == 2);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(1); // numOfServices
		request.append<ServiceTypeNum>(serviceType);
		request.appendUint8(1); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[0]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(definitions[appServicePair].size() == 1);
		REQUIRE(std::find(definitions[appServicePair].begin(), definitions[appServicePair].end(),
		                  ForwardingAndPacketHelper::messages1[1]) != definitions[appServicePair].end());

		Message secondRequest(StorageAndRetrievalService::ServiceType,
		                      StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                      Message::TC, 1);
		secondRequest.appendFixedString(packetStoreID);
		secondRequest.appendUint8(1); // numOfApplications
		secondRequest.append<ApplicationProcessId>(applicationID);
		secondRequest.appendUint8(1); // numOfServices
		secondRequest.append<ServiceTypeNum>(serviceType);
		secondRequest.appendUint8(1); // numOfMessages
		secondRequest.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[1]);

		MessageParser::execute(secondRequest);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(definitions.find(appServicePair) == definitions.end());
		REQUIRE(definitions.size() == 1);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete a whole service type definition (N3 = 0)") {
		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		populatePacketSelection(packetStoreID, applicationID);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[0]; // st03

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(1); // numOfServices
		request.append<ServiceTypeNum>(serviceType);
		request.appendUint8(0); // numOfMessages = 0, deleting the whole service type definition

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(definitions.find(std::make_pair(applicationID, serviceType)) == definitions.end());
		REQUIRE(definitions.size() == 1);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete a whole application process definition (N2 = 0)") {
		ApplicationProcessId applicationID = 1;
		ApplicationProcessId otherApplicationID = 2;
		auto packetStoreID = addPacketStoreToPacketSelection();
		populatePacketSelection(packetStoreID, applicationID);
		populatePacketSelection(packetStoreID, otherApplicationID);
		auto& configuration = packetSelection.packetStoreAppProcessConfig[packetStoreID];
		auto& definitions = configuration.definitions;
		definitions[std::make_pair(applicationID, std::numeric_limits<ServiceTypeNum>::min())];
		definitions[std::make_pair(applicationID, std::numeric_limits<ServiceTypeNum>::max())];
		REQUIRE(definitions.size() == 6);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		request.appendUint8(1); // numOfApplications
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(0); // numOfServices = 0, deleting the whole application process definition

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		CHECK(not configuration.isApplicationEnabled(applicationID));
		CHECK(configuration.isApplicationEnabled(otherApplicationID));
		REQUIRE(definitions.size() == 2);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Deleting an application process that was not validated beforehand") {
		ApplicationProcessConfiguration configuration;
		configuration.definitions[std::make_pair(ApplicationProcessId{2}, ForwardingAndPacketHelper::services[0])];

		configuration.deleteApplicationProcess(1);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::ElementNotInArray) == 1);
		REQUIRE(configuration.definitions.size() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Deleting a report type that was not validated beforehand") {
		ApplicationProcessId applicationID = 1;
		ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[0];
		ApplicationProcessConfiguration configuration;
		configuration.definitions[std::make_pair(applicationID, serviceType)].push_back(ForwardingAndPacketHelper::messages1[0]);

		configuration.deleteReportType(applicationID, serviceType, ForwardingAndPacketHelper::messages1[1]);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::ElementNotInArray) == 1);
		REQUIRE(configuration.isReportTypeAdded(applicationID, serviceType, ForwardingAndPacketHelper::messages1[0]));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested deletions of non-existent definitions") {
		ApplicationProcessId applicationID = 1;
		ApplicationProcessId nonExistentApplicationID = 2;
		ServiceTypeNum nonExistentServiceType = 4; // exists in AllReportTypes, but not in the configuration
		MessageTypeNum nonExistentMessageType = 99;

		auto packetStoreID = addPacketStoreToPacketSelection();
		populatePacketSelection(packetStoreID, applicationID);
		auto& definitions = packetSelection.packetStoreAppProcessConfig[packetStoreID].definitions;
		ServiceTypeNum serviceType = ForwardingAndPacketHelper::services[0]; // st03

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		request.appendUint8(2); // numOfApplications
		request.append<ApplicationProcessId>(nonExistentApplicationID);
		request.appendUint8(1); // numOfServices
		request.append<ServiceTypeNum>(serviceType);
		request.appendUint8(1); // numOfMessages
		request.append<MessageTypeNum>(ForwardingAndPacketHelper::messages1[0]);
		request.append<ApplicationProcessId>(applicationID);
		request.appendUint8(2); // numOfServices
		request.append<ServiceTypeNum>(nonExistentServiceType);
		request.appendUint8(0); // numOfMessages
		request.append<ServiceTypeNum>(serviceType);
		request.appendUint8(1); // numOfMessages
		request.append<MessageTypeNum>(nonExistentMessageType);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistentReportTypeDefinition) == 1);
		REQUIRE(definitions.size() == 2);
		REQUIRE(definitions[std::make_pair(applicationID, serviceType)].size() == 2);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Report the content of the application process storage-control configuration") {
	SECTION("Requested packet store, not present in the storage and retrieval subservice") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportApplicationProcess,
		                Message::TC, 1);

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = {0};
		String<ECSSPacketStoreIdSize> nonExistentPacketStoreID(packetStoreData);
		request.appendFixedString(nonExistentPacketStoreID);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore) == 1);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid content report of the application process storage-control configuration") {
		Message addRequest(StorageAndRetrievalService::ServiceType,
		                   StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                   Message::TC, 1);

		ApplicationProcessId applicationID = 1;
		auto packetStoreID = addPacketStoreToPacketSelection();
		packetSelection.controlledApplications.push_back(applicationID);
		addRequest.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validReportTypes(addRequest);
		MessageParser::execute(addRequest);
		CHECK(ServiceTests::count() == 0);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportApplicationProcess,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.serviceType == StorageAndRetrievalService::ServiceType);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::ApplicationProcessReport);

		uint8_t reportedPacketStoreId[ECSSPacketStoreIdSize + 1] = {0};
		report.readString(reportedPacketStoreId, ECSSPacketStoreIdSize);
		CHECK(String<ECSSPacketStoreIdSize>(reportedPacketStoreId) == packetStoreID);

		CHECK(report.readUint8() == 1); // numOfApplications
		CHECK(report.read<ApplicationProcessId>() == applicationID);
		CHECK(report.readUint8() == 2); // numOfServices

		CHECK(report.read<ServiceTypeNum>() == ForwardingAndPacketHelper::services[0]);
		CHECK(report.readUint8() == 2); // numOfMessages
		CHECK(report.read<MessageTypeNum>() == ForwardingAndPacketHelper::messages1[0]);
		CHECK(report.read<MessageTypeNum>() == ForwardingAndPacketHelper::messages1[1]);

		CHECK(report.read<ServiceTypeNum>() == ForwardingAndPacketHelper::services[1]);
		CHECK(report.readUint8() == 2); // numOfMessages
		CHECK(report.read<MessageTypeNum>() == ForwardingAndPacketHelper::messages2[0]);
		CHECK(report.read<MessageTypeNum>() == ForwardingAndPacketHelper::messages2[1]);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Packet selection requests shorter than a packet store ID") {
	auto messageType = GENERATE(StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
	                            StorageAndRetrievalService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
	                            StorageAndRetrievalService::MessageType::ReportApplicationProcess);

	Message request(StorageAndRetrievalService::ServiceType, messageType, Message::TC, 1);
	request.appendUint8(1);

	MessageParser::execute(request);

	CHECK(ServiceTests::thrownError(ErrorHandler::AcceptanceErrorType::MessageTooShort));
	CHECK(packetSelection.packetStoreAppProcessConfig.empty());

	ServiceTests::reset();
	Services.reset();
}

TEST_CASE("Deleting a packet store with a request shorter than a packet store ID") {
	Message request(StorageAndRetrievalService::ServiceType,
	                StorageAndRetrievalService::MessageType::DeletePacketStores,
	                Message::TC, 1);
	request.appendUint16(1);

	MessageParser::execute(request);

	CHECK(ServiceTests::thrownError(ErrorHandler::AcceptanceErrorType::MessageTooShort));

	ServiceTests::reset();
	Services.reset();
}

TEST_CASE("Deleting a packet store deletes its application process storage-control configuration") {
	auto addReportTypes = [](const String<ECSSPacketStoreIdSize>& packetStoreID) {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		request.appendFixedString(packetStoreID);
		ForwardingAndPacketHelper::validReportTypes(request);
		MessageParser::execute(request);
	};

	auto deletePacketStore = [](const String<ECSSPacketStoreIdSize>& packetStoreID) {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores,
		                Message::TC, 1);
		request.appendUint16(1);
		request.appendFixedString(packetStoreID);
		MessageParser::execute(request);
	};

	auto& configurations = packetSelection.packetStoreAppProcessConfig;
	packetSelection.controlledApplications.push_back(ForwardingAndPacketHelper::applications[0]);

	SECTION("A recreated packet store starts with an empty configuration") {
		auto packetStoreID = addPacketStoreToPacketSelection();
		addReportTypes(packetStoreID);
		REQUIRE(configurations[packetStoreID].definitions.size() == 2);

		deletePacketStore(packetStoreID);
		CHECK(not storageAndRetrieval1.packetStoreExists(packetStoreID));
		CHECK(configurations.empty());

		addPacketStoreToPacketSelection();
		CHECK(configurations[packetStoreID].definitions.empty());
		CHECK(ServiceTests::count() == 0);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Deleting all packet stores (N = 0)") {
		addReportTypes(addPacketStoreToPacketSelection('1'));
		addReportTypes(addPacketStoreToPacketSelection('2'));
		REQUIRE(configurations.size() == 2);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores,
		                Message::TC, 1);
		request.appendUint16(0);
		MessageParser::execute(request);

		CHECK(storageAndRetrieval1.currentNumberOfPacketStores() == 0);
		CHECK(configurations.empty());
		CHECK(ServiceTests::count() == 0);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Going through more packet stores than can exist at the same time") {
		for (uint16_t store = 0; store <= ECSSMaxPacketStores; store++) {
			auto packetStoreID = addPacketStoreToPacketSelection(static_cast<uint8_t>('a' + store));
			addReportTypes(packetStoreID);
			REQUIRE(configurations[packetStoreID].definitions.size() == 2);

			deletePacketStore(packetStoreID);
			REQUIRE(configurations.empty());
		}
		CHECK(ServiceTests::count() == 0);

		resetAppProcessConfigurationPacketSelection();
		ServiceTests::reset();
		Services.reset();
	}
}
