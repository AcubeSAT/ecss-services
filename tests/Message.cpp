#include <catch2/catch.hpp>
#include <Message.hpp>

TEST_CASE("1: Message is usable", "[message]") {
	Message message(5, 17, Message::TC, 3);

	REQUIRE(message.serviceType == 5);
	REQUIRE(message.messageType == 17);
	REQUIRE(message.packetType == Message::TC);
	REQUIRE(message.applicationId == 3);
	REQUIRE(message.dataSize == 0);

	message.appendByte(250);
	REQUIRE(message.dataSize == 1);
	REQUIRE(message.readByte() == 250);
}

TEST_CASE("2: Bit manipulations", "[message]") {
	Message message(0, 0, Message::TC, 0);

	message.appendBits(10, 0x357);
	message.appendBits(4, 0xb);
	message.appendBits(2, 0);
	message.appendByte(248);
	message.appendBits(7, 0x16);
	message.appendBits(1, 0x1);
	message.appendBits(8, 0xff);

	REQUIRE(message.dataSize == 5);

	CHECK(message.readBits(10) == 0x357);
	CHECK(message.readBits(4) == 0xb);
	CHECK(message.readBits(2) == 0);
	CHECK(message.readBits(5) == 0x1f);
	CHECK(message.readBits(3) == 0);
	CHECK(message.readByte() == 0x2d);
	CHECK(message.readByte() == 0xff);

	message.resetRead();

	CHECK(message.readUint32() == 0xd5ecf82d);
	CHECK(message.readBits(8) == 0xff);
}
