#include <iostream>
#include "ECSS_Definitions.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "catch2/catch_all.hpp"

/**
 * System-statistics initialization, so there are actual statistics in the map to work with.
 */
void initializeStatistics(uint16_t interval1, uint16_t interval2) {
	Statistic stat1;
	Statistic stat2;
	stat1.selfSamplingInterval = interval1;
	stat2.selfSamplingInterval = interval2;
	ParameterId id1 = 7;
	ParameterId id2 = 5;

	int numOfSamples = 3;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-1: [ 1, 3, 5 ]
		stat1.updateStatistics(i * 2 + 1);
	}
	numOfSamples = 6;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-2: [ 3, 5, 7, 9, 11, 13 ]
		stat2.updateStatistics(i * 2 + 3);
	}
	Services.parameterStatistics.statisticsMap.insert({id1, stat1});
	Services.parameterStatistics.statisticsMap.insert({id2, stat2});
}

TEST_CASE("Reporting of statistics") {

	if (not Services.parameterStatistics.HasAutomaticStatisticsReset) {
		SECTION("Report statistics, with auto statistic reset disabled with TC") {
			initializeStatistics(6, 7);
			Message request = Message(ParameterStatisticsService::ServiceType,
			                          ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);

			MessageParser::execute(request);
			CHECK(ServiceTests::count() == 1);

			Message report = ServiceTests::get(0);
			CHECK(report.serviceType == ParameterStatisticsService::ServiceType);
			CHECK(report.messageType == ParameterStatisticsService::MessageType::ParameterStatisticsReport);
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // start time
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // end time
			CHECK(report.readUint16() == 2);                                    // number of parameters reported
			// Parameter B
			CHECK(report.read<ParameterId>() == 5);                             // ID-2
			CHECK(report.read<ParameterSampleCount>() == 6);                    // number of samples
			CHECK(report.readFloat() == 13);                                    // max value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // max time
			CHECK(report.readFloat() == 3);                                     // min value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // min time
			CHECK(report.readFloat() == 8);                                     // mean
			CHECK(report.readFloat() == Catch::Approx(3.41565).epsilon(0.01));
			// Parameter A
			CHECK(report.read<ParameterId>() == 7);                             // ID-1
			CHECK(report.read<ParameterSampleCount>() == 3);                    // number of samples
			CHECK(report.readFloat() == 5);                                     // max value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // max time
			CHECK(report.readFloat() == 1);                                     // min value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // min time
			CHECK(report.readFloat() == 3);                                     // mean
			CHECK(static_cast<int>(report.readFloat()) == 1);                   // stddev

			CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}

		SECTION("Report statistics, with auto statistics reset disabled, but reset is given by TC") {
			Message request = Message(ParameterStatisticsService::ServiceType,
			                          ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
			request.appendBoolean(true);
			Services.parameterStatistics.statisticsMap[5].mean = 5;
			Services.parameterStatistics.statisticsMap[7].mean = 3;

			CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

			MessageParser::execute(request);

			CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}

		ServiceTests::reset();

		SECTION("Report statistics, with auto statistic reset disabled without TC") {
			initializeStatistics(6, 7);
			Services.parameterStatistics.reportParameterStatistics(false);

			CHECK(ServiceTests::count() == 1);

			Message report = ServiceTests::get(0);
			CHECK(report.serviceType == ParameterStatisticsService::ServiceType);
			CHECK(report.messageType == ParameterStatisticsService::MessageType::ParameterStatisticsReport);
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // start time
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // end time
			CHECK(report.readUint16() == 2);                                    // number of parameters reported
			// Parameter B
			CHECK(report.read<ParameterId>() == 5);                             // ID-2
			CHECK(report.read<ParameterSampleCount>() == 6);                    // number of samples
			CHECK(report.readFloat() == 13);                                    // max value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // max time
			CHECK(report.readFloat() == 3);                                     // min value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // min time
			CHECK(report.readFloat() == 8);                                     // mean
			CHECK(report.readFloat() == Catch::Approx(3.41565).epsilon(0.01));
			// Parameter A
			CHECK(report.read<ParameterId>() == 7);                             // ID-1
			CHECK(report.read<ParameterSampleCount>() == 3);                    // number of samples
			CHECK(report.readFloat() == 5);                                     // max value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // max time
			CHECK(report.readFloat() == 1);                                     // min value
			CHECK(report.read<Time::DefaultCUC>().formatAsBytes() == 86769000); // min time
			CHECK(report.readFloat() == 3);                                     // mean
			CHECK(static_cast<int>(report.readFloat()) == 1);                   // stddev

			CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}

		SECTION("Report statistics, with auto statistics reset disabled, but reset is given with args, without TC") {
			Services.parameterStatistics.statisticsMap[5].mean = 5;
			Services.parameterStatistics.statisticsMap[7].mean = 3;

			CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

			Services.parameterStatistics.reportParameterStatistics(true);

			CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}
	}
	else {
		SECTION("Report statistics, with auto statistics reset enabled with TC") {
			Message request = Message(ParameterStatisticsService::ServiceType,
			                          ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
			MessageParser::execute(request);

			CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}

		ServiceTests::reset();

		SECTION("Report statistics, with auto statistics reset enabled without TC") {
			Services.parameterStatistics.reportParameterStatistics(false);

			CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
			CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());
		}
	}
}

TEST_CASE("Resetting the parameter statistics") {
	SECTION("Reset via TC") {
		initializeStatistics(6, 7);
		Message request = Message(ParameterStatisticsService::ServiceType,
		                          ParameterStatisticsService::MessageType::ResetParameterStatistics, Message::TC, 1);

		CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
		CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

		MessageParser::execute(request);

		CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
		CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

		ServiceTests::reset();
	}

	SECTION("Reset without TC") {
		initializeStatistics(6, 7);

		CHECK(not Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
		CHECK(not Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

		Services.parameterStatistics.resetParameterStatistics();

		CHECK(Services.parameterStatistics.statisticsMap[5].statisticsAreInitialized());
		CHECK(Services.parameterStatistics.statisticsMap[7].statisticsAreInitialized());

		ServiceTests::reset();
	}
}

TEST_CASE("Enable the periodic reporting of statistics") {
	SECTION("Valid reporting interval requested") {
		initializeStatistics(6, 7);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting, Message::TC, 1);
		request.append<SamplingInterval>(6);
		Services.parameterStatistics.setPeriodicReportingStatus(false);
		CHECK(Services.parameterStatistics.getReportingIntervalMs() == 700);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(Services.parameterStatistics.getPeriodicReportingStatus() == true);
		CHECK(Services.parameterStatistics.getReportingIntervalMs() == 6);
	}

	SECTION("Invalid reporting interval requested") {
		Message request2 =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting, Message::TC, 1);
		request2.append<SamplingInterval>(3);
		Services.parameterStatistics.setPeriodicReportingStatus(false);
		CHECK(Services.parameterStatistics.getReportingIntervalMs() == 6);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidSamplingRateError) == 1);
		CHECK(Services.parameterStatistics.getPeriodicReportingStatus() == false);
		CHECK(Services.parameterStatistics.getReportingIntervalMs() == 6);

		ServiceTests::reset();
	}
}

