#include <catch2/catch.hpp>
#include <Services/EventReportService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <cstring>

/*
 * @todo Change the reinterpret_cast
 */
TEST_CASE("Informative Event Report TM[5,1]", "[service][st05]") {
	EventReportService eventReportService;
	const unsigned char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.informativeEventReport(EventReportService::InformativeUnknownEvent,
	                                          eventReportData, 10);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 1);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 0);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, reinterpret_cast<const char *>(eventReportData)) == 0);
}

TEST_CASE("Low Severity Anomaly Report TM[5,2]", "[service][st05]") {
	EventReportService eventReportService;
	const unsigned char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.lowSeverityAnomalyReport(EventReportService::LowSeverityUnknownEvent,
	                                            eventReportData, 10);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 2);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 4);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, reinterpret_cast<const char *>(eventReportData)) == 0);
}

TEST_CASE("Medium Severity Anomaly Report TM[5,3]", "[service][st05]") {
	EventReportService eventReportService;
	const unsigned char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.mediumSeverityAnomalyReport
		(EventReportService::MediumSeverityUnknownEvent, eventReportData, 10);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 3);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 5);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, reinterpret_cast<const char *>(eventReportData)) == 0);
}

TEST_CASE("High Severity Anomaly Report TM[5,4]", "[service][st05]") {
	EventReportService eventReportService;
	const unsigned char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent,
	                                             eventReportData, 10);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 4);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 6);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, reinterpret_cast<const char *>(eventReportData)) == 0);
}

TEST_CASE("Enable Report Generation TC[5,5]", "[service][st05]") {
	EventReportService eventReportService;
	eventReportService.getStateOfEvents().reset();
	EventReportService::Event eventID[] = {EventReportService::AssertionFail,
	                                       EventReportService::LowSeverityUnknownEvent};
	eventReportService.enableReportGeneration(2, eventID);
	CHECK(eventReportService.getStateOfEvents()[2] == 1);
	CHECK(eventReportService.getStateOfEvents()[4] == 1);
}

TEST_CASE("Disable Report Generation TC[5,6]", "[service][st05]") {
	EventReportService eventReportService;
	EventReportService::Event eventID[] = {EventReportService::InformativeUnknownEvent,
	                                       EventReportService::MediumSeverityUnknownEvent};
	eventReportService.disableReportGeneration(2, eventID);
	CHECK(eventReportService.getStateOfEvents()[0] == 0);
	CHECK(eventReportService.getStateOfEvents()[5] == 0);

	const unsigned char eventReportData[] = "HelloWorld";
	eventReportService.highSeverityAnomalyReport(EventReportService::InformativeUnknownEvent,
	                                             eventReportData, 10);
	CHECK(ServiceTests::hasOneMessage() == false);
}

TEST_CASE("Request list of disabled events TC[5,7]", "[service][st05]") {
	EventReportService eventReportService;
	eventReportService.requestListOfDisabledEvents();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Check if there is message of type 8 created
	CHECK(report.messageType == 8);
}

TEST_CASE("List of Disabled Events Report TM[5,8]", "[service][st05]") {
	EventReportService eventReportService;
	EventReportService::Event eventID[] = {EventReportService::MCUStart,
	                                       EventReportService::HighSeverityUnknownEvent};
	// Disable 3rd and 6th
	eventReportService.disableReportGeneration(2, eventID);
	eventReportService.listOfDisabledEventsReport();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Check for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 8);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 6);
	// Check for the information stored in report
	CHECK(report.readHalfword() == 2);
	CHECK(report.readEnum16() == 3);
	CHECK(report.readEnum16() == 6);
}

TEST_CASE("List of observables 6.5.6", "[service][st05]") {
	EventReportService eventReportService;
	EventReportService::Event eventID[] = {EventReportService::HighSeverityUnknownEvent};
	eventReportService.disableReportGeneration(1, eventID);

	const unsigned char eventReportData[] = "HelloWorld";

	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent,
	                                             eventReportData,
	                                             10);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::MediumSeverityUnknownEvent,
		                                         eventReportData,
		                                         10);
	CHECK(eventReportService.lowSeverityReportCount == 0);
	CHECK(eventReportService.mediumSeverityReportCount == 1);
	CHECK(eventReportService.highSeverityReportCount == 0);

	CHECK(eventReportService.lowSeverityEventCount == 0);
	CHECK(eventReportService.mediumSeverityEventCount == 1);
	CHECK(eventReportService.highSeverityEventCount == 1);

	CHECK(eventReportService.disabledEventsCount == 1);

	CHECK(eventReportService.lastLowSeverityReportID == 65535);
	CHECK(eventReportService.lastMediumSeverityReportID == 5);
	CHECK(eventReportService.lastHighSeverityReportID == 65535);
}
