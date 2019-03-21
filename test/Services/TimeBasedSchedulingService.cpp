#include <catch2/catch.hpp>
#include "ServiceTests.hpp"
#include <Services/TimeBasedSchedulingService.hpp>

#include <ctime>
#include <vector>

/*
 * A namespace defined explicitly for the purposes of testing. This namespace contains a
 * structure, which has been declared as a friend in the TimeBasedSchedulingService class, so
 * that it can access the private members required for testing validation.
 */
namespace unit_test {
	struct Tester {
		static bool executionFunctionStatus(TimeBasedSchedulingService &tmService) {
			return tmService.executionFunctionStatus;
		}

		/*
		 * Read the private member scheduled activities and since it is a list and it can't be
		 * accessed, get each element and save it to a vector.
		 */
		static auto scheduledActivities(TimeBasedSchedulingService &tmService) {
			std::vector<TimeBasedSchedulingService::ScheduledActivity*>listElements;

			for (auto &element : tmService.scheduledActivities) {
				listElements.push_back(&element);
			}
			return listElements; // Return the list elements
		}
	};
}

Message testMessage1, testMessage2, testMessage3, testMessage4;
MessageParser msgParser;
auto currentTime = static_cast<uint32_t >(time(nullptr)); // Get the current system time
bool messagesPopulated = false; // Indicate whether the test messages are initialized

// Run this function to set the service up before moving on with further testing
auto activityInsertion(TimeBasedSchedulingService &timeService) {
	if (not messagesPopulated) {
		// Initialize the test messages
		testMessage1.serviceType = 6;
		testMessage1.messageType = 5;
		testMessage1.packetType = Message::TC;
		testMessage1.applicationId = 8; // todo: Remove the dummy application ID
		testMessage1.appendUint16(4253); // Append dummy data

		testMessage2.serviceType = 4;
		testMessage2.messageType = 5;
		testMessage2.packetType = Message::TC;
		testMessage2.applicationId = 4; // todo: Remove the dummy application ID
		testMessage2.appendUint16(45667); // Append dummy data

		testMessage3.serviceType = 3;
		testMessage3.messageType = 2;
		testMessage3.packetType = Message::TC;
		testMessage3.appendUint16(456); // Append dummy data

		testMessage4.serviceType = 12;
		testMessage4.messageType = 23;
		testMessage4.packetType = Message::TC;
		testMessage4.appendUint16(934); // Append dummy data

		messagesPopulated = true; // Indicate initialized test messages
	}

	Message receivedMessage(11, 4, Message::TC, 1);
	receivedMessage.appendUint16(4); // Total number of requests

	// Test activity 1
	receivedMessage.appendUint32(currentTime + 1556435);
	receivedMessage.appendString(msgParser.convertTCToStr(testMessage1));

	// Test activity 2
	receivedMessage.appendUint32(currentTime + 1957232);
	receivedMessage.appendString(msgParser.convertTCToStr(testMessage2));

	// Test activity 3
	receivedMessage.appendUint32(currentTime + 1726435);
	receivedMessage.appendString(msgParser.convertTCToStr(testMessage3));

	// Test activity 4
	receivedMessage.appendUint32(currentTime + 17248435);
	receivedMessage.appendString(msgParser.convertTCToStr(testMessage4));

	// Insert activities in the schedule. They have to be inserted sorted
	timeService.insertActivities(receivedMessage);

	return unit_test::Tester::scheduledActivities(timeService); // Return the activities vector
}


TEST_CASE("TC[11,1] Enable Schedule Execution", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	Message receivedMessage(11, 1, Message::TC, 1);

	timeService.enableScheduleExecution(receivedMessage);
	CHECK(unit_test::Tester::executionFunctionStatus(timeService));
}

TEST_CASE("TC[11,2] Disable Schedule Execution", "[service][st11]") {
	Message receivedMessage(11, 2, Message::TC, 1);
	TimeBasedSchedulingService timeService;

	timeService.disableScheduleExecution(receivedMessage);
	CHECK(not unit_test::Tester::executionFunctionStatus(timeService));
}

