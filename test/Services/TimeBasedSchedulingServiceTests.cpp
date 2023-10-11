#include <Services/TimeBasedSchedulingService.hpp>
#include <catch2/catch_all.hpp>
#include "ServiceTests.hpp"

#include <ctime>
#include <vector>

using namespace std::chrono_literals;

/*
 * A namespace defined explicitly for the purposes of testing. This namespace contains a
 * structure, which has been declared as a friend in the TimeBasedSchedulingService class, so
 * that it can access the private members required for testing validation.
 */
namespace unit_test {
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

			std::transform(
			    tmService.scheduledActivities.begin(), tmService.scheduledActivities.end(),
			    std::back_inserter(listElements), [](auto& activity) -> auto{ return &activity; });

			return listElements; // Return the list elements
		}
	};
} // namespace unit_test

Message testMessage1, testMessage2, testMessage3, testMessage4;
Time::DefaultCUC currentTime = TimeGetter::getCurrentTimeDefaultCUC(); // Get the current system time
bool messagesPopulated = false;                                        // Indicate whether the test messages are initialized

// Run this function to set the service up before moving on with further testing
auto activityInsertion(TimeBasedSchedulingService& timeService) {
	if (not messagesPopulated) {
		// Initialize the test messages
		testMessage1.serviceType = 6;
		testMessage1.messageType = 5;
		testMessage1.packetType = Message::TC;
		testMessage1.applicationId = 8;  // todo: Remove the dummy application ID
		testMessage1.appendUint16(4253); // Append dummy data

		testMessage2.serviceType = 4;
		testMessage2.messageType = 5;
		testMessage2.packetType = Message::TC;
		testMessage2.applicationId = 4;   // todo: Remove the dummy application ID
		testMessage2.appendUint16(45667); // Append dummy data

		testMessage3.serviceType = 3;
		testMessage3.messageType = 2;
		testMessage3.packetType = Message::TC;
		testMessage3.appendUint16(456); // Append dummy data

		testMessage4.serviceType = 12;
		testMessage4.messageType = 3;
		testMessage4.packetType = Message::TC;
		testMessage4.appendUint16(934); // Append dummy data

		messagesPopulated = true; // Indicate initialized test messages
	}

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, 1);
	receivedMessage.appendUint16(4); // Total number of requests
	receivedMessage.sourceId = 0; // todo: proper handling of sourceID when globally integrated

	// Test activity 1
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 155643s);
	receivedMessage.appendMessage(testMessage1, ECSSTCRequestStringSize);

	// Test activity 2
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 195723s);
	receivedMessage.appendMessage(testMessage2, ECSSTCRequestStringSize);

	// Test activity 3
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 172643s);
	receivedMessage.appendMessage(testMessage3, ECSSTCRequestStringSize);

	// Test activity 4
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 1724843s);
	receivedMessage.appendMessage(testMessage4, ECSSTCRequestStringSize);

	// Insert activities in the schedule. They have to be inserted sorted
	timeService.insertActivities(receivedMessage);

	return unit_test::Tester::scheduledActivities(timeService); // Return the activities vector
}


TimeBasedSchedulingService& timeBasedService = Services.timeBasedScheduling;

TEST_CASE("Execute the first activity, removes it from the list and return the release time of next activity to be executed") {
	Services.reset();
	auto scheduledActivities = activityInsertion(timeBasedService);

	auto nextActivityExecutionCUCTime = timeBasedService.executeScheduledActivity(currentTime + 155643s);
	REQUIRE(nextActivityExecutionCUCTime == currentTime + 172643s);

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportAllScheduledActivities, Message::TC, 1);
	timeBasedService.detailReportAllActivities(receivedMessage);
	Message response = ServiceTests::get(0);
	uint16_t iterationCount = response.readUint16();
	REQUIRE(iterationCount == 3);

	nextActivityExecutionCUCTime = timeBasedService.executeScheduledActivity(currentTime + 10s);
	REQUIRE(nextActivityExecutionCUCTime == currentTime + 172643s);

	nextActivityExecutionCUCTime = timeBasedService.executeScheduledActivity(currentTime + 172643s);
	REQUIRE(nextActivityExecutionCUCTime == currentTime + 195723s);

	timeBasedService.detailReportAllActivities(receivedMessage);
	response = ServiceTests::get(1);
	iterationCount = response.readUint16();
	REQUIRE(iterationCount == 2);

	nextActivityExecutionCUCTime = timeBasedService.executeScheduledActivity(currentTime + 195723s);
	REQUIRE(nextActivityExecutionCUCTime == currentTime + 1724843s);

	timeBasedService.detailReportAllActivities(receivedMessage);
	response = ServiceTests::get(2);
	iterationCount = response.readUint16();
	REQUIRE(iterationCount == 1);

	nextActivityExecutionCUCTime = timeBasedService.executeScheduledActivity(currentTime + 1724843s);
	REQUIRE(nextActivityExecutionCUCTime == Time::DefaultCUC::max());

	timeBasedService.detailReportAllActivities(receivedMessage);
	response = ServiceTests::get(3);
	iterationCount = response.readUint16();
	REQUIRE(iterationCount == 0);
}

