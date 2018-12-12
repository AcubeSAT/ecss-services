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
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.informativeEventReport(EventReportService::InformativeUnknownEvent,
	                                          eventReportData);
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
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("Low Severity Anomaly Report TM[5,2]", "[service][st05]") {
	EventReportService eventReportService;
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.lowSeverityAnomalyReport(EventReportService::LowSeverityUnknownEvent,
	                                            eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 2);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 1);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("Medium Severity Anomaly Report TM[5,3]", "[service][st05]") {
	EventReportService eventReportService;
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.mediumSeverityAnomalyReport
		(EventReportService::MediumSeverityUnknownEvent, eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 3);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 2);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}

TEST_CASE("High Severity Anomaly Report TM[5,4]", "[service][st05]") {
	EventReportService eventReportService;
	const char eventReportData[] = "HelloWorld";
	char checkString[255];
	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent,
	                                             eventReportData);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	// Checks for the data-members of the report Message created
	CHECK(report.serviceType == 5);
	CHECK(report.messageType == 4);
	CHECK(report.packetType == Message::TM); // packet type(TM = 0, TC = 1)
	REQUIRE(report.dataSize == 12);
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnum16() == 3);
	report.readString(checkString, 10);
	CHECK(strcmp(checkString, eventReportData) == 0);
}