TEST_CASE("TC[11,4] Activity Insertion", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	REQUIRE(scheduledActivities.size() == 4);
	REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435);
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232);
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435);
	REQUIRE(scheduledActivities.at(0)->request == testMessage1);
	REQUIRE(scheduledActivities.at(1)->request == testMessage3);
	REQUIRE(scheduledActivities.at(2)->request == testMessage2);
	REQUIRE(scheduledActivities.at(3)->request == testMessage4);
}

TEST_CASE("TC[11,15] Time shift all scheduled activities (Positive shift)", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);
	const int32_t timeShift = 6789;

	Message receivedMessage(11, 15, Message::TC, 1);
	receivedMessage.appendSint32(timeShift);

	CHECK(scheduledActivities.size() == 4);
	timeService.timeShiftAllActivities(receivedMessage);

	REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435 + timeShift);
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435 + timeShift);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232 + timeShift);
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435 + timeShift);
}

TEST_CASE("TC[11,15] Time shift all scheduled activities (Negative shift)", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);
	const int32_t timeShift = 6789;

	Message receivedMessage(11, 15, Message::TC, 1);
	receivedMessage.appendSint32(-timeShift);

	CHECK(scheduledActivities.size() == 4);
	timeService.timeShiftAllActivities(receivedMessage);

	REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435 - timeShift);
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435 - timeShift);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232 - timeShift);
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435 - timeShift);

}

TEST_CASE("TC[11,7] Time shift activities by ID (Positive Shift)", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);
	const int32_t timeShift = 67890000; // Relative time-shift value

	// Verify that everything is in place
	CHECK(scheduledActivities.size() == 4);
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

	Message receivedMessage(11, 7, Message::TC, 1);
	receivedMessage.appendSint32(timeShift); // Time-shift value
	receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count


	timeService.timeShiftActivitiesByID(receivedMessage);
	scheduledActivities = unit_test::Tester::scheduledActivities(timeService);

	// Make sure the new value is inserted sorted
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 1957232 + timeShift);
	REQUIRE(scheduledActivities.at(3)->request == testMessage2);
}

TEST_CASE("TC[11,7] Time shift activities by ID (Negative Shift)", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);
	const int32_t timeShift = 250000; // Relative time-shift value

	// Verify that everything is in place
	CHECK(scheduledActivities.size() == 4);
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

	Message receivedMessage(11, 7, Message::TC, 1);
	receivedMessage.appendSint32(-timeShift); // Time-shift value
	receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count


	timeService.timeShiftActivitiesByID(receivedMessage);
	scheduledActivities = unit_test::Tester::scheduledActivities(timeService);

	// Output should be sorted
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1957232 - timeShift);
	REQUIRE(scheduledActivities.at(1)->request == testMessage2);
}


TEST_CASE("TC[11,9] Detail report scheduled activities by ID", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	// Verify that everything is in place
	CHECK(scheduledActivities.size() == 4);
	scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

	Message receivedMessage(11, 9, Message::TC, 1);
	receivedMessage.appendUint16(2); // Two instructions in the request
	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

	timeService.detailReportActivitiesByID(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 11);
	CHECK(response.messageType == 10);

	uint16_t iterationCount = response.readUint16();
	for (uint16_t i = 0; i < iterationCount; i++) {
		uint32_t receivedReleaseTime = response.readUint32();

		Message receivedTCPacket;
		uint8_t receivedDataStr[ECSS_TC_REQUEST_STRING_SIZE];
		response.readString(receivedDataStr, ECSS_TC_REQUEST_STRING_SIZE);
		receivedTCPacket = msgParser.parseRequestTC(receivedDataStr);

		if (i == 0) {
			REQUIRE(receivedReleaseTime == scheduledActivities.at(0)->requestReleaseTime);
			REQUIRE(receivedTCPacket == scheduledActivities.at(0)->request);
		} else {
			REQUIRE(receivedReleaseTime == scheduledActivities.at(2)->requestReleaseTime);
			REQUIRE(receivedTCPacket == scheduledActivities.at(2)->request);
		}
	}
}

