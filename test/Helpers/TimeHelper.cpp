#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"

TEST_CASE("Time format implementation", "[CUC]") {

	SECTION("Convert UTC date to elapsed seconds since Unix epoch") {
		struct TimeAndDate TimeInfo = {0};
		// 10/04/1998 10:15:00
		TimeInfo.year = 1998;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		TimeHelper time;
		uint32_t currTime = time.get_mkUTCtime(TimeInfo);

		uint16_t elapsedDays = currTime / 86400;
		uint32_t msOfDay = currTime % 86400 * 1000;
		uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
		CHECK(TimeHelper::implementCDSTimeFormat(TimeInfo) == timeFormat);

		// 1/1/2019 00:00:00
		TimeInfo.year = 2019;
		TimeInfo.month = 1;
		TimeInfo.day = 1;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = time.get_mkUTCtime(TimeInfo);

		elapsedDays = currTime / 86400;
		msOfDay = currTime % 86400 * 1000;
		timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
		CHECK(TimeHelper::implementCDSTimeFormat(TimeInfo) == timeFormat);
	}

	SECTION("Convert elapsed seconds since Unix epoch to UTC date"){
		uint32_t seconds = 892203300; // elapsed seconds between 10/04/1998 10:15:00 and Unix epoch

		TimeHelper time;
		struct TimeAndDate TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 1998);
		CHECK(TimeInfo.month == 4);
		CHECK(TimeInfo.day == 10);
		CHECK(TimeInfo.hour == 10);
		CHECK(TimeInfo.minute == 15);
		CHECK(TimeInfo.second == 0);

		seconds = 1546300800;

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 1);
		CHECK(TimeInfo.day == 1);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);
	}

}

