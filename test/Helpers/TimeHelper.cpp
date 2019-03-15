#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"

TEST_CASE("Time format implementation", "[CUC]") {

	SECTION("Convert UTC date to elapsed seconds since Unix epoch") {
		struct TimeAndDate TimeInfo = {0};
		// 10/04/2020 10:15:00
		TimeInfo.year = 2020;
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
		CHECK(TimeHelper::generateCDStimeFormat(TimeInfo) == timeFormat);

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
		CHECK(TimeHelper::generateCDStimeFormat(TimeInfo) == timeFormat);
	}

	SECTION("Convert elapsed seconds since Unix epoch to UTC date") {
		uint32_t seconds = 1586513700; // elapsed seconds between 10/04/2020 10:15:00 and Unix epoch

		TimeHelper time;
		struct TimeAndDate TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 4);
		CHECK(TimeInfo.day == 10);
		CHECK(TimeInfo.hour == 10);
		CHECK(TimeInfo.minute == 15);
		CHECK(TimeInfo.second == 0);

		seconds = 1546300800; // elapsed seconds between 1/1/2019 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 1);
		CHECK(TimeInfo.day == 1);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1550966400; // elapsed seconds between 24/2/2019 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 2);
		CHECK(TimeInfo.day == 24);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1551571200; // elapsed seconds between 3/3/2019 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 3);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1742907370; // elapsed seconds between 25/3/2025 12:56:10 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2025);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 25);
		CHECK(TimeInfo.hour == 12);
		CHECK(TimeInfo.minute == 56);
		CHECK(TimeInfo.second == 10);

		seconds = 1583020800; // elapsed seconds between 1/3/2020 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 1);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1582934400; // elapsed seconds between 2/29/2020 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 2);
		CHECK(TimeInfo.day == 29);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1577923200; // elapsed seconds between 2/1/2020 00:00:00 and Unix epoch

		TimeInfo = time.get_utcTime(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 1);
		CHECK(TimeInfo.day == 2);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

	}

	SECTION("Date comparison") {
		SECTION("Different year") {
			struct TimeAndDate Now = {0};
			// 10/04/2021 10:15:00
			Now.year = 2021;
			Now.month = 4;
			Now.day = 10;
			Now.hour = 10;
			Now.minute = 15;
			Now.second = 0;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}

		SECTION("Different month") {

			struct TimeAndDate Now = {0};
			// 10/05/2020 10:15:00
			Now.year = 2020;
			Now.month = 5;
			Now.day = 10;
			Now.hour = 10;
			Now.minute = 15;
			Now.second = 0;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}

		SECTION("Different day") {
			struct TimeAndDate Now = {0};
			// 11/04/2020 10:15:00
			Now.year = 2020;
			Now.month = 5;
			Now.day = 11;
			Now.hour = 10;
			Now.minute = 15;
			Now.second = 0;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}

		SECTION("Different hour") {
			struct TimeAndDate Now = {0};
			// 10/04/2020 11:15:00
			Now.year = 2020;
			Now.month = 4;
			Now.day = 10;
			Now.hour = 11;
			Now.minute = 15;
			Now.second = 0;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}

		SECTION("Different minute") {
			struct TimeAndDate Now = {0};
			// 11/04/2020 10:16:00
			Now.year = 2020;
			Now.month = 4;
			Now.day = 10;
			Now.hour = 10;
			Now.minute = 16;
			Now.second = 0;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}

		SECTION("Different second") {
			struct TimeAndDate Now = {0};
			// 11/04/2020 10:15:01
			Now.year = 2020;
			Now.month = 4;
			Now.day = 10;
			Now.hour = 10;
			Now.minute = 15;
			Now.second = 1;

			struct TimeAndDate Date = {0};
			// 10/04/2020 10:15:00
			Date.year = 2020;
			Date.month = 4;
			Date.day = 10;
			Date.hour = 10;
			Date.minute = 15;
			Date.second = 0;

			CHECK(TimeHelper::IsAfter(Now, Date, true) == false);
			CHECK(TimeHelper::IsAfter(Date, Now, true) == true);
			CHECK(TimeHelper::IsBefore(Now, Date, true) == true);
			CHECK(TimeHelper::IsBefore(Date, Now, true) == false);
		}
	}

}
