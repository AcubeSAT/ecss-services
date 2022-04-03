#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "ECSS_Definitions.hpp"
#include "Services/RealTimeForwardingControlService.hpp"

RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t redundantServices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

void initializeValid(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 2;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
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

	REQUIRE(request.readUint8() == 1);
	REQUIRE(request.readUint8() == 1);
	REQUIRE(request.readUint8() == 2);
	REQUIRE(request.readUint8() == 3);
	REQUIRE(request.readUint8() == 2);
	REQUIRE(request.readUint8() == HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport);
	REQUIRE(request.readUint8() == HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport);
	REQUIRE(request.readUint8() == 5);
	REQUIRE(request.readUint8() == 2);
	REQUIRE(request.readUint8() == EventReportService::MessageType::InformativeEventReport);
	REQUIRE(request.readUint8() == EventReportService::MessageType::DisabledListEventReport);
	request.resetRead();
}

void initializeCombined(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 17 : 2;
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

void allReportsOfService(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 2;
	uint8_t numOfMessagesPerService = 0;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);

		for (uint8_t j = 0; j < numOfServicesPerApp; j++) {
			uint8_t serviceType = services[j];
			request.appendUint8(serviceType);
			request.appendUint8(numOfMessagesPerService);
		}
	}
}

void allReportsOfServiceCombined(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
	request.appendUint8(numOfApplications);

	for (uint8_t i = 0; i < numOfApplications; i++) {
		request.appendUint8(applications2[i]);
		uint8_t numOfServicesPerApp = (i == 0) ? 17 : 2;
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

void allReportTypesOfApp(Message& request) {
	uint8_t numOfApplications = 1;
	uint8_t numOfServicesPerApp = 0;

	request.appendUint8(numOfApplications);

	for (auto appID : applications) {
		request.appendUint8(appID);
		request.appendUint8(numOfServicesPerApp);
	}
}

void allReportsOfAppCombined(Message& request) {
	uint8_t numOfApplications = 3;
	uint8_t numOfMessagesPerService = 2;

	uint8_t applications2[] = {1, 2, 3};
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

void resetAppProcessConfiguration() {
	realTimeForwarding.applicationProcessConfiguration.definitions.clear();
	REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Add report types to the Application Process Configuration") {
	SECTION("Successful addition of report types to the Application Process Configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initializeValid(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 1);

		for (auto appID : applications) {
			REQUIRE(applicationProcesses.find(appID) != applicationProcesses.end());
			REQUIRE(applicationProcesses[appID].size() == 2);

			for (uint8_t j = 0; j < 2; j++) {
				uint8_t serviceType = services[j];
				REQUIRE(applicationProcesses[appID].find(serviceType) != applicationProcesses[appID].end());
				REQUIRE(applicationProcesses[appID][serviceType].size() == 2);
				uint8_t* messages = (j == 0) ? messages1 : messages2;

				for (uint8_t k = 0; k < 2; k++) {
					REQUIRE(std::find(applicationProcesses[appID][serviceType].begin(),
					                  applicationProcesses[appID][serviceType].end(),
					                  messages[k]) != applicationProcesses[appID][serviceType].end());
				}
			}
		}

		auto& servicesAllowed = realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty;
		REQUIRE(servicesAllowed[applicationID].size() == 2);
		REQUIRE(servicesAllowed[applicationID][services[0]] == true);
		REQUIRE(servicesAllowed[applicationID][services[1]] == true);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Requested Application Process is not controlled by the service") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		initializeValid(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty[applicationID].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All service types already allowed") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initializeValid(request);

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].empty());
		realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty[applicationID][services[0]] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max service types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initializeValid(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration;

		for (auto service : allServices) {
			applicationProcessConfig.definitions[applicationID][service].clear();
			applicationProcessConfig.serviceNotEmpty[applicationID][service] = true;
		}
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 15);
		REQUIRE(applicationProcessConfig.serviceNotEmpty[applicationID].size() == 15);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 15);
		REQUIRE(applicationProcessConfig.serviceNotEmpty[applicationID].size() == 15);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All report types already allowed") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0];
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initializeValid(request);

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID][serviceType].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID][serviceType].empty());
		realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty[applicationID][serviceType] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllReportTypesAlreadyAllowed) ==
		      1);
		REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID].size() == 2);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Max report types already reached") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		uint8_t serviceType = services[0];
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initializeValid(request);

		auto& applicationProcessConfig = realTimeForwarding.applicationProcessConfiguration;

		applicationProcessConfig.serviceNotEmpty[applicationID][serviceType] = true;
		for (uint8_t i = 0; i < ECSSMaxReportTypeDefinitions; i++) {
			applicationProcessConfig.definitions[applicationID][serviceType].push_back(i);
		}

		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 1);
		REQUIRE(applicationProcessConfig.serviceNotEmpty[applicationID].size() == 1);
		REQUIRE(applicationProcessConfig.definitions[applicationID][serviceType].size() ==
		        ECSSMaxReportTypeDefinitions);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached) == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID].size() == 2);
		REQUIRE(applicationProcessConfig.serviceNotEmpty[applicationID].size() == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID][services[1]].size() == 2);
		REQUIRE(applicationProcessConfig.definitions[applicationID][serviceType].size() ==
		        ECSSMaxReportTypeDefinitions);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid and invalid application-related requests combined") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		uint8_t applicationID3 = 3;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID3);
		initializeCombined(request);

		realTimeForwarding.applicationProcessConfiguration.definitions[applicationID3].clear();
		CHECK(realTimeForwarding.applicationProcessConfiguration.definitions[applicationID3].empty());
		realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty[applicationID3][services[0]] = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed) ==
		      1);

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		auto& servicesAllowed = realTimeForwarding.applicationProcessConfiguration.serviceNotEmpty;

		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses.find(applicationID2) == applicationProcesses.end());
		REQUIRE(applicationProcesses.find(applicationID1) != applicationProcesses.end());
		REQUIRE(applicationProcesses.find(applicationID3) != applicationProcesses.end());

		REQUIRE(applicationProcesses[applicationID1].size() == 15);
		REQUIRE(servicesAllowed[applicationID1].size() == 15);
		for (auto serviceType : allServices) {
			REQUIRE(servicesAllowed[applicationID1][serviceType]);
		}
		REQUIRE(applicationProcesses[applicationID3].empty());

		for (auto& serviceType : applicationProcesses[applicationID1]) {
			REQUIRE(std::find(std::begin(allServices), std::end(allServices), serviceType.first) !=
			        std::end(allServices));
			REQUIRE(serviceType.second.size() == 2);
			auto& messagesToCheck = (serviceType.first == 1) ? messages1 : messages2;
			for (auto message : serviceType.second) {
				REQUIRE(std::find(std::begin(messagesToCheck), std::end(messagesToCheck), message) !=
				        std::end(messagesToCheck));
			}
		}

		REQUIRE(servicesAllowed[applicationID3].size() == 1);
		REQUIRE(servicesAllowed[applicationID3][services[0]] == true);

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of a specified service type") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		allReportsOfService(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses[applicationID1].size() == 2);
		for (auto serviceType : services) {
			REQUIRE(applicationProcesses[applicationID1].find(serviceType) !=
			        applicationProcesses[applicationID1].end());
			REQUIRE(applicationProcesses[applicationID1][serviceType].empty());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of a service type, combined with invalid requests") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID2);
		allReportsOfServiceCombined(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached) == 2);

		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses.size() == 2);
		REQUIRE(applicationProcesses[applicationID1].size() == 15);
		REQUIRE(applicationProcesses[applicationID2].size() == 2);

		for (auto& serviceType: applicationProcesses[applicationID1]) {
			REQUIRE(serviceType.second.empty());
		}
		for (auto& serviceType: applicationProcesses[applicationID2]) {
			REQUIRE(serviceType.second.empty());
		}

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid addition of all report types of an application process") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		allReportTypesOfApp(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses[applicationID1].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Addition of all report types of an application process, combined with invalid request") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);
		uint8_t applicationID1 = 1;
		uint8_t applicationID2 = 2;
		realTimeForwarding.controlledApplications.push_back(applicationID1);
		realTimeForwarding.controlledApplications.push_back(applicationID2);
		allReportsOfAppCombined(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::NotControlledApplication) == 1);
		auto& applicationProcesses = realTimeForwarding.applicationProcessConfiguration.definitions;
		REQUIRE(applicationProcesses[applicationID1].empty());
		REQUIRE(applicationProcesses[applicationID2].empty());

		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
