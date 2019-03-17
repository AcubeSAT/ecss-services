#include <catch2/catch.hpp>
#include "ServiceTests.hpp"
#include <Services/TimeBasedSchedulingService.hpp>

#include <iostream>

namespace unit_test {
	struct Tester {
		static TimeBasedSchedulingService gen;

		static bool executionFunctionStatus(TimeBasedSchedulingService &tmService) {
			return tmService.executionFunctionStatus;
		}

		static uint8_t *currentNumberOfActivities(TimeBasedSchedulingService &tmService) {
			return &tmService.currentNumberOfActivities;
		}

		static auto scheduledActivities(TimeBasedSchedulingService &tmService) {
			return &tmService.scheduledActivities;
		}
	};
}


Message testMessage1, testMessage2, testMessage3, testMessage4;
TimeBasedSchedulingService timeSchedulingService;
MessageParser msgParser;

/*
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
*/

TEST_CASE("ST[11] Generic Test", "[service][st11]") {
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeSchedulingService);

	SECTION("TC[11,4] Activity Insertion") {
		// Initialize the test messages
		testMessage1.serviceType = 6;
		testMessage1.messageType = 5;
		testMessage1.packetType = Message::TC;
		testMessage1.applicationId = 4;

		testMessage2.serviceType = 4;
		testMessage2.messageType = 5;
		testMessage2.packetType = Message::TC;
		testMessage2.applicationId = 6;

		testMessage3.serviceType = 3;
		testMessage3.messageType = 2;
		testMessage3.packetType = Message::TC;
		testMessage3.applicationId = 7;

		testMessage4.serviceType = 12;
		testMessage4.messageType = 23;
		testMessage4.packetType = Message::TC;
		testMessage4.applicationId = 5;


		Message receivedMessage(11, 4, Message::TC, 1);
		receivedMessage.appendUint16(4); // Total number of requests

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
		timeSchedulingService.insertActivities(receivedMessage);

		CHECK(unit_test::Tester::scheduledActivities(timeSchedulingService)->size() == 4);

		REQUIRE(scheduledActivities->at(0).requestReleaseTime == 1556435);
		REQUIRE(scheduledActivities->at(1).requestReleaseTime == 1726435);
		REQUIRE(scheduledActivities->at(2).requestReleaseTime == 1957232);
		REQUIRE(scheduledActivities->at(3).requestReleaseTime == 17248435);
		REQUIRE(scheduledActivities->at(0).request == testMessage1);
		REQUIRE(scheduledActivities->at(1).request == testMessage3);
		REQUIRE(scheduledActivities->at(2).request == testMessage2);
		REQUIRE(scheduledActivities->at(3).request == testMessage4);
	}

	SECTION("TC[11,15] Time shift all scheduled activities ", "[positive_shift]") {
		const int32_t timeShift = 6789;

		Message receivedMessage(11, 15, Message::TC, 1);
		receivedMessage.appendSint32(timeShift);

		timeSchedulingService.timeShiftAllActivities(receivedMessage);
		CHECK(scheduledActivities->size() == 4);
		REQUIRE(scheduledActivities->at(0).requestReleaseTime == 1556435 + timeShift);
		REQUIRE(scheduledActivities->at(1).requestReleaseTime == 1726435 + timeShift);
		REQUIRE(scheduledActivities->at(2).requestReleaseTime == 1957232 + timeShift);
		REQUIRE(scheduledActivities->at(3).requestReleaseTime == 17248435 + timeShift);
	}

	SECTION("TC[11,15] Time shift all scheduled activities ", "[negative_shift]") {
		const int32_t timeShift = -6789;

		Message receivedMessage(11, 15, Message::TC, 1);
		receivedMessage.appendSint32(timeShift);

		timeSchedulingService.timeShiftAllActivities(receivedMessage);
		CHECK(scheduledActivities->size() == 4);
		REQUIRE(scheduledActivities->at(0).requestReleaseTime == 1556435);
		REQUIRE(scheduledActivities->at(1).requestReleaseTime == 1726435);
		REQUIRE(scheduledActivities->at(2).requestReleaseTime == 1957232);
		REQUIRE(scheduledActivities->at(3).requestReleaseTime == 17248435);
	}

	SECTION("TC[11,7] Time shift activities by ID") {
		Message receivedMessage(11, 7, Message::TC, 1);
		receivedMessage.appendUint16(4);

	}

	SECTION("TC[11,9] Detail report scheduled activities by ID") {
		Message receivedMessage(11, 9, Message::TC, 1);
		receivedMessage.appendUint16(4);

	}

	SECTION("TC[11,12] Summary report scheduled activities by ID") {
		Message receivedMessage(11, 12, Message::TC, 1);
		receivedMessage.appendUint16(4);

	}

	// todo: Implement the the application ID before proceeding with the test
	SECTION("TC[11,5] Activity deletion by ID") {
		Message receivedMessage(11, 5, Message::TC, 1);
		receivedMessage.appendUint16(1); // Just one instruction to delete an activity
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId);
		receivedMessage.appendUint16(testMessage2.packetSequenceCount);

		timeSchedulingService.deleteActivitiesByID(receivedMessage);
		CHECK(scheduledActivities->size() == 4);
	}

	SECTION("TC[11,16] Detail report all scheduled activities") {
		Message receivedMessage(11, 16, Message::TC, 1);
		receivedMessage.appendUint16(4);

	}
}


/*
TEST_CASE("TC[11,15] Time shift all scheduled activities", "[service][st11]") {
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeSchedulingService);

	Message receivedMessage(11, 15, Message::TC, 1);
	receivedMessage.appendUint32(6789);

	//timeSchedulingService.timeShiftAllActivities(receivedMessage);
	CHECK(scheduledActivities->size() == 4);
	REQUIRE(scheduledActivities->at(0).requestReleaseTime == 1556435);
}*/
/*
TEST_CASE("TC[11,7] Time shift activities by ID", "[service][st11]") {
	Message receivedMessage(11, 7, Message::TC, 1);
	receivedMessage.appendUint16(4);

//timeBasedSchedulingService.detailReporActivitiesByID();
}

TEST_CASE("TC[11,9] Detail report scheduled activities by ID", "[service][st11]") {
	Message receivedMessage(11, 9, Message::TC, 1);
	receivedMessage.appendUint16(4);

//timeBasedSchedulingService.detailReporActivitiesByID();
}

TEST_CASE("TC[11,12] Summary report scheduled activities by ID", "[service][st11]") {
	Message receivedMessage(11, 12, Message::TC, 1);
	receivedMessage.appendUint16(4);

//timeBasedSchedulingService.detailReporActivitiesByID();
}

TEST_CASE("TC[11,159] Time shift all scheduled activities", "[service][st11]") {
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeSchedulingService);

	Message receivedMessage(11, 15, Message::TC, 1);
	receivedMessage.appendUint32(6789);

	//timeSchedulingService.timeShiftAllActivities(receivedMessage);
	CHECK(scheduledActivities->size() == 4);
	REQUIRE(scheduledActivities->at(0).requestReleaseTime == 1556435);
}

TEST_CASE("TC[11,16] Detail report all scheduled activities", "[service][st11]") {
	Message receivedMessage(11, 16, Message::TC, 1);
	receivedMessage.appendUint16(4);

//timeBasedSchedulingService.detailReporActivitiesByID();
}*/
