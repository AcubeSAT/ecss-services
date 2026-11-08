#include <Services/TimeBasedSchedulingService.hpp>
#include <catch2/catch_all.hpp>
#include "ServiceTests.hpp"

#include <ctime>
#include <limits>
#include <optional>
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
			    std::back_inserter(listElements), [](auto& activity) -> auto { return &activity; });

			return listElements; // Return the list elements
		}
	};
} // namespace unit_test

Message testMessage1, testMessage2, testMessage3, testMessage4;
Time::DefaultCUC currentTime;
bool messagesPopulated = false; // Indicate whether the test messages are initialized

constexpr uint16_t singleInstruction = 1;
constexpr size_t singleActivity = 1;
constexpr uint64_t singleError = 1;
constexpr SequenceCount testMessage1SequenceCount = 1;
constexpr SequenceCount testMessage2SequenceCount = 2;
constexpr SequenceCount testMessage3SequenceCount = 3;
constexpr SequenceCount testMessage4SequenceCount = 4;

/**
 * Appends a TC[11,4] instruction that schedules @p request, as a full TC packet, at @p releaseTime
 */
static void appendInstruction(Message& insertRequest, const Time::DefaultCUC& releaseTime, const Message& request) {
	insertRequest.appendDefaultCUCTimeStamp(releaseTime);
	insertRequest.appendString(MessageParser::compose(request));
}

/**
 * Appends the request identifier of @p request to a TC[11,5], TC[11,7], TC[11,9] or TC[11,12] instruction
 */
static void appendRequestID(Message& message, const Message& request) {
	message.append<ApplicationProcessUserId>(request.sourceId);
	message.append<ApplicationProcessId>(request.applicationId);
	message.append<SequenceCount>(request.packetSequenceCount);
}

/**
 * Reads the request of a TM[11,10] notification, which is a full TC packet
 */
static Message readRequest(Message& report) {
	const uint8_t* const request = report.data.data() + report.readPosition;
	const uint32_t requestSize = MessageParser::packetSize(request);
	report.skipBytes(requestSize);
	return MessageParser::parse(request, requestSize);
}

// Run this function to set the service up before moving on with further testing
auto activityInsertion(TimeBasedSchedulingService& timeService) {
	currentTime = TimeGetter::getCurrentTimeDefaultCUC();

	if (not messagesPopulated) {
		// Initialize the test messages
		testMessage1.serviceType = MemoryManagementService::ServiceType;
		testMessage1.messageType = MemoryManagementService::MessageType::DumpRawMemoryData;
		testMessage1.packetType = Message::TC;
		testMessage1.packetSequenceCount = testMessage1SequenceCount;
		testMessage1.appendUint16(4253); // Append dummy data

		testMessage2.serviceType = ParameterStatisticsService::ServiceType;
		testMessage2.messageType = ParameterStatisticsService::MessageType::DisablePeriodicParameterReporting;
		testMessage2.packetType = Message::TC;
		testMessage2.packetSequenceCount = testMessage2SequenceCount;
		testMessage2.appendUint16(45667); // Append dummy data

		testMessage3.serviceType = HousekeepingService::ServiceType;
		testMessage3.messageType = HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport;
		testMessage3.packetType = Message::TC;
		testMessage3.packetSequenceCount = testMessage3SequenceCount;
		testMessage3.appendUint16(456); // Append dummy data

		testMessage4.serviceType = OnBoardMonitoringService::ServiceType;
		testMessage4.messageType = OnBoardMonitoringService::MessageType::ChangeMaximumTransitionReportingDelay;
		testMessage4.packetType = Message::TC;
		testMessage4.packetSequenceCount = testMessage4SequenceCount;
		testMessage4.appendUint16(934); // Append dummy data

		messagesPopulated = true; // Indicate initialized test messages
	}

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
	receivedMessage.appendUint16(4); // Total number of requests

	// Test activity 1
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 155643s);
	receivedMessage.appendString(MessageParser::compose(testMessage1));

	// Test activity 2
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 195723s);
	receivedMessage.appendString(MessageParser::compose(testMessage2));

	// Test activity 3
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 172643s);
	receivedMessage.appendString(MessageParser::compose(testMessage3));

	// Test activity 4
	receivedMessage.appendDefaultCUCTimeStamp(currentTime + 1724843s);
	receivedMessage.appendString(MessageParser::compose(testMessage4));

	// Insert activities in the schedule. They have to be inserted sorted
	timeService.insertActivities(receivedMessage);

	return unit_test::Tester::scheduledActivities(timeService); // Return the activities vector
}


