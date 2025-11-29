#include <Message.hpp>
#include <Services/EventReportService.hpp>
#include <catch2/catch_all.hpp>
#include <cstring>
#include "ServiceTests.hpp"

EventReportService& eventReportService = Services.eventReport;

TEST_CASE("Informative Event Report TM[5,1]", "[service][st05]") {
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.informativeEventReport(EventReportService::UnknownEvent, eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == EventReportService::ServiceType);
	CHECK(report.messageType == EventReportService::MessageType::InformativeEventReport);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.read<EventDefinitionId>() == 1);
	report.readCString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("Low Severity Anomaly Report TM[5,2]", "[service][st05]") {
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.lowSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == EventReportService::ServiceType);
	CHECK(report.messageType == EventReportService::MessageType::LowSeverityAnomalyReport);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.read<EventDefinitionId>() == 1);
	report.readCString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("Medium Severity Anomaly Report TM[5,3]", "[service][st05]") {
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.mediumSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == EventReportService::ServiceType);
	CHECK(report.messageType == EventReportService::MessageType::MediumSeverityAnomalyReport);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.read<EventDefinitionId>() == 1);
	report.readCString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("High Severity Anomaly Report TM[5,4]", "[service][st05]") {
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.highSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == EventReportService::ServiceType);
	CHECK(report.messageType == EventReportService::MessageType::HighSeverityAnomalyReport);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.read<EventDefinitionId>() == 1);
	report.readCString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("Enable Report Generation TC[5,5]", "[service][st05]") {
	eventReportService.disableAllEvents();
	EventReportService::Event eventID[] = {EventReportService::AssertionFail,
	                                       EventReportService::UnknownEvent};
	Message message(EventReportService::ServiceType, EventReportService::MessageType::EnableReportGenerationOfEvents, Message::TC, 1);
	message.appendUint16(2);
	message.append<EventDefinitionId>(eventID[0]);
	message.append<EventDefinitionId>(eventID[1]);
	MessageParser::execute(message);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::AssertionFail] == 1);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::UnknownEvent] == 1);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::MCUStart] == 0);
}
TEST_CASE("Disable All Event Report Generation") {
	auto eventStates = eventReportService.getStateOfEvents();
	for (size_t i = 0; i < eventStates.size(); i++) {
		CHECK(eventStates[i] == true);
	}
	eventReportService.disableAllEvents();
	eventStates = eventReportService.getStateOfEvents();
	for (size_t i = 0; i < eventStates.size(); i++) {
		CHECK(eventStates[i] == false);
	}
}
TEST_CASE("Disable Report Generation TC[5,6]", "[service][st05]") {
	EventReportService::Event eventID[] = {EventReportService::UnknownEvent, EventReportService::MCUStart};
	Message message(EventReportService::ServiceType, EventReportService::MessageType::DisableReportGenerationOfEvents, Message::TC, 1);
	message.appendUint16(2);
	message.append<EventDefinitionId>(eventID[0]);
	message.append<EventDefinitionId>(eventID[1]);
	MessageParser::execute(message);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::UnknownEvent] == 0);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::MCUStart] == 0);
	CHECK(eventReportService.getStateOfEvents()[EventReportService::AssertionFail] == 1);

	const String<64> eventReportData = "HelloWorld";
	eventReportService.highSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	CHECK(ServiceTests::hasOneMessage() == false);
}

TEST_CASE("Request list of disabled events TC[5,7]", "[service][st05]") {
	Message message(EventReportService::ServiceType, EventReportService::MessageType::ReportListOfDisabledEvents,
	                Message::TC, 1);
	MessageParser::execute(message);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Check if there is message of type 8 created
	CHECK(report.messageType == EventReportService::MessageType::DisabledListEventReport);
}

TEST_CASE("List of Disabled Events Report TM[5,8]", "[service][st05]") {
	EventReportService::Event eventID[] = {EventReportService::MCUStart, EventReportService::UnknownEvent};
	Message message(EventReportService::ServiceType, EventReportService::MessageType::DisableReportGenerationOfEvents, Message::TC, 1);
	message.appendUint16(2);
	message.append<EventDefinitionId>(eventID[0]);
	message.append<EventDefinitionId>(eventID[1]);
	// Disable 3rd and 6th
	MessageParser::execute(message);
	eventReportService.listOfDisabledEventsReport();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Check for the data-members of the report Message created
	CHECK(report.serviceType == EventReportService::ServiceType);
	CHECK(report.messageType == EventReportService::MessageType::DisabledListEventReport);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 6);
	// Check for the information stored in report
	CHECK(report.readHalfword() == 2);
	CHECK(report.read<EventDefinitionId>() == 1);
	CHECK(report.read<EventDefinitionId>() == 4);
}

TEST_CASE("List of observables 6.5.6", "[service][st05]") {
	EventReportService::Event eventID[] = {EventReportService::UnknownEvent, EventReportService::MCUStart};
	Message message(EventReportService::ServiceType, EventReportService::MessageType::DisableReportGenerationOfEvents, Message::TC, 1);
	message.appendUint16(1);
	message.append<EventDefinitionId>(eventID[0]);
	MessageParser::execute(message);

	const String<64> eventReportData = "HelloWorld";

	eventReportService.highSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::MCUStart, eventReportData);
	CHECK(eventReportService.lowSeverityReportCount == 0);
	CHECK(eventReportService.mediumSeverityReportCount == 1);
	CHECK(eventReportService.highSeverityReportCount == 0);

	CHECK(eventReportService.lowSeverityEventCount == 0);
	CHECK(eventReportService.mediumSeverityEventCount == 1);
	CHECK(eventReportService.highSeverityEventCount == 1);

	CHECK(eventReportService.disabledEventsCount == 1);

	CHECK(eventReportService.lastLowSeverityReportID == 65535);
	CHECK(eventReportService.lastMediumSeverityReportID == 4);
	CHECK(eventReportService.lastHighSeverityReportID == 65535);

}
