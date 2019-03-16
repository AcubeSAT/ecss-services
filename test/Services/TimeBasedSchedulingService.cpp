#include <catch2/catch.hpp>
#include "ServiceTests.hpp"
#include <Services/TimeBasedSchedulingService.hpp>

#include <iostream>

namespace unit_test {
	struct Tester {
		TimeBasedSchedulingService gen;

		static bool executionFunctionStatus(TimeBasedSchedulingService tmService) {
			return tmService.executionFunctionStatus;
		}

		static uint8_t currentNumberOfActivities(TimeBasedSchedulingService tmService) {
			return tmService.currentNumberOfActivities;
		}

		static auto scheduledActivities(TimeBasedSchedulingService tmService) {
			return tmService.scheduledActivities;
		}
	};
}


TEST_CASE("TC[11,1]", "[service][st11]") {
	Message receivedMessage;
	receivedMessage.serviceType = 11;
	receivedMessage.messageType = 1;

	TimeBasedSchedulingService timeSchedulingService;
	timeSchedulingService.enableScheduleExecution(receivedMessage);
	CHECK(unit_test::Tester::executionFunctionStatus(timeSchedulingService));
}

TEST_CASE("TC[11,2]", "[service][st11]") {
	Message receivedMessage;
	receivedMessage.serviceType = 11;
	receivedMessage.messageType = 2;

	TimeBasedSchedulingService timeSchedulingService;
	timeSchedulingService.disableScheduleExecution(receivedMessage);
	CHECK(not unit_test::Tester::executionFunctionStatus(timeSchedulingService));
}

TEST_CASE("ST[11] Integrated test", "[service][st11]") {
#define NUMBER_OF_INSTRUCTIONS 4 // The number of total instructions in the packet

	Message testMessage1(6, 5, Message::TC, 4), testMessage2(4, 5, Message::TC, 6),
			testMessage3(3, 2, Message::TC, 7), testMessage4(12, 23, Message::TC, 5);
	MessageParser msgParser;
	TimeBasedSchedulingService timeBasedSchedulingService;

	SECTION("TC[11,4] Activity insertion") {
		Message receivedMessage(11, 4, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		// Test activity 1
		testMessage1.appendUint16(4253); // Append dummy data
		receivedMessage.appendUint32(1556435); // todo: Append actual time
		receivedMessage.appendString(msgParser.convertTCToStr(testMessage1));

		// Test activity 2
		testMessage2.appendUint16(45667); // Append dummy data
		receivedMessage.appendUint32(1957232); // todo: Append actual time
		receivedMessage.appendString(msgParser.convertTCToStr(testMessage2));

		// Test activity 3
		testMessage3.appendUint16(456); // Append dummy data
		receivedMessage.appendUint32(1726435); // todo: Append actual time
		receivedMessage.appendString(msgParser.convertTCToStr(testMessage3));

		// Test activity 4
		testMessage4.appendUint16(934); // Append dummy data
		receivedMessage.appendUint32(17248435); // todo: Append actual time
		receivedMessage.appendString(msgParser.convertTCToStr(testMessage4));

		// Insert activities in the schedule. They have to be inserted sorted
		timeBasedSchedulingService.insertActivities(receivedMessage);

		auto scheduledActivities = unit_test::Tester::scheduledActivities(
			timeBasedSchedulingService);
		CHECK(scheduledActivities.size() == 4);
		REQUIRE(scheduledActivities.at(0).requestReleaseTime == 1556435);
		REQUIRE(scheduledActivities.at(1).requestReleaseTime == 1726435);
		REQUIRE(scheduledActivities.at(2).requestReleaseTime == 1957232);
		REQUIRE(scheduledActivities.at(3).requestReleaseTime == 17248435);
		REQUIRE(scheduledActivities.at(0).request == testMessage1);
		REQUIRE(scheduledActivities.at(1).request == testMessage3);
		REQUIRE(scheduledActivities.at(2).request == testMessage2);
		REQUIRE(scheduledActivities.at(3).request == testMessage4);
	}

	SECTION("TC[11,5] Activity deletion by ID") {
		Message receivedMessage(11, 5, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}

	SECTION("TC[11,7] Time shift activities by ID") {
		Message receivedMessage(11, 7, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}

	SECTION("TC[11,9] Detail report scheduled activities by ID") {
		Message receivedMessage(11, 9, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}

	SECTION("TC[11,12] Summary report scheduled activities by ID") {
		Message receivedMessage(11, 12, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}

	SECTION("TC[11,15] Time shift all scheduled activities") {
		Message receivedMessage(11, 15, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}

	SECTION("TC[11,16] Detail report all scheduled activities") {
		Message receivedMessage(11, 16, Message::TC, 1);
		receivedMessage.appendUint16(NUMBER_OF_INSTRUCTIONS);

		//timeBasedSchedulingService.detailReporActivitiesByID();
	}
}
