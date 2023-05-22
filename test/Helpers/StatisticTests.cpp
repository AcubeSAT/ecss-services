#include "Helpers/Statistic.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Statistics updating function") {
	SECTION("values in one by one") {
		Statistic stat1;
		CHECK(stat1.statisticsAreInitialized());

		double value = 3.24;
		stat1.updateStatistics(value);
		CHECK(stat1.max == 3.24);
		CHECK(stat1.min == 3.24);
		CHECK(stat1.mean == 3.24);

		value = 1.3;
		stat1.updateStatistics(value);
		CHECK(stat1.max == 3.24);
		CHECK(stat1.min == 1.3);
		CHECK(stat1.mean == Catch::Approx(2.27).epsilon(0.01));

		value = 5.8;
		stat1.updateStatistics(value);
		CHECK(stat1.max == 5.8);
		CHECK(stat1.min == 1.3);
		CHECK(stat1.mean == Catch::Approx(3.446).epsilon(0.001));
	}

	SECTION("Multiple consecutive values") {
		double values[10] = {8.3001, 2.3, 6.4, 1.1, 8.35, 3.4, 6, 8.31, 4.7, 1.09};
		Statistic stat;
		CHECK(stat.statisticsAreInitialized());
		for (auto& value: values) {
			stat.updateStatistics(value);
		}
		CHECK(stat.max == 8.35);
		CHECK(stat.min == 1.09);
		CHECK(stat.mean == Catch::Approx(4.99501).epsilon(0.00001));
	}
}

TEST_CASE("Appending of statistics to message") {
	SECTION("Successful appending of statistics") {
		Message report(ParameterStatisticsService::ServiceType,
		               ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);
		double values[10] = {8.3001, 2.3, 6.4, 1.1, 8.35, 3.4, 6, 8.31, 4.7, 1.09};
		Statistic stat;
		CHECK(stat.statisticsAreInitialized());
		for (auto& value: values) {
			stat.updateStatistics(value);
		}
		stat.appendStatisticsToMessage(report);

		REQUIRE(report.readFloat() == 8.35f);
		REQUIRE(report.readUint32() == 86769000);   // dummy time value
		REQUIRE(report.readFloat() == 1.09f);
		REQUIRE(report.readUint32() == 86769000);
		REQUIRE(report.readFloat() == Catch::Approx(4.99501).epsilon(0.00001));
		REQUIRE(report.readFloat() == Catch::Approx(2.76527).epsilon(0.00001));
	}
}

TEST_CASE("Change the value of the sampling interval") {
	SECTION("Test the setter function") {
		Statistic stat;
		CHECK(stat.selfSamplingInterval == 0);
		stat.setSelfSamplingInterval(4);
		REQUIRE(stat.selfSamplingInterval == 4);
	}
}

TEST_CASE("Reset statistics") {
	SECTION("Successfully reset all the statistics of a parameter") {
		double values[10] = {8.3001, 2.3, 6.4, 1.1, 8.35, 3.4, 6, 8.31, 4.7, 1.09};
		Statistic stat;
		CHECK(stat.statisticsAreInitialized());
		for (auto& value: values) {
			stat.updateStatistics(value);
		}
		CHECK(not stat.statisticsAreInitialized());
		stat.resetStatistics();
		REQUIRE(stat.statisticsAreInitialized());
	}
}
