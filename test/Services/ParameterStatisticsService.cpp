#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/ParameterService.hpp"

/**
 * System-statistics initialization, so there are actual statistics in the map to work with.
 */
void initializeStatistics(uint16_t interval1, uint16_t interval2) {
	Statistic stat1;
	Statistic stat2;
	stat1.selfSamplingInterval = interval1;
	stat2.selfSamplingInterval = interval2;
	uint16_t id1 = 7;
	uint16_t id2 = 5;

	int numOfSamples = 3;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-1: [ 1, 3, 5 ]
		stat1.updateStatistics(i * 2 + 1);
	}
	numOfSamples = 6;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-2: [ 3, 5, 7, 9, 11, 13 ]
		stat2.updateStatistics(i * 2 + 3);
	}
	systemStatistics.statisticsMap.insert({id1, stat1});
	systemStatistics.statisticsMap.insert({id2, stat2});
}

void resetSystem() {
	systemStatistics.statisticsMap.clear();
}

bool statisticsAreInitialized(Statistic& stat) {
	return (stat.sampleCounter == 0 and stat.mean == 0 and stat.sumOfSquares == 0 and
	        stat.maxTime == 0 and stat.minTime == 0 and stat.standardDeviation == 0 and
	        stat.max == -std::numeric_limits<double>::infinity() and
	        stat.min == std::numeric_limits<double>::infinity());
}

