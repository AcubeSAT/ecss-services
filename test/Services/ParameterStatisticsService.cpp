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
	for (int i = 0; i < numOfSamples; i++) {    // Values of stat-1: [ 1, 3, 5 ]
		stat1.updateStatistics(i * 2 + 1);
	}
	numOfSamples = 6;
	for (int i = 0; i < numOfSamples; i++) {    // Values of stat-2: [ 3, 5, 7, 9, 11, 13 ]
		stat2.updateStatistics(i * 2 + 3);
	}
	systemStatistics.statisticsMap.insert({id1, stat1});
	systemStatistics.statisticsMap.insert({id2, stat2});
}

void resetSystem() {
	systemStatistics.statisticsMap.clear();
}

TEST_CASE("Parameter Statistics Reporting Sub-service") {
	SECTION("Reporting of valid statistics") {
		initializeStatistics(6, 7);
		Message request = Message(ParameterStatisticsService::ServiceType,
		                            ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == ParameterStatisticsService::ServiceType);
		CHECK(report.messageType == ParameterStatisticsService::MessageType::ParameterStatisticsReport);
		CHECK(report.readUint16() == 1);    // start time
		CHECK(report.readUint16() == 1);    // end time
		CHECK(report.readUint16() == 2);    // number of parameters reported
		// Parameter B
		CHECK(report.readUint16() == 5);    // ID-2
		CHECK(report.readUint16() == 6);    // number of samples
		CHECK(report.readFloat() == 13);    // max value
		CHECK(report.readUint32() == 0);    // max time
		CHECK(report.readFloat() == 3);    // min value
		CHECK(report.readUint32() == 0);    // min time
		CHECK(report.readFloat() == 8);     // mean
		CHECK(static_cast <int> (report.readFloat()) == 3);  // stddev
		// Parameter A
		CHECK(report.readUint16() == 7);    // ID-1
		CHECK(report.readUint16() == 3);    // number of samples
		CHECK(report.readFloat() == 5);    // max value
		CHECK(report.readUint32() == 0);    // max time
		CHECK(report.readFloat() == 1);    // min value
		CHECK(report.readUint32() == 0);    // min time
		CHECK(report.readFloat() == 3);     // mean
		CHECK(static_cast <int> (report.readFloat()) == 1);  // stddev

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

//	SECTION("Periodic reporting of valid statistics") {
//		initializeStatistics(6, 7);
//		Message request = Message(ParameterStatisticsService::ServiceType,
//		                          ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting, Message::TC, 1);
//		request.appendUint16(3);
//		MessageParser::execute(request);
//		CHECK(ServiceTests::count() == 10);     // 10 reports generated (check function implementation)
//
//		resetSystem();
//		ServiceTests::reset();
//		Services.reset();
//	}

	/**
	 * @todo: In order to test every case for this function, we need to specify what is the MAX_PARAMETERS
	 */
	SECTION("Add/Update statistics definitions") {
		initializeStatistics(7, 6);
		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions,
		                          Message::TC, 1);
		uint16_t numOfIds = 3;
		uint16_t paramId1 = 0;
		uint16_t paramId2 = 1;
		uint16_t paramId3 = 2;
		uint16_t interval1 = 14;
		uint16_t interval2 = 32;
		uint16_t interval3 = 2;
		request.appendUint16(numOfIds);
		request.appendUint16(paramId1);
		request.appendUint16(interval1);
		request.appendUint16(paramId2);
		request.appendUint16(interval2);
		request.appendUint16(interval3);

		CHECK(systemStatistics.statisticsMap[0].selfSamplingInterval == 0);
		CHECK(systemStatistics.statisticsMap[1].selfSamplingInterval == 0);
		CHECK(systemStatistics.statisticsMap[2].selfSamplingInterval == 0);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidSamplingRateError) == 1);
		CHECK(ServiceTests::count() == 1);
		CHECK(systemStatistics.statisticsMap[0].selfSamplingInterval == 14);
		CHECK(systemStatistics.statisticsMap[1].selfSamplingInterval == 32);

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Delete statistics definitions") {
		Statistic stat1;
		Statistic stat2;
		systemStatistics.statisticsMap.insert({0, stat1});
		systemStatistics.statisticsMap.insert({1, stat2});

		REQUIRE(systemStatistics.statisticsMap.size() == 2);
		REQUIRE(systemStatistics.statisticsMap.find(0) != systemStatistics.statisticsMap.end());
		REQUIRE(systemStatistics.statisticsMap.find(1) != systemStatistics.statisticsMap.end());
		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions,
		                          Message::TC, 1);
		uint16_t numIds = 3;
		uint16_t id1 = 0;
		uint16_t id2 = 1;
		uint16_t id3 = 255;  // Invalid ID
		request.appendUint16(numIds);
		request.appendUint16(id1);
		request.appendUint16(id2);
		request.appendUint16(id3);

		MessageParser::execute(request);
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

		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::ReportParameterStatisticsDefinitions,
		                          Message::TC, 1);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		CHECK(report.readUint16() == 0);    //Reporting interval
		CHECK(report.readUint16() == 1);    //Num of valid Ids
		CHECK(report.readUint16() == 5);    //Valid parameter ID
		CHECK(report.readUint16() == 12);   //Sampling interval

		resetSystem();
		ServiceTests::reset();
		Services.reset();
	}

}