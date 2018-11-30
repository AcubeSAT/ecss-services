#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"

TEST_CASE("ST[01] message parsing", "[MessageParser][st01]") {
	MessageParser messageParser;

	Message message = Message(1, 1, Message::TC, 2);
	messageParser.execute(message);
	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 1);
	CHECK(response.packetType == Message::TM);

	message = Message(1, 2, Message::TC, 2);
	messageParser.execute(message);
	response = ServiceTests::get(1);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 2);
	CHECK(response.packetType == Message::TM);

	message = Message(1, 7, Message::TC, 2);
	messageParser.execute(message);
	response = ServiceTests::get(2);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 7);
	CHECK(response.packetType == Message::TM);

	message = Message(1, 8, Message::TC, 2);
	messageParser.execute(message);
	response = ServiceTests::get(3);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 8);
	CHECK(response.packetType == Message::TM);

	message = Message(1, 10, Message::TC, 2);
	messageParser.execute(message);
	response = ServiceTests::get(4);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 10);
	CHECK(response.packetType == Message::TM);
}

TEST_CASE("ST[17] message parsing", "[MessageParser][st17]") {
	MessageParser messageParser;

	Message message = Message(17, 1, Message::TC, 1);
	messageParser.execute(message);
	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 17);
	CHECK(response.messageType == 2);
	CHECK(response.packetType == Message::TM);

	message = Message(17, 3, Message::TC, 1);
	message.appendUint16(7);
	messageParser.execute(message);
	response = ServiceTests::get(1);
	CHECK(response.serviceType == 17);
	CHECK(response.messageType == 4);
	CHECK(response.packetType == Message::TM);
}