TEST_CASE("Disabling the periodic reporting of statistics") {
	SECTION("Successfully disable the periodic reporting") {
		initializeStatistics(6, 7);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DisablePeriodicParameterReporting, Message::TC, 1);
		Services.parameterStatistics.setPeriodicReportingStatus(true);

		MessageParser::execute(request);
		REQUIRE(Services.parameterStatistics.getPeriodicReportingStatus() == false);

		ServiceTests::reset();
	}
}

TEST_CASE("Add/Update statistics definitions") {
	SECTION("Update existing parameter statistic definition") {
		initializeStatistics(750, 2400);
		Statistic newStatistic;
		newStatistic.setSelfSamplingInterval(0);
		Services.parameterStatistics.statisticsMap.insert({0, newStatistic});

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions, Message::TC, 1);
		ParameterId numOfIds = 1;
		request.appendUint16(numOfIds);

		ParameterId paramId1 = 0;
		SamplingInterval interval1 = 1400;
		request.append<ParameterId>(paramId1);
		request.append<SamplingInterval>(interval1);

		CHECK(Services.parameterStatistics.statisticsMap.size() == 3);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 0);
		CHECK(Services.parameterStatistics.statisticsMap.size() == 3);
		CHECK(Services.parameterStatistics.statisticsMap[0].selfSamplingInterval == 1400);

		ServiceTests::reset();
	}

	SECTION("Add new statistic definition") {
		initializeStatistics(7, 6);
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions, Message::TC, 1);
		ParameterId numOfIds = 1;
		request.appendUint16(numOfIds);

		ParameterId paramId1 = 1;
		SamplingInterval interval1 = 3200;
		request.append<ParameterId>(paramId1);
		request.append<SamplingInterval>(interval1);

		CHECK(Services.parameterStatistics.statisticsMap.size() == 2);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 0);
		CHECK(Services.parameterStatistics.statisticsMap.size() == 3);
		CHECK(Services.parameterStatistics.statisticsMap[1].selfSamplingInterval == 3200);

		ServiceTests::reset();
	}

	SECTION("All possible invalid requests combined with add/update") {
		initializeStatistics(7000, 6000);
		Statistic newStatistic;
		newStatistic.setSelfSamplingInterval(0);
		Services.parameterStatistics.statisticsMap.insert({0, newStatistic});

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions, Message::TC, 1);
		ParameterId numOfIds = 6;
		request.appendUint16(numOfIds);

		ParameterId paramId1 = 0;
		ParameterId paramId2 = 1;
		ParameterId paramId3 = 2;
		ParameterId paramId4 = ECSSParameterCount + 24;
		ParameterId paramId5 = ECSSParameterCount + 1;
		ParameterId paramId6 = 3;

		SamplingInterval interval1 = 14000;
		SamplingInterval interval2 = 32000;
		SamplingInterval interval3 = 20;
		SamplingInterval interval4 = 7000;
		SamplingInterval interval5 = 8000;
		SamplingInterval interval6 = 9000;

		request.append<ParameterId>(paramId1);
		request.append<SamplingInterval>(interval1);
		request.append<ParameterId>(paramId2);
		request.append<SamplingInterval>(interval2);
		request.append<ParameterId>(paramId3);
		request.append<SamplingInterval>(interval3);
		request.append<ParameterId>(paramId4);
		request.append<SamplingInterval>(interval4);
		request.append<ParameterId>(paramId5);
		request.append<SamplingInterval>(interval5);
		request.append<ParameterId>(paramId6);
		request.append<SamplingInterval>(interval6);

		CHECK(Services.parameterStatistics.statisticsMap.size() == 3);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 4);
		CHECK(Services.parameterStatistics.statisticsMap.size() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidSamplingRateError) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingParameter) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxStatisticDefinitionsReached) == 1);
		CHECK(Services.parameterStatistics.statisticsMap[0].selfSamplingInterval == 14000);
		CHECK(Services.parameterStatistics.statisticsMap[1].selfSamplingInterval == 32000);

		ServiceTests::reset();
	}
}

