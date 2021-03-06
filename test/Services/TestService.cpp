#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TestService& testService = Services.testService;

TEST_CASE("TM[17,1]", "[service][st17]") {
	Message receivedPacket = Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, 1);
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::AreYouAliveTestReport);
	REQUIRE(response.dataSize == 0);
}

TEST_CASE("TM[17,3]", "[service][st17]") {
	Message receivedPacket = Message(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest, Message::TC, 1);
	receivedPacket.appendEnum16(40);
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == TestService::ServiceType);
	CHECK(response.messageType == TestService::MessageType::OnBoardConnectionTestReport);
	REQUIRE(response.dataSize == 2);
	CHECK(response.readEnum16() == 40);
}
