#include "MessageParser.hpp"
#include <Message.hpp>
#include <catch2/catch_all.hpp>
#include <cstring>
#include "Helpers/CRCHelper.hpp"
#include "Helpers/TimeGetter.hpp"

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
	uint8_t wantedPacket[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x09, 0x20, 0x81,
	                          0x1f, 0x00, 0x07, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

	Message message;
	message.packetType = Message::TC;
	message.applicationId = 7;
	message.serviceType = 129;
	message.messageType = 31;
	message.packetSequenceCount = 8199;
	String<5> sourceString = "hello";
	std::copy(sourceString.data(), sourceString.data() + sourceString.size(), message.data);
	message.dataSize = 5;

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
#if ECSS_CRC_INCLUDED
	CHECK(createdPacket.size() == 18);
	CHECK(memcmp(createdPacket.data(), wantedPacket, 16) == 0);

	const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
	uint8_t crc_verification = CRCHelper::validateCRC(packet, 18);
	CHECK(crc_verification == 0);
#else
	CHECK(createdPacket.size() == 16);
	CHECK((createdPacket == String<16>(wantedPacket)));
#endif

}

TEST_CASE("TM message parsing", "[MessageParser]") {
	uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x12, 0x20, 0x16,
	                    0x11,0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00,
	                    0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};
	uint32_t time = TimeGetter::getCurrentTimeDefaultCUC().formatAsBytes();
	packet[13] = (time >> 24) & 0xFF;
	packet[14] = (time >> 16) & 0xFF;
	packet[15] = (time >> 8) & 0xFF;
	packet[16] = (time) & 0xFF;

	Message message = MessageParser::parse(packet, 24);
	CHECK(message.packetType == Message::TM);
	CHECK(message.applicationId == 2);
	CHECK(message.packetSequenceCount == 77);
	CHECK(message.dataSize == 7);
	CHECK(message.serviceType == 22);
	CHECK(message.messageType == 17);
	CHECK(memcmp(message.data, "hellohi", 7) == 0);

	// Add ECSS and CCSDS header
	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
	uint32_t messageTime =  (createdPacket[16] & 0xFF) | ((createdPacket[15] & 0xFF ) << 8) | ((createdPacket[14] & 0xFF) << 16) | ((createdPacket[13] & 0xFF ) << 24);
	CHECK(messageTime == time);

}

TEST_CASE("TM Message parsing into a string", "[MessageParser]") {
	uint8_t wantedPacket[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x11, 0x20, 0x16,
	                          0x11,0x00, 0x00,0x00, 0x02,0x00, 0x00,0x00,
	                          0x00, 0x68,0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};
	uint32_t time = TimeGetter::getCurrentTimeDefaultCUC().formatAsBytes();
	wantedPacket[13] = (time >> 24) & 0xFF;
	wantedPacket[14] = (time >> 16) & 0xFF;
	wantedPacket[15] = (time >> 8) & 0xFF;
	wantedPacket[16] = (time) & 0xFF;

	Message message;
	message.packetType = Message::TM;
	message.applicationId = 2;
	message.packetSequenceCount = 77;
	message.serviceType = 22;
	message.messageType = 17;
	String<7> sourceString = "hellohi";
	std::copy(sourceString.data(), sourceString.data() + sourceString.size(), message.data);
	message.dataSize = 7;
	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

#if ECSS_CRC_INCLUDED
	CHECK(createdPacket.size() == 26);
	CHECK(memcmp(createdPacket.data(), wantedPacket, 24) == 0);

	const uint8_t* packet = reinterpret_cast<uint8_t*>(&createdPacket.data()[0]);
	uint8_t crc_verification = CRCHelper::validateCRC(packet, 26);
	CHECK(crc_verification == 0);
#else
	CHECK(createdPacket.size() == 24);
	CHECK((createdPacket == String<24>(wantedPacket)));
#endif
}
