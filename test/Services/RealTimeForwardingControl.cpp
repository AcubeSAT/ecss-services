#include <iostream>
#include "ECSS_Definitions.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "catch2/catch.hpp"

RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t redundantServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

bool findApplication(uint8_t targetAppID) {
	auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [targetAppID](auto definition) { return targetAppID == definition.first.first; });
}

bool findServiceType(uint8_t applicationID, uint8_t targetService) {
	auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [applicationID, targetService](auto definition) { return applicationID == definition.first.first and targetService == definition.first.second; });
}

void checkAppProcessConfig() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

	// Check if configuration is initialized properly
	for (auto appID: applications) {
		REQUIRE(findApplication(appID));

		for (auto serviceType: services) {
			auto appServicePair = std::make_pair(appID, serviceType);
			REQUIRE(findServiceType(appID, serviceType));
			REQUIRE(applicationProcesses[appServicePair].size() == 2);

			for (auto messageType: messages1) {
				REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
				                  applicationProcesses[appServicePair].end(),
				                  messageType) != applicationProcesses[appServicePair].end());
			}
		}
	}
}

void initializeAppProcessConfig() {
	//	uint8_t numOfApplications = 1;
	//	uint8_t numOfServicesPerApp = 2;
	//	uint8_t numOfMessagesPerService = 2;

	for (auto appID: applications) {
		for (auto serviceType: services) {
			auto appServicePair = std::make_pair(appID, serviceType);
			for (auto messageType: messages1) {
				realTimeForwarding.applicationProcessConfiguration.definitions[appServicePair].push_back(
				    messageType);
			}
		}
	}
	checkAppProcessConfig();
}

void checkAppProcessConfig2() {
	auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
	uint8_t applications2[] = {1, 2, 3};

	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	// Check if configuration is initialized properly
	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		REQUIRE(findApplication(appID));

		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServices : services;

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messages2 : messages1;

			REQUIRE(findServiceType(appID, serviceType));
			auto appServicePair = std::make_pair(appID, serviceType);
			REQUIRE(applicationProcesses[appServicePair].size() == 2);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				REQUIRE(std::find(applicationProcesses[appServicePair].begin(),
				                  applicationProcesses[appServicePair].end(),
				                  messageType) != applicationProcesses[appServicePair].end());
			}
		}
	}
}

void initializeAppProcessConfig2() {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = applications2[i];
		uint8_t numOfServices = (i == 2) ? 15 : 2;
		uint8_t* serviceTypes = (i == 2) ? allServices : services;

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = serviceTypes[j];
			uint8_t* messages = (i == 2) ? messages2 : messages1;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t messageType = messages[k];
				realTimeForwarding.applicationProcessConfiguration.definitions[std::make_pair(appID, serviceType)].push_back(
				    messageType);
			}
		}
	}
	checkAppProcessConfig2();
}

void serviceNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t nonExistingService = 13; // add arbitrary, non-existing service type
			request.appendUint8(nonExistingService);
			request.appendUint8(numOfMessagesPerService);
			uint8_t* messages = (j == 0) ? messages1 : messages2;

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages[k]);
			}
		}
	}
}

void messageNotInApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				uint8_t nonExistingMessage =
				    RealTimeForwardingControlService::MessageType::EventReportConfigurationContentReport;
				request.appendUint8(nonExistingMessage);
			}
		}
	}
}

void deleteValidReportTypes(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 1;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteReportEmptyService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteReportEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID: applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);

			for (uint8_t k = 0; k < numOfMessagesPerService; k++) {
				request.appendUint8(messages1[k]);
			}
		}
	}
}

void deleteApplicationProcess(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);
	request.appendUint8(applications[0]);
	request.appendUint8(numOfServicesPerApp);
}

void deleteService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 1;
	uint8_t numOfMessagesPerService = 0;

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

void deleteServiceEmptyApplication(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

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

void deleteValidInvalidReportTypes(Message& request) {
	uint8_t numOfApplications = 4;
	uint8_t numOfServices = 2;
	uint8_t numOfMessages = 2;
	uint8_t validInvalidApplications[] = {1, 99, 3, 2}; // one invalid and three valid
	uint8_t validInvalidServices[][2] = {{3, 9}, {3, 5}, {1, 8}, {3, 5}};
	uint8_t validInvalidMessages[] = {EventReportService::MessageType::DisabledListEventReport,
	                                  HousekeepingService::MessageType::HousekeepingParametersReport};

	request.appendUint8(numOfApplications);
	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t appID = validInvalidApplications[i];

		request.appendUint8(appID);
		request.appendUint8(numOfServices);

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = validInvalidServices[i][j];
			uint8_t* messages = (i == 2) ? validInvalidMessages : messages1;

			request.appendUint8(serviceType);
			request.appendUint8(numOfMessages);

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = messages[k];
				request.appendUint8(messageType);
			}
		}
	}
}

