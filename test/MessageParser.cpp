#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include <cstring>
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"
#include "ServicePool.hpp"

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

TEST_CASE("TC data parsing into a message", "[MessageParser]") {

}

TEST_CASE("TM message parsing", "[MessageParser]") {
	MessageParser messageParser;
	uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0c, 0x20, 0x16, 0x11, 0x00, 0x00, 0x68,
		0x65, 0x6c, 0x6c, 0x6f,
		0x68, 0x69};
	Message message = messageParser.parse(packet, 18);
	CHECK(message.packetType == Message::TM);
	CHECK(message.applicationId == 2);
	CHECK(message.dataSize == 7);
	CHECK(message.serviceType == 22);
	CHECK(message.messageType == 17);
	CHECK(memcmp(message.data, "hellohi", 7) == 0);
}
