#include <iostream>
#include "ECSS_Definitions.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "catch2/catch.hpp"

RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t allServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20};
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
	uint8_t applications2[] = {1, 2, 3};
	// Check if configuration is initialized properly
	for (auto appID: applications2) {
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
	uint8_t applications2[] = {1, 2, 3};
	for (auto appID: applications2) {
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

void resetAppProcessConfiguration() {
	realTimeForwarding.applicationProcessConfiguration.definitions.clear();
	REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Report the the Application Process Configuration content") {
	SECTION("Valid reporting of the application process configuration content") {
		Message request(RealTimeForwardingControlService::ServiceType, RealTimeForwardingControlService::MessageType::ReportAppProcessConfigurationContent, Message::TC, 1);
		initializeAppProcessConfig();

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.serviceType == RealTimeForwardingControlService::ServiceType);
		REQUIRE(report.messageType == RealTimeForwardingControlService::MessageType::AppProcessConfigurationContentReport);
		REQUIRE(report.readUint8() == 3);

		// Application 1
		REQUIRE(report.readUint8() == 1);
		REQUIRE(report.readUint8() == 2);            // num of services
		REQUIRE(report.readUint8() == 3);            // service 1
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 2
		REQUIRE(report.readUint8() == 5);            // service 2
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 1

		// Application 2
		REQUIRE(report.readUint8() == 2);
		REQUIRE(report.readUint8() == 2);            // num of services
		REQUIRE(report.readUint8() == 3);            // service 1
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 2
		REQUIRE(report.readUint8() == 5);            // service 2
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 1

		// Application 3
		REQUIRE(report.readUint8() == 3);
		REQUIRE(report.readUint8() == 2);            // num of services
		REQUIRE(report.readUint8() == 3);            // service 1
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 2
		REQUIRE(report.readUint8() == 5);            // service 2
		REQUIRE(report.readUint8() == 2);            // num of messages
		REQUIRE(report.readUint8() == messages1[0]); // message 1
		REQUIRE(report.readUint8() == messages1[1]); // message 1

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid reporting of the application process configuration content 2") {
		Message request(RealTimeForwardingControlService::ServiceType, RealTimeForwardingControlService::MessageType::ReportAppProcessConfigurationContent, Message::TC, 1);

		for (auto appID: applications) {
			for (auto serviceType: allServices) {
				auto appServicePair = std::make_pair(appID, serviceType);
				for (auto& message: AllMessageTypes::messagesOfService[serviceType]) {
					realTimeForwarding.applicationProcessConfiguration.definitions[appServicePair].push_back(message);
				}
			}
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.serviceType == RealTimeForwardingControlService::ServiceType);
		REQUIRE(report.messageType == RealTimeForwardingControlService::MessageType::AppProcessConfigurationContentReport);
		REQUIRE(report.readUint8() == 1);

		for (auto appID: applications) {
			REQUIRE(report.readUint8() == appID);
			uint8_t numOfServices = 10;
			REQUIRE(report.readUint8() == numOfServices);

			for (auto serviceType: allServices) {
				REQUIRE(report.readUint8() == serviceType);
				REQUIRE(report.readUint8() == AllMessageTypes::messagesOfService[serviceType].size());
				for (auto& message: AllMessageTypes::messagesOfService[serviceType]) {
					REQUIRE(report.readUint8() == message);
				}
			}
		}

		ServiceTests::reset();
		Services.reset();
	}
}
