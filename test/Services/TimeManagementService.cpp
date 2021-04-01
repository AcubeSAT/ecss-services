#include <catch2/catch_all.hpp>
#include <Services/TimeManagementService.hpp>
#include "ServiceTests.hpp"

TimeManagementService& timeService = Services.timeManagement;

TEST_CASE("TM[9,3]", "[service][st09]") {
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

	timeService.cdsTimeReport(TimeInfo);
	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == TimeManagementService::ServiceType);
	CHECK(response.messageType == TimeManagementService::MessageType::CdsTimeReport);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readHalfword() == static_cast<uint16_t>(timeFormat >> 32));
	CHECK(response.readWord() == static_cast<uint32_t>(timeFormat));

	Message message = Message(TimeManagementService::ServiceType, 128, Message::TC, 3);
	message.appendHalfword(static_cast<uint16_t>(timeFormat >> 32));
	message.appendWord(static_cast<uint32_t>(timeFormat));

	MessageParser::execute(message);
	TimeInfo = timeService.cdsTimeRequest(message);
	CHECK(TimeInfo.year == 2020);
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

	currTime = TimeHelper::utcToSeconds(TimeInfo);

	elapsedDays = currTime / 86400;
	msOfDay = currTime % 86400 * 1000;
	timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);

	timeService.cdsTimeReport(TimeInfo);
	response = ServiceTests::get(1);
	CHECK(response.serviceType == TimeManagementService::ServiceType);
	CHECK(response.messageType == TimeManagementService::MessageType::CdsTimeReport);
	CHECK(response.packetType == Message::TM);
	CHECK(response.readHalfword() == static_cast<uint16_t>(timeFormat >> 32));
	CHECK(response.readWord() == static_cast<uint32_t>(timeFormat));

	message = Message(TimeManagementService::ServiceType, 128, Message::TC, 3);
	message.appendHalfword(static_cast<uint16_t>(timeFormat >> 32));
	message.appendWord(static_cast<uint32_t>(timeFormat));

	TimeInfo = timeService.cdsTimeRequest(message);
	CHECK(TimeInfo.year == 2019);
	CHECK(TimeInfo.month == 1);
	CHECK(TimeInfo.day == 1);
	CHECK(TimeInfo.hour == 0);
	CHECK(TimeInfo.minute == 0);
	CHECK(TimeInfo.second == 0);
}
