#include <ECSSMessage.hpp>
#include <Services/TestService.hpp>
#include <catch2/catch_all.hpp>
#include "ServiceTests.hpp"

TestService& testService = Services.testService;

TEST_CASE("TC[17,1]", "[service][st17]") {
	ECSSMessage receivedPacket = ECSSMessage(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, ECSSMessage::TC, 1);
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	ECSSMessage response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::AreYouAliveTestReport);
	REQUIRE(response.dataSize == 0);
}

TEST_CASE("TM[17,2]", "[service][st17]") {
	testService.areYouAliveReport();

	ECSSMessage response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::AreYouAliveTestReport);
	REQUIRE(response.dataSize == 0);
}

TEST_CASE("TC[17,3]", "[service][st17]") {
	ECSSMessage receivedPacket = ECSSMessage(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest, ECSSMessage::TC, 1);
	receivedPacket.appendEnum16(ApplicationId);
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	ECSSMessage response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::OnBoardConnectionTestReport);
	REQUIRE(response.dataSize == 2);
	CHECK(response.readEnum16() == ApplicationId);
}

TEST_CASE("TM[17,4]", "[service][st17]") {
	testService.onBoardConnectionReport(40);

	ECSSMessage response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::OnBoardConnectionTestReport);
	REQUIRE(response.dataSize == 2);
	CHECK(response.readEnum16() == 40);
}
