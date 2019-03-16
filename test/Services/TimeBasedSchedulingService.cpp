#include <catch2/catch.hpp>
#include "ServiceTests.hpp"

// #define private public
#include <Services/TimeBasedSchedulingService.hpp>

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
	auto sch = unit_test::Tester::scheduledActivities(timeSchedulingService);
}

TEST_CASE("TC[11,4]", "[service][st11]") {
	Message receivedMessage, testMessage1, testMessage2, testMessage3;
	// MessageParser msgParser;

	receivedMessage.serviceType = 11;
	receivedMessage.messageType = 4;

	receivedMessage.appendUint16(3);
/*
	// Test activity 1
	testMessage1.serviceType = 6;
	testMessage1.messageType = 5;
	testMessage1.applicationId = 45;
	testMessage1.packetSequenceCount = 67;
	testMessage1.appendUint16(456); // todo: Append dummy data
	receivedMessage.appendUint32(1556435); // todo: Append actual time
	receivedMessage.appendOctetString(msgParser.convertTCToStr(testMessage1));

	// Test activity 2
	testMessage2.serviceType = 6;	receivedMessage.appendUint32(1556435); // todo: Append actual time
	receivedMessage.appendOctetString(msgParser.convertTCToStr(testMessage1));
	testMessage2.messageType = 5;
	testMessage2.applicationId = 45;
	testMessage2.packetSequenceCount = 67;
	testMessage2.appendUint16(456); // todo: Append dummy data
	receivedMessage.appendUint32(1556435); // todo: Append actual time
	receivedMessage.appendOctetString(msgParser.convertTCToStr(testMessage2));
*/
	// Test activity 3
	// testMessage3.serviceType = 6;
	// testMessage3.messageType = 5;
	// testMessage3.applicationId = 45;
	// testMessage3.packetSequenceCount = 67;
	// testMessage3.appendUint16(456); // todo: Append dummy data
	// receivedMessage.appendUint32(1556435); // todo: Append actual time
	// receivedMessage.appendString(msgParser.convertTCToStr(testMessage3));


	TimeBasedSchedulingService timeBasedSchedulingService;
	timeBasedSchedulingService.insertActivities(receivedMessage);

 /*
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedSchedulingService);
	REQUIRE(scheduledActivities.at(0).requestReleaseTime == 1556435);
	REQUIRE(scheduledActivities.at(0).requestID.applicationID == 45);
*/
}
