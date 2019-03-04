#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"

TEST_CASE("Time format implementation", "[CUC]") {
	struct TimeAndDate timeInfo = {0};
	// 10/04/1998 10:15:00
	timeInfo.year = 1998;
	timeInfo.month = 4;
	timeInfo.day = 10;
	timeInfo.hour = 10;
	timeInfo.minute = 15;
	timeInfo.second = 0;

	TimeHelper time;
	uint32_t currTime = time.get_mkgmtime(timeInfo);

	uint16_t elapsedDays = currTime/86400;
	uint32_t msOfDay = currTime % 86400 * 1000;
	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
	CHECK(TimeHelper::implementCDSTimeFormat(timeInfo) == timeFormat);

	// 1/1/2019 00:00:00
	timeInfo.year = 2019;
	timeInfo.month = 1;
	timeInfo.day = 1;
	timeInfo.hour = 0;
	timeInfo.minute = 0;
	timeInfo.second = 0;

	currTime = time.get_mkgmtime(timeInfo);

	elapsedDays = currTime/86400;
	msOfDay = currTime % 86400 * 1000;
	timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);
	CHECK(TimeHelper::implementCDSTimeFormat(timeInfo) == timeFormat);

}