TEST_CASE("TC[11,1] Enable Schedule Execution", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::EnableTimeBasedScheduleExecutionFunction, Message::TC, 1);

	MessageParser::execute(receivedMessage); //timeService.enableScheduleExecution(receivedMessage);
	CHECK(unit_test::Tester::executionFunctionStatus(timeBasedService));
}

TEST_CASE("TC[11,2] Disable Schedule Execution", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DisableTimeBasedScheduleExecutionFunction, Message::TC, 1);

	MessageParser::execute(receivedMessage); //timeService.disableScheduleExecution(receivedMessage);
	CHECK(not unit_test::Tester::executionFunctionStatus(timeBasedService));
}

TEST_CASE("TC[11,4] Activity Insertion", "[service][st11]") {
	Services.reset();
	auto scheduledActivities = activityInsertion(timeBasedService);

	REQUIRE(scheduledActivities.size() == 4);

	REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 155643s);
	REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 172643s);
	REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 195723s);
	REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 1724843s);

	REQUIRE(testMessage1.bytesEqualWith(scheduledActivities.at(0)->request));
	REQUIRE(testMessage3.bytesEqualWith(scheduledActivities.at(1)->request));
	REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(2)->request));
	REQUIRE(testMessage4.bytesEqualWith(scheduledActivities.at(3)->request));

	SECTION("Error throw test") {
		Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, 1);
		receivedMessage.appendUint16(1); // Total number of requests

		receivedMessage.appendDefaultCUCTimeStamp(currentTime - 155643s);
		MessageParser::execute(receivedMessage); //timeService.insertActivities(receivedMessage);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,15] Time shift all scheduled activities", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftALlScheduledActivities, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	const Time::RelativeTime timeShift = 6789;

	SECTION("Positive Shift") {
		receivedMessage.appendRelativeTime(-timeShift);

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage); //timeService.timeShiftAllActivities(receivedMessage);

		REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 155643s - std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 172643s - std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 195723s - std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 1724843s - std::chrono::seconds(timeShift));
	}

	SECTION("Negative Shift") {
		receivedMessage.appendRelativeTime(timeShift);

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage); //timeService.timeShiftAllActivities(receivedMessage);
		REQUIRE(scheduledActivities.at(0)->requestReleaseTime == currentTime + 155643s + std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 172643s + std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 195723s + std::chrono::seconds(timeShift));
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 1724843s + std::chrono::seconds(timeShift));
	}

	SECTION("Error throwing") {
		receivedMessage.appendRelativeTime(-6789000); // Provide a huge time shift to cause an error

		CHECK(scheduledActivities.size() == 4);
		MessageParser::execute(receivedMessage); //timeService.timeShiftAllActivities(receivedMessage);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::SubServiceExecutionStartError));
	}
}