TimeBasedSchedulingService& timeBasedService = Services.timeBasedScheduling;

/**
 * Enables the time-based schedule execution function with a TC[11,1]
 */
static void enableScheduleExecution() {
	const Message request(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::EnableTimeBasedScheduleExecutionFunction, Message::TC, ApplicationId);
	timeBasedService.enableScheduleExecution(request);
}

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

/**
 * Inserts an activity with its own TC[11,4], with @p sequenceCount as the sequence count of its request, so that it
 * tells the activity apart from other activities.
 */
static void insertActivity(const Message& request, const Time::DefaultCUC& releaseTime, SequenceCount sequenceCount) {
	Message scheduledRequest = request;
	scheduledRequest.packetSequenceCount = sequenceCount;

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
	receivedMessage.appendUint16(singleInstruction);
	appendInstruction(receivedMessage, releaseTime, scheduledRequest);

	timeBasedService.insertActivities(receivedMessage);
}

TEST_CASE("Execute an activity whose request changes the schedule", "[service][st11]") {
	Services.reset();
	const Time::DefaultCUC now(TimeGetter::getCurrentTimeDefaultCUC());
	const Time::DefaultCUC firstReleaseTime = now + 100s;
	const Time::DefaultCUC secondReleaseTime = now + 200s;
	const Message otherRequest(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	enableScheduleExecution();

	SECTION("TC[11,3] resets the schedule") {
		const Message resetRequest(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ResetTimeBasedSchedule, Message::TC, ApplicationId);
		insertActivity(resetRequest, firstReleaseTime, 0);
		insertActivity(otherRequest, secondReleaseTime, 1);

		REQUIRE(timeBasedService.executeScheduledActivity(firstReleaseTime) == Time::DefaultCUC::max());
		REQUIRE(unit_test::Tester::scheduledActivities(timeBasedService).empty());
		REQUIRE(not unit_test::Tester::executionFunctionStatus(timeBasedService));

		const Time::DefaultCUC releaseTimeAfterReset = now + 300s;
		constexpr SequenceCount sequenceCountAfterReset = 2;
		insertActivity(otherRequest, releaseTimeAfterReset, sequenceCountAfterReset);
		REQUIRE(unit_test::Tester::scheduledActivities(timeBasedService).size() == singleActivity);

		enableScheduleExecution();
		REQUIRE(timeBasedService.executeScheduledActivity(releaseTimeAfterReset) == Time::DefaultCUC::max());
		CHECK(ServiceTests::hasNoErrors());
		REQUIRE(ServiceTests::hasOneMessage());
		CHECK(ServiceTests::get(0).messageType == TestService::MessageType::AreYouAliveTestReport);
	}

	SECTION("TC[11,5] asks to delete its own activity") {
		const ApplicationProcessUserId sourceID = 0;
		const SequenceCount deleteActivitySequenceCount = 0;
		const SequenceCount otherActivitySequenceCount = 1;
		Message deleteRequest(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DeleteActivitiesById, Message::TC, ApplicationId);
		deleteRequest.appendUint16(1);
		deleteRequest.append<ApplicationProcessUserId>(sourceID);
		deleteRequest.append<ApplicationProcessId>(ApplicationId);
		deleteRequest.append<SequenceCount>(deleteActivitySequenceCount);
		insertActivity(deleteRequest, firstReleaseTime, deleteActivitySequenceCount);
		insertActivity(otherRequest, secondReleaseTime, otherActivitySequenceCount);

		const auto deleteActivityID = unit_test::Tester::scheduledActivities(timeBasedService).at(0)->requestID;
		REQUIRE(deleteActivityID.sourceID == sourceID);
		REQUIRE(deleteActivityID.applicationID == ApplicationId);
		REQUIRE(deleteActivityID.sequenceCount == deleteActivitySequenceCount);

		REQUIRE(timeBasedService.executeScheduledActivity(firstReleaseTime) == secondReleaseTime);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));

		auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == 1);
		REQUIRE(scheduledActivities.at(0)->requestID.sequenceCount == otherActivitySequenceCount);
	}
}

