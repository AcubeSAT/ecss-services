#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include <cstring>
#include "Helpers/CRCHelper.hpp"
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"
#include "ServicePool.hpp"


TEST_CASE("TC message parsing", "[MessageParser]") {
	uint8_t packet[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x0a, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

	Message message = MessageParser::parse(packet, 16);
	CHECK(message.packetType == Message::TC);
	CHECK(message.applicationId == 7);
	CHECK(message.packetSequenceCount == 8199);
	CHECK(message.dataSize == 5);
	CHECK(message.serviceType == 129);
	CHECK(message.messageType == 31);
	CHECK(memcmp(message.data, "hello", 5) == 0);
}

TEST_CASE("TC Message parsing into a string", "[MessageParser]") {
	uint8_t wantedPacket[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x0a, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c,
		0x6f};

	Message message;
	message.packetType = Message::TC;
	message.applicationId = 7;
	message.serviceType = 129;
	message.messageType = 31;
	message.packetSequenceCount = 8199;
	memcpy(message.data, "hello", 5);
	message.dataSize = 5;

	String<CCSDS_MAX_MESSAGE_SIZE> createdPacket = MessageParser::compose(message);
#if ECSS_CRC_INCLUDED
	CHECK(createdPacket.size() == 18);
	CHECK(memcmp(createdPacket.data(), wantedPacket, 16) == 0);

	createdPacket.data();
	const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
	uint8_t crc_verification = CRCHelper::validateCRC(packet, 18);
	CHECK(crc_verification == 0);
#else
	CHECK(createdPacket.size() == 16);
	// The two parentheses are necessary so that Catch2 doesn't try to parse the strings here
	CHECK((createdPacket == String<16>(wantedPacket)));
#endif
}

TEST_CASE("TM message parsing", "[MessageParser]") {
	uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0c, 0x20, 0x16, 0x11,
		0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

	Message message = MessageParser::parse(packet, 18);
	CHECK(message.packetType == Message::TM);
	CHECK(message.applicationId == 2);
	CHECK(message.packetSequenceCount == 77);
	CHECK(message.dataSize == 7);
	CHECK(message.serviceType == 22);
	CHECK(message.messageType == 17);
	CHECK(memcmp(message.data, "hellohi", 7) == 0);
}

TEST_CASE("TM Message parsing into a string", "[MessageParser]") {
	uint8_t wantedPacket[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0c, 0x20, 0x16, 0x11,
		0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

	Message message;
	message.packetType = Message::TM;
	message.applicationId = 2;
	message.packetSequenceCount = 77;
	message.serviceType = 22;
	message.messageType = 17;
	memcpy(message.data, "hellohi", 7);
	message.dataSize = 7;
	String<CCSDS_MAX_MESSAGE_SIZE> createdPacket = MessageParser::compose(message);

#if ECSS_CRC_INCLUDED
	CHECK(createdPacket.size() == 20);
	CHECK(memcmp(createdPacket.data(), wantedPacket, 18) == 0);

	const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
	uint8_t crc_verification = CRCHelper::validateCRC(packet, 20);
	CHECK(crc_verification == 0);
#else
	CHECK(createdPacket.size() == 18);
	// The two parentheses are necessary so that Catch2 doesn't try to parse the strings here
	CHECK((createdPacket == String<18>(wantedPacket)));
#endif
}
