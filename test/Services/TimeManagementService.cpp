#include <catch2/catch.hpp>
#include <Services/TimeManagementService.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[9,3]", "[service][st09]") {
	TimeManagementService timeService;

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

	timeService.cdsTimeReport(timeInfo);
	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 9);
	CHECK(response.messageType == 3);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readByte() == static_cast<uint8_t>((timeFormat >> 32)));
	CHECK(response.readWord() == static_cast<uint32_t>(timeFormat));


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

	timeService.cdsTimeReport(timeInfo);
	response = ServiceTests::get(1);
	CHECK(response.serviceType == 9);
	CHECK(response.messageType == 3);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readByte() == static_cast<uint8_t>((timeFormat >> 32)));
	CHECK(response.readWord() == static_cast<uint32_t>(timeFormat));

}