TEST_CASE("Execute every activity whose release time is reached", "[service][st11]") {
	Services.reset();
	constexpr std::chrono::seconds releaseDelay = 100s;
	constexpr std::chrono::seconds laterReleaseDelay = 200s;
	constexpr SequenceCount firstSequenceCount = 0;
	constexpr SequenceCount secondSequenceCount = 1;
	constexpr SequenceCount laterSequenceCount = 2;
	constexpr uint64_t numberOfDueActivities = 2;
	const Time::DefaultCUC now(TimeGetter::getCurrentTimeDefaultCUC());
	const Time::DefaultCUC releaseTime = now + releaseDelay;
	const Time::DefaultCUC laterReleaseTime = now + laterReleaseDelay;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	insertActivity(request, releaseTime, firstSequenceCount);
	insertActivity(request, releaseTime, secondSequenceCount);
	insertActivity(request, laterReleaseTime, laterSequenceCount);
	enableScheduleExecution();

	CHECK(timeBasedService.executeScheduledActivity(releaseTime) == laterReleaseTime);
	REQUIRE(ServiceTests::count() == numberOfDueActivities);
	CHECK(ServiceTests::get(0).messageType == TestService::MessageType::AreYouAliveTestReport);
	CHECK(ServiceTests::get(1).messageType == TestService::MessageType::AreYouAliveTestReport);

	const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
	REQUIRE(scheduledActivities.size() == singleActivity);
	CHECK(scheduledActivities.at(0)->requestID.sequenceCount == laterSequenceCount);
}

TEST_CASE("Delete due activities without executing them while the execution function is disabled", "[service][st11]") {
	Services.reset();
	constexpr std::chrono::seconds releaseDelay = 200s;
	constexpr std::chrono::seconds earlierReleaseDelay = 100s;
	constexpr SequenceCount sequenceCount = 0;
	constexpr SequenceCount disableSequenceCount = 1;
	const Time::DefaultCUC now(TimeGetter::getCurrentTimeDefaultCUC());
	const Time::DefaultCUC releaseTime = now + releaseDelay;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	const Message disableRequest(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DisableTimeBasedScheduleExecutionFunction, Message::TC, ApplicationId);

	SECTION("Disabled since the service started") {
		insertActivity(request, releaseTime, sequenceCount);
	}

	SECTION("Disabled by a TC[11,2]") {
		enableScheduleExecution();
		timeBasedService.disableScheduleExecution(disableRequest);
		insertActivity(request, releaseTime, sequenceCount);
	}

	SECTION("Disabled by a TC[11,2] released before the activity") {
		enableScheduleExecution();
		insertActivity(disableRequest, now + earlierReleaseDelay, disableSequenceCount);
		insertActivity(request, releaseTime, sequenceCount);
	}

	CHECK(timeBasedService.executeScheduledActivity(releaseTime) == Time::DefaultCUC::max());
	CHECK(unit_test::Tester::scheduledActivities(timeBasedService).empty());
	CHECK(not unit_test::Tester::executionFunctionStatus(timeBasedService));
	CHECK(ServiceTests::count() == 0);
	CHECK(ServiceTests::hasNoErrors());
}

static std::optional<uint8_t> scheduledFunctionArgument;

static void scheduledFunction(String<ECSSFunctionMaxArgLength> argument) {
	scheduledFunctionArgument = argument[0];
}

