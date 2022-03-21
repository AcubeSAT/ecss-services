#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "ECSS_Definitions.hpp"
#include "Services/RealTimeForwardingControlService.hpp"

uint8_t applications[] = {1};
uint8_t services[] = {3, 5};
uint8_t messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                       HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};

uint8_t messages2[] = {EventReportService::MessageType::InformativeEventReport,
                       EventReportService::MessageType::DisabledListEventReport};

RealTimeForwardingControlService& realTimeForwarding = Services.realTimeForwarding;

void initialize(Message& request) {
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

void resetAppProcessConfiguration() {
	realTimeForwarding.applicationProcessConfiguration.definitions.clear();
	REQUIRE(realTimeForwarding.applicationProcessConfiguration.definitions.empty());
}

TEST_CASE("Add report types to the application process configuration") {
	SECTION("Successful addition of report  types to the application process configuration") {
		Message request(RealTimeForwardingControlService::ServiceType,
		                RealTimeForwardingControlService::MessageType::AddReportTypesToAppProcessConfiguration,
		                Message::TC, 1);

		uint8_t applicationID = 1;
		realTimeForwarding.controlledApplications.push_back(applicationID);
		initialize(request);

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
		resetAppProcessConfiguration();
		ServiceTests::reset();
		Services.reset();
	}
}