void resetAppProcessConfiguration() {
	realTimeForwarding.applicationProcessConfiguration.definitions.clear();
	REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Delete report types from the Application Process Configuration") {
	SECTION("Empty the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t numOfApplications = 0;
		request.appendUint8(numOfApplications);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested application, is not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t numOfApplications = 1;
		uint8_t applicationID = 2;
		request.appendUint8(numOfApplications);
		request.appendUint8(applicationID);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingApplication) == 1);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested service type, not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		serviceNotInApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition) == 2);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested report type, not present in the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		messageNotInApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition) ==
		      4);
		checkAppProcessConfig();

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid deletion of report types from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteValidReportTypes(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses[std::make_pair(applicationID, services[0])].size() == 1);
		REQUIRE(applicationProcesses[std::make_pair(applicationID, services[1])].size() == 1);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Report type deletion, results in empty service type") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteReportEmptyService(request);
		initializeAppProcessConfig();

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.size() == 2);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(applicationProcesses.size() == 1);
		REQUIRE(findServiceType(applicationID, services[1]));

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Report type deletion, results in empty application process definition") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteReportEmptyApplication(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete an application process, from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		deleteApplicationProcess(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete a service type, from the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteService(request);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(applicationProcesses.size() == 1);
		REQUIRE(not findServiceType(applicationID, services[0]));

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Service type deletion, results in empty application process") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID = 1;
		deleteServiceEmptyApplication(request);
		initializeAppProcessConfig();
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(not applicationProcesses.empty());

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(applicationProcesses.empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid and invalid requests to delete report types, combined") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::DeleteReportTypesFromAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t remainingApps[] = {1, 3};
		uint8_t remainingMessage[] = {EventReportService::MessageType::InformativeEventReport};

		deleteValidInvalidReportTypes(request);
		initializeAppProcessConfig2();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition) ==
		      2);

		auto& definitions = realTimeForwarding.applicationProcessConfiguration.definitions;

		REQUIRE(definitions.size() == 16);
		for (auto appID: remainingApps) {
			REQUIRE(findApplication(appID));
		}
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [&remainingApps](auto& definition) { return std::find(std::begin(remainingApps), std::end(remainingApps), definition.first.first) != std::end(remainingApps); }) == 16);

		// Check for appID = 1
		uint8_t appID1 = remainingApps[0];
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID1](auto& definition) { return appID1 == definition.first.first; }) == 1);
		REQUIRE(not findServiceType(appID1, services[0]));
		auto appServicePair = std::make_pair(appID1, services[1]);
		REQUIRE(definitions[appServicePair].size() == 2);

		for (auto& message: messages1) {
			REQUIRE(std::find(definitions[appServicePair].begin(),
			                  definitions[appServicePair].end(),
			                  message) != definitions[appServicePair].end());
		}

		// Check for appID = 2
		uint8_t appID2 = 2;
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID2](auto& definition) { return appID2 == definition.first.first; }) == 0);

		// Check for appID = 3
		uint8_t appID3 = remainingApps[1];
		REQUIRE(std::count_if(std::begin(definitions), std::end(definitions), [appID3](auto& definition) { return appID3 == definition.first.first; }) == 15);

		for (uint8_t i = 0; i < 15; i++) {
			uint8_t numOfMessages = (i == 0 or i == 7) ? 1 : 2; // we only deleted one report from services 1 and 8
			uint8_t* messages = (i == 0 or i == 7) ? remainingMessage : messages2;
			auto appServicePair1 = std::make_pair(appID3, allServices[i]);

			REQUIRE(definitions[appServicePair1].size() == numOfMessages);
			for (uint8_t j = 0; j < numOfMessages; j++) {
				REQUIRE(std::find(definitions[appServicePair1].begin(),
				                  definitions[appServicePair1].end(),
				                  messages[j]) != definitions[appServicePair1].end());
			}
		}
		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