TEST_CASE("Execute a scheduled TC[8,1]", "[service][st11]") {
	Services.reset();
	constexpr std::chrono::seconds releaseDelay = 100s;
	constexpr SequenceCount sequenceCount = 0;
	constexpr uint8_t argument = 199;
	const String<ECSSFunctionNameLength> functionName("sched");
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + releaseDelay;
	scheduledFunctionArgument.reset();
	Services.functionManagement.include(functionName, &scheduledFunction);

	Message request(FunctionManagementService::ServiceType, FunctionManagementService::MessageType::PerformFunction, Message::TC, ApplicationId);
	request.appendFixedString(functionName);
	request.appendUint8(argument);
	insertActivity(request, releaseTime, sequenceCount);
	enableScheduleExecution();

	CHECK(timeBasedService.executeScheduledActivity(releaseTime) == Time::DefaultCUC::max());
	CHECK(ServiceTests::hasNoErrors());
	CHECK(scheduledFunctionArgument == argument);
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
		Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
		receivedMessage.appendUint16(1); // Total number of requests

		receivedMessage.appendDefaultCUCTimeStamp(currentTime - 155643s);
		receivedMessage.appendString(MessageParser::compose(testMessage1));
		MessageParser::execute(receivedMessage); //timeService.insertActivities(receivedMessage);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}
}

TEST_CASE("TC[11,4] Activity insertion with a request that fails to parse", "[service][st11]") {
	Services.reset();
	constexpr char unsupportedPUSVersion = 0;
	const Time::DefaultCUC now(TimeGetter::getCurrentTimeDefaultCUC());
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	String<CCSDSMaxMessageSize> unparsableRequest = MessageParser::compose(request);
	unparsableRequest[CCSDSPrimaryHeaderSize] = unsupportedPUSVersion;
	if constexpr (ECSSCRCIncluded) {
		unparsableRequest.resize(unparsableRequest.size() - CRCHelper::CRCField);
		const uint16_t crc = CRCHelper::calculateCRC(reinterpret_cast<const uint8_t*>(unparsableRequest.data()), unparsableRequest.size());
		unparsableRequest.push_back(static_cast<char>(crc >> std::numeric_limits<uint8_t>::digits));
		unparsableRequest.push_back(static_cast<char>(crc));
	}

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
	receivedMessage.appendUint16(2);
	receivedMessage.appendDefaultCUCTimeStamp(now + 100s);
	receivedMessage.appendString(unparsableRequest);
	receivedMessage.appendDefaultCUCTimeStamp(now + 200s);
	receivedMessage.appendString(MessageParser::compose(request));

	MessageParser::execute(receivedMessage);
	CHECK(ServiceTests::countThrownErrors(ErrorHandler::InstructionExecutionStartError) == 1);

	const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
	REQUIRE(scheduledActivities.size() == 1);
	CHECK(scheduledActivities.at(0)->requestReleaseTime == now + 200s);
	CHECK(Message::isSameType(request, scheduledActivities.at(0)->request));
}

TEST_CASE("TC[11,4] Activity insertion addressed to another application process", "[service][st11]") {
	Services.reset();
	constexpr ApplicationProcessId otherApplicationId = ApplicationId + 1;
	constexpr uint16_t numberOfInstructions = 2;
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + 100s;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, otherApplicationId);
	receivedMessage.appendUint16(numberOfInstructions);
	for (uint16_t i = 0; i < numberOfInstructions; i++) {
		appendInstruction(receivedMessage, releaseTime, request);
	}

	MessageParser::execute(receivedMessage);
	CHECK(ServiceTests::countErrors() == numberOfInstructions);
	CHECK(ServiceTests::countThrownErrors(ErrorHandler::InstructionExecutionStartError) == numberOfInstructions);
	CHECK(unit_test::Tester::scheduledActivities(timeBasedService).empty());
}

TEST_CASE("TC[11,4] Activity insertion identifies each activity by its request", "[service][st11]") {
	Services.reset();
	constexpr ApplicationProcessUserId requestSourceId = 7;
	constexpr SequenceCount requestSequenceCount = 42;
	constexpr ApplicationProcessUserId insertSourceId = requestSourceId + 1;
	constexpr SequenceCount insertSequenceCount = requestSequenceCount + 1;
	constexpr std::chrono::seconds releaseDelay = 100s;
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + releaseDelay;

	Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId, requestSourceId);
	request.packetSequenceCount = requestSequenceCount;

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId, insertSourceId);
	receivedMessage.packetSequenceCount = insertSequenceCount;
	receivedMessage.appendUint16(singleInstruction);
	appendInstruction(receivedMessage, releaseTime, request);

	MessageParser::execute(receivedMessage);
	CHECK(ServiceTests::hasNoErrors());

	const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
	REQUIRE(scheduledActivities.size() == singleActivity);
	CHECK(scheduledActivities.at(0)->requestID.sourceID == requestSourceId);
	CHECK(scheduledActivities.at(0)->requestID.applicationID == ApplicationId);
	CHECK(scheduledActivities.at(0)->requestID.sequenceCount == requestSequenceCount);
	CHECK(scheduledActivities.at(0)->request == request);
}

