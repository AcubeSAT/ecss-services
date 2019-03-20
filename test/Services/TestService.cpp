#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TestService & testService = Services.testService;

TEST_CASE("TM[17,1]", "[service][st17]") {
	Message receivedPacket = Message(17, 1, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 17);
	CHECK(response.messageType == 2);
	REQUIRE(response.dataSize == 0);
}

TEST_CASE("TM[17,3]", "[service][st17]") {
	Message receivedPacket = Message(17, 3, Message::TC, 1);
	receivedPacket.appendEnum16(40);
	testService.onBoardConnection(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 17);
	CHECK(response.messageType == 4);
	REQUIRE(response.dataSize == 2);
	CHECK(response.readEnum16() == 40);
}
