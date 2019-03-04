#include <catch2/catch.hpp>
#include <Services/TimeManagementService.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[9,3]", "[service][st09]") {
	TimeManagementService timeService;

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

	uint16_t elapsedDays = currTime/86400;
	uint32_t msOfDay = currTime % 86400 * 1000;
	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);

	timeService.cdsTimeReport(TimeInfo);
	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 9);
	CHECK(response.messageType == 3);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readHalfword() == static_cast<uint16_t>(timeFormat >> 32));
	CHECK(response.readWord() == static_cast<uint32_t >(timeFormat));

	Message message = Message(9, 128, Message::TC, 3);
	message.appendHalfword(static_cast<uint16_t >(timeFormat >> 32));
	message.appendWord(static_cast<uint32_t >(timeFormat));

	TimeInfo = timeService.cdsTimeRequest(message);
	CHECK(TimeInfo.year == 1998);
	CHECK(TimeInfo.month == 4);
	CHECK(TimeInfo.day == 10);
	CHECK(TimeInfo.hour == 10);
	CHECK(TimeInfo.minute == 15);
	CHECK(TimeInfo.second == 0);


	// 1/1/2019 00:00:00
	TimeInfo.year = 2019;
	TimeInfo.month = 1;
	TimeInfo.day = 1;
	TimeInfo.hour = 0;
	TimeInfo.minute = 0;
	TimeInfo.second = 0;

	currTime = time.get_mkUTCtime(TimeInfo);

	elapsedDays = currTime/86400;
	msOfDay = currTime % 86400 * 1000;
	timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);

	timeService.cdsTimeReport(TimeInfo);
	response = ServiceTests::get(1);
	CHECK(response.serviceType == 9);
	CHECK(response.messageType == 3);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readHalfword() == static_cast<uint16_t>(timeFormat >> 32));
	CHECK(response.readWord() == static_cast<uint32_t >(timeFormat));

	message = Message(9, 128, Message::TC, 3);
	message.appendHalfword(static_cast<uint16_t >(timeFormat >> 32));
	message.appendWord(static_cast<uint32_t >(timeFormat));

	TimeInfo = timeService.cdsTimeRequest(message);
	CHECK(TimeInfo.year == 2019);
	CHECK(TimeInfo.month == 1);
	CHECK(TimeInfo.day == 1);
	CHECK(TimeInfo.hour == 0);
	CHECK(TimeInfo.minute == 0);
	CHECK(TimeInfo.second == 0);
}
