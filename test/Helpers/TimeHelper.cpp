#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Time format implementation for CDS", "[CDS]") {
	SECTION("Invalid date") {
		TimeAndDate TimeInfo;

		// invalid year
		TimeInfo.year = 2018;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		TimeHelper::utcToSeconds(TimeInfo);

		// invalid month
		TimeInfo.year = 2019;
		TimeInfo.month = 60;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		TimeHelper::utcToSeconds(TimeInfo);

		// invalid day
		TimeInfo.year = 2019;
		TimeInfo.month = 4;
		TimeInfo.day = 35;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		TimeHelper::utcToSeconds(TimeInfo);

		// invalid hour
		TimeInfo.year = 2019;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 100;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		TimeHelper::utcToSeconds(TimeInfo);

		// invalid minute
		TimeInfo.year = 2019;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 200;
		TimeInfo.second = 0;

		TimeHelper::utcToSeconds(TimeInfo);

		// invalid second
		TimeInfo.year = 2019;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 122;

		TimeHelper::utcToSeconds(TimeInfo);

		CHECK(ServiceTests::countErrors() == 6);
		CHECK(ServiceTests::thrownError(ErrorHandler::InvalidDate));
	}

	SECTION("Convert UTC date to elapsed seconds since Unix epoch") {
		TimeAndDate TimeInfo;
		// 10/04/2020 10:15:00
		TimeInfo.year = 2020;
		TimeInfo.month = 4;
		TimeInfo.day = 10;
		TimeInfo.hour = 10;
		TimeInfo.minute = 15;
		TimeInfo.second = 0;

		uint32_t currTime = TimeHelper::utcToSeconds(TimeInfo);

		uint16_t elapsedDays = currTime / 86400;
		uint32_t msOfDay = currTime % 86400 * 1000;
		uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
		CHECK(TimeHelper::generateCDSTimeFormat(TimeInfo) == timeFormat);

		// 1/1/2019 00:00:00
		TimeInfo.year = 2019;
		TimeInfo.month = 1;
		TimeInfo.day = 1;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = TimeHelper::utcToSeconds(TimeInfo);

		elapsedDays = currTime / 86400;
		msOfDay = currTime % 86400 * 1000;
		timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
		CHECK(TimeHelper::generateCDSTimeFormat(TimeInfo) == timeFormat);

		// 5/12/2020 00:00:00
		TimeInfo.year = 2020;
		TimeInfo.month = 12;
		TimeInfo.day = 5;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = TimeHelper::utcToSeconds(TimeInfo);
		CHECK(currTime == 1607126400);

		// 10/12/2020 00:00:00
		TimeInfo.year = 2020;
		TimeInfo.month = 12;
		TimeInfo.day = 10;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = TimeHelper::utcToSeconds(TimeInfo);
		CHECK(currTime == 1607558400);

		// 15/12/2020 00:00:00
		TimeInfo.year = 2020;
		TimeInfo.month = 12;
		TimeInfo.day = 15;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = TimeHelper::utcToSeconds(TimeInfo);
		CHECK(currTime == 1607990400);

		// 20/12/2020 00:00:00
		TimeInfo.year = 2020;
		TimeInfo.month = 12;
		TimeInfo.day = 20;
		TimeInfo.hour = 0;
		TimeInfo.minute = 0;
		TimeInfo.second = 0;

		currTime = TimeHelper::utcToSeconds(TimeInfo);
		CHECK(currTime == 1608422400);
	}

	SECTION("Convert elapsed seconds since Unix epoch to UTC date") {
		uint32_t seconds = 1586513700; // elapsed seconds between 10/04/2020 10:15:00 and Unix epoch

		TimeAndDate TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 4);
		CHECK(TimeInfo.day == 10);
		CHECK(TimeInfo.hour == 10);
		CHECK(TimeInfo.minute == 15);
		CHECK(TimeInfo.second == 0);

		seconds = 1546300800; // elapsed seconds between 1/1/2019 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 1);
		CHECK(TimeInfo.day == 1);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1550966400; // elapsed seconds between 24/2/2019 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 2);
		CHECK(TimeInfo.day == 24);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1551571200; // elapsed seconds between 3/3/2019 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2019);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 3);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1742907370; // elapsed seconds between 25/3/2025 12:56:10 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2025);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 25);
		CHECK(TimeInfo.hour == 12);
		CHECK(TimeInfo.minute == 56);
		CHECK(TimeInfo.second == 10);

		seconds = 1583020800; // elapsed seconds between 1/3/2020 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 3);
		CHECK(TimeInfo.day == 1);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1582934400; // elapsed seconds between 2/29/2020 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 2);
		CHECK(TimeInfo.day == 29);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);

		seconds = 1577923200; // elapsed seconds between 2/1/2020 00:00:00 and Unix epoch

		TimeInfo = TimeHelper::secondsToUTC(seconds);
		CHECK(TimeInfo.year == 2020);
		CHECK(TimeInfo.month == 1);
		CHECK(TimeInfo.day == 2);
		CHECK(TimeInfo.hour == 0);
		CHECK(TimeInfo.minute == 0);
		CHECK(TimeInfo.second == 0);
	}
}

TEST_CASE("Time format implementation for CUC", "[CUC]") {
	TimeAndDate TimeInfo;
	// 10/04/2020 10:15:00
	TimeInfo.year = 2020;
	TimeInfo.month = 4;
	TimeInfo.day = 10;
	TimeInfo.hour = 10;
	TimeInfo.minute = 15;
	TimeInfo.second = 0;

	uint32_t currTime = TimeHelper::utcToSeconds(TimeInfo);

	uint32_t timeFormat = currTime + LEAP_SECONDS;
	CHECK(TimeHelper::generateCUCTimeFormat(TimeInfo) == timeFormat);

	Message message = Message(9, 128, Message::TC, 3);
	message.appendWord(timeFormat);
	CHECK((TimeHelper::parseCUCTimeFormat(message.data) == TimeInfo) == true);

	// 1/1/2019 00:00:00
	TimeInfo.year = 2019;
	TimeInfo.month = 1;
	TimeInfo.day = 1;
	TimeInfo.hour = 0;
	TimeInfo.minute = 0;
	TimeInfo.second = 0;

	currTime = TimeHelper::utcToSeconds(TimeInfo);

	timeFormat = currTime + LEAP_SECONDS; // TAI format
	CHECK(TimeHelper::generateCUCTimeFormat(TimeInfo) == timeFormat);

	message = Message(9, 128, Message::TC, 3);
	message.appendWord(timeFormat);
	CHECK((TimeHelper::parseCUCTimeFormat(message.data) == TimeInfo) == true);
}