TEST_CASE("TC[11,4] Activity insertion with a request that cannot be executed", "[service][st11]") {
	Services.reset();
	constexpr ApplicationProcessId otherApplicationId = ApplicationId + 1;
	constexpr ServiceTypeNum unknownServiceType = std::numeric_limits<ServiceTypeNum>::max();
	constexpr uint16_t numberOfInstructions = 2;
	constexpr std::chrono::seconds releaseDelay = 100s;
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + releaseDelay;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	Message unexecutableRequest;

	SECTION("Addressed to another application process") {
		unexecutableRequest = Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, otherApplicationId);
	}

	SECTION("Unknown service type") {
		unexecutableRequest = Message(unknownServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	}

	SECTION("Report message type of a known service") {
		unexecutableRequest = Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTestReport, Message::TC, ApplicationId);
	}

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
	receivedMessage.appendUint16(numberOfInstructions);
	appendInstruction(receivedMessage, releaseTime, unexecutableRequest);
	appendInstruction(receivedMessage, releaseTime, request);

	MessageParser::execute(receivedMessage);
	CHECK(ServiceTests::countErrors() == singleError);
	CHECK(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));

	const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
	REQUIRE(scheduledActivities.size() == singleActivity);
	CHECK(scheduledActivities.at(0)->request == request);
}

TEST_CASE("TC[11,4] Activity insertion with a request larger than an activity can hold", "[service][st11]") {
	Services.reset();
	constexpr std::chrono::seconds releaseDelay = 100s;
	constexpr uint8_t requestByte = 0xAB;
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + releaseDelay;
	Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	const uint16_t largestDataSize = TimeBasedSchedulingService::MaxRequestSize - MessageParser::compose(request).size();
	while (request.dataSize < largestDataSize) {
		request.appendUint8(requestByte);
	}

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);
	receivedMessage.appendUint16(singleInstruction);

	SECTION("The largest request") {
		appendInstruction(receivedMessage, releaseTime, request);
		MessageParser::execute(receivedMessage);

		CHECK(ServiceTests::hasNoErrors());
		const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == singleActivity);
		CHECK(scheduledActivities.at(0)->request == request);
	}

	SECTION("One byte larger than the largest request") {
		request.appendUint8(requestByte);
		appendInstruction(receivedMessage, releaseTime, request);
		MessageParser::execute(receivedMessage);

		CHECK(ServiceTests::countErrors() == singleError);
		CHECK(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
		CHECK(unit_test::Tester::scheduledActivities(timeBasedService).empty());
	}
}

TEST_CASE("TC[11,4] Activity insertion with more instructions than its data holds", "[service][st11]") {
	Services.reset();
	constexpr std::chrono::seconds releaseDelay = 100s;
	const Time::DefaultCUC releaseTime = TimeGetter::getCurrentTimeDefaultCUC() + releaseDelay;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, ApplicationId);

	SECTION("The count is larger than the number of instructions") {
		constexpr uint16_t numberOfInstructions = std::numeric_limits<uint16_t>::max();
		receivedMessage.appendUint16(numberOfInstructions);
		appendInstruction(receivedMessage, releaseTime, request);
	}

	SECTION("The request of the last instruction runs past the end of the data") {
		constexpr uint16_t numberOfInstructions = 2;
		String<CCSDSMaxMessageSize> truncatedRequest = MessageParser::compose(request);
		truncatedRequest.pop_back();

		receivedMessage.appendUint16(numberOfInstructions);
		appendInstruction(receivedMessage, releaseTime, request);
		receivedMessage.appendDefaultCUCTimeStamp(releaseTime);
		receivedMessage.appendString(truncatedRequest);
	}

	MessageParser::execute(receivedMessage);
	CHECK(ServiceTests::countErrors() == singleError);
	CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooShort));

	const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
	REQUIRE(scheduledActivities.size() == singleActivity);
	CHECK(scheduledActivities.at(0)->request == request);
}

