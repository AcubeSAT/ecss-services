#include <catch2/catch.hpp>
#include <Message.hpp>
#include <cstring>
#include "Helpers/CRCHelper.hpp"
#include "MessageParser.hpp"

TEST_CASE("TC message parsing", "[MessageParser]") {
	uint8_t packet[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x08, 0x20, 0x81, 0x1f, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

	Message message = MessageParser::parse(packet, 14);
	CHECK(message.packetType == Message::TC);
	CHECK(message.applicationId == 7);
	CHECK(message.packetSequenceCount == 8199);
	CHECK(message.dataSize == 5);
	CHECK(message.serviceType == 129);
	CHECK(message.messageType == 31);
	CHECK(memcmp(message.data, "hello", 5) == 0);
}

TEST_CASE("TC Message parsing into a string", "[MessageParser]") {
	uint8_t wantedPacket[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x08, 0x00, 0x81, 0x1f, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

	Message message;
	message.packetType = Message::TC;
	message.applicationId = 7;
	message.serviceType = 129;
	message.messageType = 31;
	message.packetSequenceCount = 8199;
	message.responseBand = static_cast<Message::ResponseBand>(0);
	message.acknowledgementFlags = 0;
	String<5> sourceString = "hello";
	std::copy(sourceString.data(), sourceString.data() + sourceString.size(), message.data);
	message.dataSize = 5;

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

	if (ECSSCRCIncluded) {
		CHECK(createdPacket.size() == 16);

		CHECK(memcmp(createdPacket.data(), wantedPacket, 14) == 0);

		const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
		uint8_t crc_verification = CRCHelper::validateCRC(packet, 16);
		CHECK(crc_verification == 0);
	} else {
		CHECK(createdPacket.size() == 14);
		CHECK(memcmp(createdPacket.data(), wantedPacket, 14) == 0);
	}
}

TEST_CASE("TM message parsing", "[MessageParser]") {
	uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0e, 0x16, 0x11, 0x15,
		0x00, 0x00, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

	Message message = MessageParser::parse(packet, 20);
	CHECK(message.packetType == Message::TM);
	CHECK(message.applicationId == 2);
	CHECK(message.packetSequenceCount == 77);
	CHECK(message.dataSize == 7);
	CHECK(message.messageTypeCounter == 21);
	CHECK(message.serviceType == 22);
	CHECK(message.messageType == 17);
	CHECK(memcmp(message.data, "hellohi", 7) == 0);
}

TEST_CASE("TM Message parsing into a string", "[MessageParser]") {
	uint8_t wantedPacket[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0e, 0x16, 0x11,
		0x02, 0x00, 0x00, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

	Message message;
	message.packetType = Message::TM;
	message.applicationId = 2;
	message.packetSequenceCount = 77;
	message.serviceType = 22;
	message.messageType = 17;
	String<7> sourceString = "hellohi";
	std::copy(sourceString.data(), sourceString.data() + sourceString.size(), message.data);
	message.dataSize = 7;
	message.messageTypeCounter = 2;
	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

	if(ECSSCRCIncluded) {
		CHECK(createdPacket.size() == 22);
		CHECK(memcmp(createdPacket.data(), wantedPacket, 20) == 0);

		const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
		uint8_t crc_verification = CRCHelper::validateCRC(packet, 22);
		CHECK(crc_verification == 0);
	}
	else {
		CHECK(createdPacket.size() == 18);
		CHECK((createdPacket == String<18>(wantedPacket)));
	}
}