TEST_CASE("TC[11,7] Time shift activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID
	CHECK(scheduledActivities.size() == 4);

	const Time::RelativeTime timeShift = 67890000; // Relative time-shift value

	SECTION("Positive Shift") {
		receivedMessage.appendRelativeTime(timeShift);            // Time-shift value
		receivedMessage.appendUint16(1);                          // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Make sure the new value is inserted sorted
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 195723s + std::chrono::seconds(timeShift));
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(3)->request));
	}

	SECTION("Negative Shift") {
		receivedMessage.appendRelativeTime(-25000);              // Time-shift value
		receivedMessage.appendUint16(1);                          // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Output should be sorted
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 195723s - 25000s);
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(1)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendRelativeTime(-250000); // Time-shift value
		receivedMessage.appendUint16(1);             // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);              // Dummy source ID
		receivedMessage.appendUint16(80);            // Dummy application ID to throw an error
		receivedMessage.appendUint16(0);             // Dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}

	SECTION("Error throw on wrong time offset") {
		receivedMessage.appendRelativeTime(-6789000);             // Time-shift value
		receivedMessage.appendUint16(1);                          // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,9] Detail report scheduled activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Detailed activity report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2);                          // Two instructions in the request
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == 10);

		uint16_t iterationCount = response.readUint16();
		CHECK(iterationCount == 2);
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime = response.readDefaultCUCTimeStamp();

			Message receivedTCPacket;
			uint8_t receivedDataStr[ECSSTCRequestStringSize];
			response.readString(receivedDataStr, ECSSTCRequestStringSize);
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
		receivedMessage.appendUint16(1);  // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);   // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0);  // Dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TM[11,10] time-based schedule detail report", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Detailed activity report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2);                          // Two instructions in the request
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == 10);

		uint16_t iterationCount = response.readUint16();
		CHECK(iterationCount == 2);
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime = response.readDefaultCUCTimeStamp();

			Message receivedTCPacket;
			uint8_t receivedDataStr[ECSSTCRequestStringSize];
			response.readString(receivedDataStr, ECSSTCRequestStringSize);
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
}

TEST_CASE("TC[11,12] Summary report scheduled activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ActivitiesSummaryReportById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Summary report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2);                          // Two instructions in the request
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == 13);

		uint16_t iterationCount = response.readUint16();
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime = response.readDefaultCUCTimeStamp();
			uint16_t receivedSourceID = response.readUint16();
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
		receivedMessage.appendUint16(1);  // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);   // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0);  // Dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TM[11,13] time-based schedule summary report", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ActivitiesSummaryReportById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Summary report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(0)->requestID.applicationID = 8; // Append a dummy application ID
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(2);                          // Two instructions in the request
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage1.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == 13);

		uint16_t iterationCount = response.readUint16();
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime = response.readDefaultCUCTimeStamp();
			uint16_t receivedSourceID = response.readUint16();
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
}

TEST_CASE("TC[11,16] Detail report all scheduled activities", "[service][st11]") {
	Services.reset();
	auto scheduledActivities = activityInsertion(timeBasedService);

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportAllScheduledActivities, Message::TC, 1);
	timeBasedService.detailReportAllActivities(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 11);
	CHECK(response.messageType == 10);

	uint16_t iterationCount = response.readUint16();
	REQUIRE(iterationCount == scheduledActivities.size());

	for (uint16_t i = 0; i < iterationCount; i++) {
		Time::DefaultCUC receivedReleaseTime = response.readDefaultCUCTimeStamp();

		Message receivedTCPacket;
		uint8_t receivedDataStr[ECSSTCRequestStringSize];
		response.readString(receivedDataStr, ECSSTCRequestStringSize);
		receivedTCPacket = MessageParser::parseECSSTC(receivedDataStr);
		REQUIRE(receivedReleaseTime == scheduledActivities.at(i)->requestReleaseTime);
		REQUIRE(receivedTCPacket.bytesEqualWith(scheduledActivities.at(i)->request));
	}
}

TEST_CASE("TC[11,5] Activity deletion by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DeleteActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Activity deletion") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);
		scheduledActivities.at(2)->requestID.applicationID = 4; // Append a dummy application ID

		receivedMessage.appendUint16(1);                          // Just one instruction to delete an activity
		receivedMessage.appendUint16(0);                           // Source ID is not implemented
		receivedMessage.appendUint16(testMessage2.applicationId); // todo: Remove the dummy app ID
		receivedMessage.appendUint16(0);                          // todo: Remove the dummy sequence count

		CHECK(scheduledActivities.size() == 4);
		timeBasedService.deleteActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		REQUIRE(scheduledActivities.size() == 3);
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1724843s);
		REQUIRE(testMessage4.bytesEqualWith(scheduledActivities.at(2)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendUint16(1);  // Just one instruction to time-shift an activity
		receivedMessage.appendUint16(0);   // Dummy source ID
		receivedMessage.appendUint16(80); // Dummy application ID to throw an error
		receivedMessage.appendUint16(0);  // Dummy sequence count

		timeBasedService.deleteActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,3] Reset schedule", "[service][st11]") {
	Services.reset();
	activityInsertion(timeBasedService);

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ResetTimeBasedSchedule, Message::TC, 1);

	timeBasedService.resetSchedule(receivedMessage);
	auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService); // Get the new list

	REQUIRE(scheduledActivities.empty());
	REQUIRE(not unit_test::Tester::executionFunctionStatus(timeBasedService));
}