TEST_CASE("Parameter Statistics Reporting Sub-service") {
	SECTION("Reporting of valid statistics") {
		initializeStatistics(6, 7);
		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
		Services.parameterStatistics.hasAutomaticStatisticsReset = false;

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == ParameterStatisticsService::ServiceType);
		CHECK(report.messageType == ParameterStatisticsService::MessageType::ParameterStatisticsReport);
		CHECK(report.readUint16() == 1); // start time
		CHECK(report.readUint16() == 1); // end time
		CHECK(report.readUint16() == 2); // number of parameters reported
		// Parameter B
		CHECK(report.readUint16() == 5); // ID-2
		CHECK(report.readUint16() == 6); // number of samples
		CHECK(report.readFloat() == 13); // max value
		CHECK(report.readUint32() == 0); // max time
		CHECK(report.readFloat() == 3); // min value
		CHECK(report.readUint32() == 0); // min time
		CHECK(report.readFloat() == 8); // mean
		CHECK(report.readFloat() == Approx(3.41565).epsilon(0.01));
		// Parameter A
		CHECK(report.readUint16() == 7); // ID-1
		CHECK(report.readUint16() == 3); // number of samples
		CHECK(report.readFloat() == 5); // max value
		CHECK(report.readUint32() == 0); // max time
		CHECK(report.readFloat() == 1); // min value
		CHECK(report.readUint32() == 0); // min time
		CHECK(report.readFloat() == 3); // mean
		CHECK(static_cast<int>(report.readFloat()) == 1); // stddev

		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		Services.parameterStatistics.hasAutomaticStatisticsReset = true;
		MessageParser::execute(request);

		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		Message request2 = Message(ParameterStatisticsService::ServiceType,
		                           ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
		request2.appendBoolean(true);
		systemStatistics.statisticsMap[5].mean = 5;
		systemStatistics.statisticsMap[7].mean = 3;
		Services.parameterStatistics.hasAutomaticStatisticsReset = false;

		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		MessageParser::execute(request2);

		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Resetting the parameter statistics") {
		initializeStatistics(6, 7);
		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::ResetParameterStatistics, Message::TC, 1);

		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(not statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		MessageParser::execute(request);

		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[5]));
		CHECK(statisticsAreInitialized(systemStatistics.statisticsMap[7]));

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enable the periodic reporting of statistics") {
		initializeStatistics(6, 7);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting, Message::TC, 1);
		request.appendUint16(3);
		Services.parameterStatistics.periodicStatisticsReportingStatus = false;
		CHECK(Services.parameterStatistics.reportingInterval == 5);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(Services.parameterStatistics.periodicStatisticsReportingStatus == true);
		CHECK(Services.parameterStatistics.reportingInterval == 3);

		Message request2 =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting, Message::TC, 1);
		request2.appendUint16(7);
		Services.parameterStatistics.periodicStatisticsReportingStatus = false;
		CHECK(Services.parameterStatistics.reportingInterval == 3);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidSamplingRateError) == 1);
		CHECK(Services.parameterStatistics.periodicStatisticsReportingStatus == false);
		CHECK(Services.parameterStatistics.reportingInterval == 3);

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Disabling the periodic reporting of statistics") {
		initializeStatistics(6, 7);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DisablePeriodicParameterReporting, Message::TC, 1);
		Services.parameterStatistics.periodicStatisticsReportingStatus = true;

		MessageParser::execute(request);
		REQUIRE(Services.parameterStatistics.periodicStatisticsReportingStatus == false);

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Add/Update statistics definitions") {
		initializeStatistics(7, 6);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions, Message::TC, 1);
		uint16_t numOfIds = 6;
		request.appendUint16(numOfIds);

		uint16_t paramId1 = 0;
		uint16_t paramId2 = 1;
		uint16_t paramId3 = 2;
		uint16_t paramId4 = 7;
		uint16_t paramId5 = 11;
		uint16_t paramId6 = 3;

		uint16_t interval1 = 14;
		uint16_t interval2 = 32;
		uint16_t interval3 = 2;
		uint16_t interval4 = 7;
		uint16_t interval5 = 8;
		uint16_t interval6 = 9;

		request.appendUint16(paramId1);
		request.appendUint16(interval1);
		request.appendUint16(paramId2);
		request.appendUint16(interval2);
		request.appendUint16(paramId3);
		request.appendUint16(interval3);
		request.appendUint16(paramId4);
		request.appendUint16(interval4);
		request.appendUint16(paramId5);
		request.appendUint16(interval5);
		request.appendUint16(paramId6);
		request.appendUint16(interval6);

		Statistic newStatistic;
		newStatistic.setSelfSamplingInterval(0);
		systemStatistics.statisticsMap.insert({0, newStatistic});

		CHECK(systemStatistics.statisticsMap.size() == 3);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 4);
		CHECK(systemStatistics.statisticsMap.size() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidSamplingRateError) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingParameter) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxStatisticDefinitionsReached) == 1);
		CHECK(systemStatistics.statisticsMap[0].selfSamplingInterval == 14);
		CHECK(systemStatistics.statisticsMap[1].selfSamplingInterval == 32);

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete statistics definitions") {
		Statistic stat1;
		Statistic stat2;
		Statistic stat3;
		systemStatistics.statisticsMap.insert({0, stat1});
		systemStatistics.statisticsMap.insert({1, stat2});
		systemStatistics.statisticsMap.insert({2, stat3});

		REQUIRE(systemStatistics.statisticsMap.size() == 3);
		REQUIRE(systemStatistics.statisticsMap.find(0) != systemStatistics.statisticsMap.end());
		REQUIRE(systemStatistics.statisticsMap.find(1) != systemStatistics.statisticsMap.end());
		REQUIRE(systemStatistics.statisticsMap.find(2) != systemStatistics.statisticsMap.end());

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions, Message::TC, 1);
		uint16_t numIds = 2;
		uint16_t id1 = 0;
		uint16_t id2 = 255; // Invalid ID
		request.appendUint16(numIds);
		request.appendUint16(id1);
		request.appendUint16(id2);

		Services.parameterStatistics.periodicStatisticsReportingStatus = true;
		MessageParser::execute(request);

		CHECK(Services.parameterStatistics.periodicStatisticsReportingStatus == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
		CHECK(systemStatistics.statisticsMap.size() == 2);

		Message request2 =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions, Message::TC, 1);
		numIds = 0;
		request2.appendUint16(numIds);

		MessageParser::execute(request2);

		CHECK(Services.parameterStatistics.periodicStatisticsReportingStatus == false);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
		CHECK(systemStatistics.statisticsMap.empty());

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameter statistics definition report") {
		initializeStatistics(0, 12);
		REQUIRE(systemStatistics.statisticsMap.size() == 2);
		REQUIRE(systemStatistics.statisticsMap.find(7) != systemStatistics.statisticsMap.end());
		REQUIRE(systemStatistics.statisticsMap.find(5) != systemStatistics.statisticsMap.end());

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::ReportParameterStatisticsDefinitions, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		CHECK(report.readUint16() == 0); // Reporting interval
		CHECK(report.readUint16() == 2); // Num of valid Ids
		CHECK(report.readUint16() == 5); // Valid parameter ID
		CHECK(report.readUint16() == 12); // Sampling interval
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 0);

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}
}