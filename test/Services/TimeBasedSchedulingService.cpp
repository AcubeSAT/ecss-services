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
namespace unit_test
{
	struct Tester {
		static bool executionFunctionStatus(TimeBasedSchedulingService& tmService) {
			return tmService.executionFunctionStatus;
		}

		/*
		 * Read the private member scheduled activities and since it is a list and it can't be
		 * accessed, get each element and save it to a vector.
		 */
		static auto scheduledActivities(TimeBasedSchedulingService& tmService) {
			std::vector<TimeBasedSchedulingService::ScheduledActivity*> listElements;

			std::transform(tmService.scheduledActivities.begin(), tmService.scheduledActivities.end(),
				std::back_inserter(listElements), [](auto& activity) -> auto { return &activity; });

			return listElements; // Return the list elements
		}
	};
} // namespace unit_test

Message testMessage1, testMessage2, testMessage3, testMessage4;
auto currentTime = static_cast<uint32_t>(time(nullptr)); // Get the current system time
bool messagesPopulated = false; // Indicate whether the test messages are initialized

// Run this function to set the service up before moving on with further testing
auto activityInsertion(TimeBasedSchedulingService& timeService) {
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
	receivedMessage.appendMessage(testMessage1, ECSS_TC_REQUEST_STRING_SIZE);

	// Test activity 2
	receivedMessage.appendUint32(currentTime + 1957232);
	receivedMessage.appendMessage(testMessage2, ECSS_TC_REQUEST_STRING_SIZE);

	// Test activity 3
	receivedMessage.appendUint32(currentTime + 1726435);
	receivedMessage.appendMessage(testMessage3, ECSS_TC_REQUEST_STRING_SIZE);

	// Test activity 4
	receivedMessage.appendUint32(currentTime + 17248435);
	receivedMessage.appendMessage(testMessage4, ECSS_TC_REQUEST_STRING_SIZE);

	// Insert activities in the schedule. They have to be inserted sorted
	timeService.insertActivities(receivedMessage);

	return unit_test::Tester::scheduledActivities(timeService); // Return the activities vector
}


TimeBasedSchedulingService & timeBasedService = Services.timeBasedScheduling;

TEST_CASE("TC[11,1] Enable Schedule Execution", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 1, Message::TC, 1);

	MessageParser::execute(receivedMessage);//timeService.enableScheduleExecution(receivedMessage);
	CHECK(unit_test::Tester::executionFunctionStatus(timeBasedService));
}

TEST_CASE("TC[11,2] Disable Schedule Execution", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 2, Message::TC, 1);

	MessageParser::execute(receivedMessage);//timeService.disableScheduleExecution(receivedMessage);
	CHECK(not unit_test::Tester::executionFunctionStatus(timeBasedService));
}

TEST_CASE("TC[11,4] Activity Insertion", "[service][st11]") {
	Services.reset();
	auto scheduledActivities = activityInsertion(timeBasedService);

	REQUIRE(scheduledActivities.size() == 4);

	REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435);
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232);
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435);
	REQUIRE(testMessage1.bytesEqualWith(scheduledActivities.at(0)->request));
	REQUIRE(testMessage3.bytesEqualWith(scheduledActivities.at(1)->request));
	REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(2)->request));
	REQUIRE(testMessage4.bytesEqualWith(scheduledActivities.at(3)->request));

	SECTION("Error throw test") {
		Message receivedMessage(11, 4, Message::TC, 1);
		receivedMessage.appendUint16(1); // Total number of requests

		receivedMessage.appendUint32(currentTime - 15564350);
		MessageParser::execute(receivedMessage);//timeService.insertActivities(receivedMessage);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,15] Time shift all scheduled activities", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 15, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	const int32_t timeShift = 6789;

	SECTION("Positive Shift") {
		receivedMessage.appendSint32(-timeShift);

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage);//timeService.timeShiftAllActivities(receivedMessage);

		REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435 - timeShift);
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435 - timeShift);
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232 - timeShift);
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435 - timeShift);
	}

	SECTION("Negative Shift") {
		receivedMessage.appendSint32(timeShift);

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage);//timeService.timeShiftAllActivities(receivedMessage);

		REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 1556435 + timeShift);
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1726435 + timeShift);
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1957232 + timeShift);
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 17248435 + timeShift);
	}

	SECTION("Error throwing") {
		receivedMessage.appendSint32(-6789000); // Provide a huge time shift to cause an error

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage);//timeService.timeShiftAllActivities(receivedMessage);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::SubServiceExecutionStartError));
	}
}

