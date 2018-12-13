#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"

TEST_CASE("ST[01] message execution", "[MessageParser][st01]") {
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

TEST_CASE("ST[17] message execution", "[MessageParser][st17]") {
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

TEST_CASE("TC message parsing", "[MessageParser]") {
	MessageParser messageParser;

	uint8_t packet[] = {0x18, 0x07, 0xc0, 0x4d, 0x00, 0x0a, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68,
	                    0x65, 0x6c, 0x6c, 0x6f};

	Message message = messageParser.parse(packet, 16);
	CHECK(message.packetType == Message::TC);
	CHECK(message.applicationId == 7);
	CHECK(message.dataSize == 5);
	CHECK(message.serviceType == 129);
	CHECK(message.messageType == 31);
	CHECK(memcmp(message.data, "hello", 5) == 0);
}