TEST_CASE("TC[11,15] Time shift all scheduled activities", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftALlScheduledActivities, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	constexpr Time::RelativeTime timeShift = 6789;

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

TEST_CASE("TC[11,15] Time shift an empty schedule", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftALlScheduledActivities, Message::TC, 1);
	receivedMessage.appendRelativeTime(GENERATE(-6789, 0, 6789));

	MessageParser::execute(receivedMessage);

	CHECK(ServiceTests::hasNoErrors());
	CHECK(unit_test::Tester::scheduledActivities(timeBasedService).empty());
}

TEST_CASE("TC[11,7] Time shift activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);
	CHECK(scheduledActivities.size() == 4);

	constexpr Time::RelativeTime timeShift = 67890000; // Relative time-shift value

	SECTION("Positive Shift") {
		receivedMessage.appendRelativeTime(timeShift);                            // Time-shift value
		receivedMessage.appendUint16(1);                                          // Just one instruction to time-shift an activity
		appendRequestID(receivedMessage, testMessage2);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Make sure the new value is inserted sorted
		REQUIRE(scheduledActivities.at(3)->requestReleaseTime == currentTime + 195723s + std::chrono::seconds(timeShift));
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(3)->request));
	}

	SECTION("Negative Shift") {
		receivedMessage.appendRelativeTime(-25000);                               // Time-shift value
		receivedMessage.appendUint16(1);                                          // Just one instruction to time-shift an activity
		appendRequestID(receivedMessage, testMessage2);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		// Output should be sorted
		REQUIRE(scheduledActivities.at(1)->requestReleaseTime == currentTime + 195723s - 25000s);
		REQUIRE(testMessage2.bytesEqualWith(scheduledActivities.at(1)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendRelativeTime(-250000);            // Time-shift value
		receivedMessage.appendUint16(1);                        // Just one instruction to time-shift an activity
		receivedMessage.append<ApplicationProcessUserId>(0);    // Dummy source ID
		receivedMessage.append<ApplicationProcessId>(80);       // Dummy application ID to throw an error
		receivedMessage.append<SequenceCount>(0);               // Dummy sequence count

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}

	SECTION("Error throw on wrong time offset") {
		receivedMessage.appendRelativeTime(-6789000);                             // Time-shift value
		receivedMessage.appendUint16(1);                                          // Just one instruction to time-shift an activity
		appendRequestID(receivedMessage, testMessage2);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::SubServiceExecutionStartError));
	}
}