TEST_CASE("TC[11,7] Time shift activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 7, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID
	CHECK(scheduledActivities.size() == 4);

	const int32_t timeShift = 67890000; // Relative time-shift value

	SECTION("Positive Shift") {
		receivedMessage.appendSint32(timeShift); // Time-shift value
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Make sure the new value is inserted sorted
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 1957232 + timeShift);
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(3)->request));
	}

	SECTION("Negative Shift") {
		receivedMessage.appendSint32(-250000); // Time-shift value
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Output should be sorted
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 1957232 - 250000);
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(1)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendSint32(-250000); // Time-shift value
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0); // Dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}

	SECTION("Error throw on wrong time offset") {
		receivedMessage.appendSint32(-6789000); // Time-shift value
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,9] Detail report scheduled activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 9, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Detailed activity report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2); // Two instructions in the request
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == 10);

		uint16_t iterationCount = response.readUint16();
		CHECK(iterationCount == 2);
		for (uint16_t i = 0; i < iterationCount; i++) {
			uint32_t receivedReleaseTime = response.readUint32();

			Message receivedTCPacket;
			uint8_t receivedDataStr[ECSS_TC_REQUEST_STRING_SIZE];
			response.readString(receivedDataStr, ECSS_TC_REQUEST_STRING_SIZE);
			receivedTCPacket = MessageParser::parseECSSTC(receivedDataStr);

			if (i == 0) {
				REQUIRE(receivedReleaseTime == scheduledActivities.at(0)->requestReleaseTime);
				REQUIRE(receivedTCPacket == scheduledActivities.at(0)->request);
			} else {
				REQUIRE(receivedReleaseTime == scheduledActivities.at(2)->requestReleaseTime);
				REQUIRE(receivedTCPacket == scheduledActivities.at(2)->request);
			}
		}
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0); // Dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,12] Summary report scheduled activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 12, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Summary report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2); // Two instructions in the request
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
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

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0); // Dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,16] Detail report all scheduled activities", "[service][st11]") {
	Services.reset();
	auto scheduledActivities = activityInsertion(timeBasedService);

	Message receivedMessage(11, 16, Message::TC, 1);
	timeBasedService.detailReportAllActivities(receivedMessage);
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
		receivedTCPacket = MessageParser::parseECSSTC(receivedDataStr);

		REQUIRE(receivedReleaseTime == scheduledActivities.at(i)->requestReleaseTime);
		REQUIRE(receivedTCPacket.bytesEqualWith(scheduledActivities.at(i)->request));
	}
}

TEST_CASE("TC[11,5] Activity deletion by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(11, 5, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Activity deletion") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(1); // Just one instruction to delete an activity
		receivedMessage.appendUint8(0); // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0); // todo: Remove the dummy sequence count

		CHECK(scheduledActivities.size() == 4);
		timeBasedService.deleteActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		REQUIRE(scheduledActivities.size() == 3);
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 17248435);
		REQUIRE(testMessage4.bytesEqualWith(scheduledActivities.at(2)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendUint16(1); // Just one instruction to time-shift an activity
		receivedMessage.appendUint8(0); // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0); // Dummy sequence count

		timeBasedService.deleteActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,3] Reset schedule", "[service][st11]") {
	Services.reset();
	activityInsertion(timeBasedService);

	Message receivedMessage(11, 3, Message::TC, 1);

	timeBasedService.resetSchedule(receivedMessage);
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService); // Get the new list

	REQUIRE(scheduledActivities.empty());
	REQUIRE(not unit_test::Tester::executionFunctionStatus(timeBasedService));
}
