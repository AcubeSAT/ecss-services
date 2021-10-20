#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/ParameterService.hpp"

TEST_CASE("Parameter Statistics Reporting Sub-service") {
	SECTION("Reporting of valid statistics") {

		/**
		 * System-statistics initialization, so there are actual statistics in the map to work with.
		 */
		Statistic stat1;
		Statistic stat2;
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

		ServiceTests::reset();
		Services.reset();
	}
}