TEST_CASE("TC[11,7] Time shift activities by ID against the earliest activity", "[service][st11]") {
	Services.reset();
	const Time::DefaultCUC now(TimeGetter::getCurrentTimeDefaultCUC());
	const Time::DefaultCUC earlierReleaseTime = now + 100s;
	const Time::DefaultCUC laterReleaseTime = now + 1000s;
	const SequenceCount earlierSequenceCount = 0;
	const SequenceCount laterSequenceCount = 1;
	const Message request(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, ApplicationId);
	insertActivity(request, earlierReleaseTime, earlierSequenceCount);
	insertActivity(request, laterReleaseTime, laterSequenceCount);

	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftActivitiesById, Message::TC, 1);
	receivedMessage.appendRelativeTime(-500);
	auto appendRequestID = [&receivedMessage](ApplicationProcessId applicationID, SequenceCount sequenceCount) {
		receivedMessage.append<ApplicationProcessUserId>(0);
		receivedMessage.append<ApplicationProcessId>(applicationID);
		receivedMessage.append<SequenceCount>(sequenceCount);
	};

	SECTION("The earliest activity would be released too early") {
		receivedMessage.appendUint16(2);
		appendRequestID(ApplicationId, earlierSequenceCount);
		appendRequestID(ApplicationId, laterSequenceCount);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::SubServiceExecutionStartError));

		const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == 2);
		CHECK(scheduledActivities.at(0)->requestReleaseTime == earlierReleaseTime);
		CHECK(scheduledActivities.at(1)->requestReleaseTime == laterReleaseTime);
	}

	SECTION("Unknown request ID next to a valid instruction") {
		const ApplicationProcessId unknownApplicationID = 80;
		receivedMessage.appendUint16(2);
		appendRequestID(unknownApplicationID, 0);
		appendRequestID(ApplicationId, laterSequenceCount);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));

		const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == 2);
		CHECK(scheduledActivities.at(0)->requestReleaseTime == earlierReleaseTime);
		CHECK(scheduledActivities.at(1)->requestReleaseTime == laterReleaseTime - 500s);
	}

	SECTION("A later activity would wrap the release time counter") {
		const SequenceCount lastSequenceCount = 2;
		const Time::DefaultCUC lastReleaseTime = Time::DefaultCUC::max() - 1000s;
		insertActivity(request, lastReleaseTime, lastSequenceCount);

		Message wrappingShift(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftActivitiesById, Message::TC, 1);
		wrappingShift.appendRelativeTime(2000);
		wrappingShift.appendUint16(2);
		wrappingShift.append<ApplicationProcessUserId>(0);
		wrappingShift.append<ApplicationProcessId>(ApplicationId);
		wrappingShift.append<SequenceCount>(earlierSequenceCount);
		wrappingShift.append<ApplicationProcessUserId>(0);
		wrappingShift.append<ApplicationProcessId>(ApplicationId);
		wrappingShift.append<SequenceCount>(lastSequenceCount);

		timeBasedService.timeShiftActivitiesByID(wrappingShift);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::SubServiceExecutionStartError));

		const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == 3);
		CHECK(scheduledActivities.at(0)->requestReleaseTime == earlierReleaseTime);
		CHECK(scheduledActivities.at(2)->requestReleaseTime == lastReleaseTime);
	}

	SECTION("Repeated request ID") {
		receivedMessage.appendUint16(2);
		appendRequestID(ApplicationId, laterSequenceCount);
		appendRequestID(ApplicationId, laterSequenceCount);

		timeBasedService.timeShiftActivitiesByID(receivedMessage);
		CHECK(ServiceTests::hasNoErrors());

		const auto scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);
		REQUIRE(scheduledActivities.size() == 2);
		CHECK(scheduledActivities.at(0)->requestReleaseTime == earlierReleaseTime);
		CHECK(scheduledActivities.at(1)->requestReleaseTime == laterReleaseTime - 500s);
	}
}

TEST_CASE("TC[11,9] Detail report scheduled activities by ID", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Detailed activity report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);

		receivedMessage.appendUint16(2);                                          // Two instructions in the request
		appendRequestID(receivedMessage, testMessage2);
		appendRequestID(receivedMessage, testMessage1);

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);

		uint16_t iterationCount = response.readUint16();
		CHECK(iterationCount == 2);
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime(response.readDefaultCUCTimeStamp());

			const Message receivedTCPacket = readRequest(response);
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
		receivedMessage.appendUint16(1);                        // Just one instruction to time-shift an activity
		receivedMessage.append<ApplicationProcessUserId>(0);    // Dummy source ID
		receivedMessage.append<ApplicationProcessId>(80);       // Dummy application ID to throw an error
		receivedMessage.append<SequenceCount>(0);               // Dummy sequence count

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}

	SECTION("Repeated request ID, more times than a report can hold") {
		constexpr uint16_t numberOfInstructions = ECSSMaxNumberOfTimeSchedActivities + 1;
		receivedMessage.appendUint16(numberOfInstructions);
		for (uint16_t i = 0; i < numberOfInstructions; i++) {
			appendRequestID(receivedMessage, testMessage1);
		}

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::TimeBasedScheduleReportIsFull) == 1);

		Message response = ServiceTests::get(ServiceTests::count() - 1);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);
		CHECK(response.readUint16() == ECSSMaxNumberOfTimeSchedActivities);
	}
}