TEST_CASE("Delete statistics definitions") {
	SECTION("Delete specified definitions") {
		Statistic stat1;
		Statistic stat2;
		Statistic stat3;
		Services.parameterStatistics.statisticsMap.insert({0, stat1});
		Services.parameterStatistics.statisticsMap.insert({1, stat2});
		Services.parameterStatistics.statisticsMap.insert({2, stat3});

		REQUIRE(Services.parameterStatistics.statisticsMap.size() == 3);
		REQUIRE(Services.parameterStatistics.statisticsMap.find(0) != Services.parameterStatistics.statisticsMap.end());
		REQUIRE(Services.parameterStatistics.statisticsMap.find(1) != Services.parameterStatistics.statisticsMap.end());
		REQUIRE(Services.parameterStatistics.statisticsMap.find(2) != Services.parameterStatistics.statisticsMap.end());

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions, Message::TC, 1);
		ParameterId numIds = 2;
		ParameterId id1 = 0;
		ParameterId id2 = 255; // Invalid ID
		request.appendUint16(numIds);
		request.append<ParameterId>(id1);
		request.append<ParameterId>(id2);

		Services.parameterStatistics.setPeriodicReportingStatus(true);
		MessageParser::execute(request);

		CHECK(Services.parameterStatistics.getPeriodicReportingStatus() == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
		CHECK(Services.parameterStatistics.statisticsMap.size() == 2);
	}

	SECTION("Delete all definitions") {
		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions, Message::TC, 1);
		ParameterId numIds = 0;
		request.appendUint16(numIds);

		MessageParser::execute(request);

		CHECK(Services.parameterStatistics.getPeriodicReportingStatus() == false);
		CHECK(Services.parameterStatistics.statisticsMap.empty());

		ServiceTests::reset();
	}
}

TEST_CASE("Parameter statistics definition report") {
	SECTION("Check if the stored report is valid") {
		initializeStatistics(0, 12);
		REQUIRE(Services.parameterStatistics.statisticsMap.size() == 2);
		REQUIRE(Services.parameterStatistics.statisticsMap.find(7) != Services.parameterStatistics.statisticsMap.end());
		REQUIRE(Services.parameterStatistics.statisticsMap.find(5) != Services.parameterStatistics.statisticsMap.end());

		Message request =
		    Message(ParameterStatisticsService::ServiceType,
		            ParameterStatisticsService::MessageType::ReportParameterStatisticsDefinitions, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		CHECK(report.read<SamplingInterval>() == 700); // Reporting interval
		CHECK(report.readUint16() == 2);               // Num of valid Ids
		CHECK(report.read<ParameterId>() == 5);        // Valid parameter ID
		CHECK(report.read<SamplingInterval>() == 12);  // Sampling interval
		CHECK(report.read<ParameterId>() == 7);
		CHECK(report.read<SamplingInterval>() == 0);

		ServiceTests::reset();
	}
}