TEST_CASE("TC[11,12] Summary report scheduled activities by ID", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	// Verify that everything is in place
	CHECK(scheduledActivities.size() == 4);
	scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

	Message receivedMessage(11, 12, Message::TC, 1);
	receivedMessage.appendUint16(2); // Two instructions in the request
	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

	timeService.summaryReportActivitiesByID(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 11);
	CHECK(response.messageType == 13);

	uint16_t iterationCount = response.readUint16();
	for (uint16_t i = 0; i < iterationCount; i++) {
		uint32_t receivedReleaseTime = response.readUint32();
		uint8_t receivedSourceID = response.readUint8();
		uint16_t receivedApplicationID = response.readUint16();
		uint16_t receivedSequenceCount = response.readUint16();

		if (i == 0) {
			REQUIRE(receivedReleaseTime == scheduledActivities.at(0)->requestReleaseTime);
			REQUIRE(receivedSourceID == scheduledActivities.at(0)->requestID.sourceID);
			REQUIRE(receivedApplicationID == scheduledActivities.at(0)->requestID.applicationID);
			REQUIRE(receivedSequenceCount == scheduledActivities.at(0)->requestID.sequenceCount);
		} else {
			REQUIRE(receivedReleaseTime == scheduledActivities.at(2)->requestReleaseTime);
			REQUIRE(receivedSourceID == scheduledActivities.at(2)->requestID.sourceID);
			REQUIRE(receivedApplicationID == scheduledActivities.at(2)->requestID.applicationID);
			REQUIRE(receivedSequenceCount == scheduledActivities.at(2)->requestID.sequenceCount);
		}
	}
}

TEST_CASE("TC[11,16] Detail report all scheduled activities", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	Message receivedMessage(11, 16, Message::TC, 1);
	timeService.detailReportAllActivities(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 11);
	CHECK(response.messageType == 10);

	uint16_t iterationCount = response.readUint16();
	REQUIRE(iterationCount == scheduledActivities.size());

	for (uint16_t i = 0; i < iterationCount; i++) {
		uint32_t receivedReleaseTime = response.readUint32();

		Message receivedTCPacket;
		uint8_t receivedDataStr[ECSS_TC_REQUEST_STRING_SIZE];
		response.readString(receivedDataStr, ECSS_TC_REQUEST_STRING_SIZE);
		receivedTCPacket = msgParser.parseRequestTC(receivedDataStr);

		REQUIRE(receivedReleaseTime == scheduledActivities.at(i)->requestReleaseTime);
		REQUIRE(scheduledActivities.at(i)->request == receivedTCPacket);
	}
}

TEST_CASE("TC[11,5] Activity deletion by ID", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	// Verify that everything is in place
	CHECK(scheduledActivities.size() == 4);
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

	Message receivedMessage(11, 5, Message::TC, 1);
	receivedMessage.appendUint16(1); // Just one instruction to delete an activity
	receivedMessage.appendUint8(0); // Source ID is not implemented
	receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
	receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

	CHECK(scheduledActivities.size() == 4);
	timeService.deleteActivitiesByID(receivedMessage);
	scheduledActivities = unit_test::Tester::scheduledActivities(timeService); // Get the new list

	REQUIRE(scheduledActivities.size() == 3);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 17248435);
	REQUIRE(scheduledActivities.at(2)->request == testMessage4);
}

TEST_CASE("TC[11,3] Reset schedule", "[service][st11]") {
	TimeBasedSchedulingService timeService;
	auto scheduledActivities = activityInsertion(timeService);

	Message receivedMessage(11, 3, Message::TC, 1);

	timeService.resetSchedule(receivedMessage);
	scheduledActivities = unit_test::Tester::scheduledActivities(timeService); // Get the new list

	REQUIRE(scheduledActivities.empty());
	REQUIRE(not unit_test::Tester::executionFunctionStatus(timeService));
}