TEST_CASE("TM[11,10] time-based schedule detail report", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportActivitiesById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Detailed activity report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);

		receivedMessage.appendUint16(2);                                          // Two instructions in the request
		appendRequestID(receivedMessage, testMessage2);
		appendRequestID(receivedMessage, testMessage1);

		timeBasedService.detailReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);

		uint16_t iterationCount = response.readUint16();
		CHECK(iterationCount == 2);
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime(response.readDefaultCUCTimeStamp());

			const Message receivedTCPacket = readRequest(response);
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

		receivedMessage.appendUint16(2);                                          // Two instructions in the request
		appendRequestID(receivedMessage, testMessage2);
		appendRequestID(receivedMessage, testMessage1);

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport);

		uint16_t iterationCount = response.readUint16();
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime(response.readDefaultCUCTimeStamp());
			ApplicationProcessUserId receivedSourceID = response.read<ApplicationProcessUserId>();
			ApplicationProcessId receivedApplicationID = response.read<ApplicationProcessId>();
			SequenceCount receivedSequenceCount = response.read<ParameterSampleCount>();

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
		receivedMessage.appendUint16(1);                        // Just one instruction to time-shift an activity
		receivedMessage.append<ApplicationProcessUserId>(0);    // Dummy source ID
		receivedMessage.append<ApplicationProcessId>(80);       // Dummy application ID to throw an error
		receivedMessage.append<SequenceCount>(0);               // Dummy sequence count

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::InstructionExecutionStartError));
	}

	SECTION("Repeated request ID, more times than a report can hold") {
		constexpr uint16_t numberOfInstructions = ECSSMaxNumberOfTimeSchedActivities + 1;
		receivedMessage.appendUint16(numberOfInstructions);
		for (uint16_t i = 0; i < numberOfInstructions; i++) {
			appendRequestID(receivedMessage, testMessage1);
		}

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::TimeBasedScheduleReportIsFull) == 1);

		Message response = ServiceTests::get(ServiceTests::count() - 1);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport);
		CHECK(response.readUint16() == ECSSMaxNumberOfTimeSchedActivities);
	}
}

TEST_CASE("TM[11,13] time-based schedule summary report", "[service][st11]") {
	Services.reset();
	Message receivedMessage(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ActivitiesSummaryReportById, Message::TC, 1);

	auto scheduledActivities = activityInsertion(timeBasedService);

	SECTION("Summary report") {
		// Verify that everything is in place
		CHECK(scheduledActivities.size() == 4);

		receivedMessage.appendUint16(2);                                          // Two instructions in the request
		appendRequestID(receivedMessage, testMessage2);
		appendRequestID(receivedMessage, testMessage1);

		timeBasedService.summaryReportActivitiesByID(receivedMessage);
		REQUIRE(ServiceTests::hasOneMessage());

		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 11);
		CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport);

		uint16_t iterationCount = response.readUint16();
		for (uint16_t i = 0; i < iterationCount; i++) {
			Time::DefaultCUC receivedReleaseTime(response.readDefaultCUCTimeStamp());
			ApplicationProcessUserId receivedSourceID = response.read<ApplicationProcessUserId>();
			ApplicationProcessId receivedApplicationID = response.read<ApplicationProcessId>();
			SequenceCount receivedSequenceCount = response.read<ParameterSampleCount>();

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
	CHECK(response.messageType == TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);

	uint16_t iterationCount = response.readUint16();
	REQUIRE(iterationCount == scheduledActivities.size());

	for (uint16_t i = 0; i < iterationCount; i++) {
		Time::DefaultCUC receivedReleaseTime(response.readDefaultCUCTimeStamp());

		const Message receivedTCPacket = readRequest(response);
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

		receivedMessage.appendUint16(1);                                          // Just one instruction to delete an activity
		appendRequestID(receivedMessage, testMessage2);

		CHECK(scheduledActivities.size() == 4);
		timeBasedService.deleteActivitiesByID(receivedMessage);
		scheduledActivities = unit_test::Tester::scheduledActivities(timeBasedService);

		REQUIRE(scheduledActivities.size() == 3);
		REQUIRE(scheduledActivities.at(2)->requestReleaseTime == currentTime + 1724843s);
		REQUIRE(testMessage4.bytesEqualWith(scheduledActivities.at(2)->request));
	}

	SECTION("Error throw on wrong request ID") {
		receivedMessage.appendUint16(1);                        // Just one instruction to time-shift an activity
		receivedMessage.append<ApplicationProcessUserId>(0);    // Dummy source ID
		receivedMessage.append<ApplicationProcessId>(80);       // Dummy application ID to throw an error
		receivedMessage.append<SequenceCount>(0);               // Dummy sequence count

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
