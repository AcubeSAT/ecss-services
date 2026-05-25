#include <Message.hpp>
#include <catch2/catch_all.hpp>
#include <cstring>
#include "Helpers/CRCHelper.hpp"
#include "Helpers/TimeGetter.hpp"
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"

TEST_CASE("TC message parsing", "[MessageParser]") {
	const char expectedData[] = "hello";

	if constexpr (ECSSCRCIncluded) {
        uint8_t packet[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x0b, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x9b, 0xe8};
        Message message = MessageParser::parse(packet, sizeof(packet));

		CHECK(message.packetType == Message::TC);
		CHECK(message.applicationId == 7);
		CHECK(message.packetSequenceCount == 8199);
		CHECK(message.dataSize == 5);
		CHECK(message.serviceType == 129);
		CHECK(message.messageType == 31);
		CHECK(message.sourceId == 0);
		CHECK(memcmp(message.data.begin(), expectedData, message.dataSize) == 0);
    } else {
        uint8_t packet[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x09, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f};
        Message message = MessageParser::parse(packet, sizeof(packet));

		CHECK(message.packetType == Message::TC);
		CHECK(message.applicationId == 7);
		CHECK(message.packetSequenceCount == 8199);
		CHECK(message.dataSize == std::strlen(expectedData));
		CHECK(message.serviceType == 129);
		CHECK(message.messageType == 31);
		CHECK(message.sourceId == 0);
		CHECK(memcmp(message.data.begin(), expectedData, message.dataSize) == 0);
    }
}

TEST_CASE("TC Message parsing into a string", "[MessageParser]") {
	Message message;
	message.packetType = Message::TC;
	message.applicationId = 7;
	message.serviceType = 129;
	message.messageType = 31;
	message.packetSequenceCount = 8199;
	message.sourceId = 0;
	String<5> sourceString = "hello";
	message.appendString(sourceString);

	CHECK(message.dataSize == sourceString.size());

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
	if constexpr (ECSSCRCIncluded) {
		uint8_t wantedPacket[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x0b, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

		CHECK(createdPacket.size() == sizeof(wantedPacket) + CRCHelper::CRCField);
		CHECK(memcmp(createdPacket.data(), wantedPacket, sizeof(wantedPacket)) == 0);
		CHECK(CRCHelper::validateCRC(reinterpret_cast<uint8_t*>(createdPacket.data()), createdPacket.size()));
	} else {
		uint8_t wantedPacket[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x09, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f};

		CHECK(createdPacket.size() == sizeof(wantedPacket));
		CHECK(memcmp(createdPacket.data(), wantedPacket, sizeof(wantedPacket)) == 0);
	}
}

TEST_CASE("TM message parsing", "[MessageParser]") {
	const char expectedData[] = "hellohi";

	if constexpr (ECSSCRCIncluded) {
		uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x13, 0x20, 0x16,
	                    	0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                    	0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};
		
		Time::DefaultCUC time(TimeGetter::getCurrentTimeDefaultCUC());
		packet[13] = (time.formatAsBytes() >> 24) & 0xFF;
		packet[14] = (time.formatAsBytes() >> 16) & 0xFF;
		packet[15] = (time.formatAsBytes() >> 8) & 0xFF;
		packet[16] = (time.formatAsBytes()) & 0xFF;

		uint16_t crc = CRCHelper::calculateCRC(packet, sizeof(packet));
		uint8_t packetCRC[sizeof(packet) + CRCHelper::CRCField];
		std::memcpy(packetCRC, packet, sizeof(packet));
		packetCRC[24] = static_cast<uint8_t>(crc >> 8);
		packetCRC[25] = static_cast<uint8_t>(crc & 0xFF);
		Message message = MessageParser::parse(packetCRC, sizeof(packetCRC));

		CHECK(message.packetType == Message::TM);
		CHECK(message.applicationId == 2);
		CHECK(message.packetSequenceCount == 77);
		CHECK(message.dataSize == std::strlen(expectedData));
		CHECK(message.serviceType == 22);
		CHECK(message.messageType == 17);
		CHECK(message.destinationId == 0);
		CHECK(memcmp(message.data.begin(), expectedData, message.dataSize) == 0);

		// Add ECSS and CCSDS header
		String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
		Time::DefaultCUC messageTime((createdPacket[16] & 0xFF) | ((createdPacket[15] & 0xFF) << 8) | ((createdPacket[14] & 0xFF) << 16) | ((createdPacket[13] & 0xFF) << 24));
		CHECK(messageTime.asTAIseconds() == time.formatAsBytes());
	} else {
		uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x11, 0x20, 0x16,
	                    	0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                    	0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

		Time::DefaultCUC time(TimeGetter::getCurrentTimeDefaultCUC());
		packet[13] = (time.formatAsBytes() >> 24) & 0xFF;
		packet[14] = (time.formatAsBytes() >> 16) & 0xFF;
		packet[15] = (time.formatAsBytes() >> 8) & 0xFF;
		packet[16] = (time.formatAsBytes()) & 0xFF;

		Message message = MessageParser::parse(packet, sizeof(packet));

		CHECK(message.packetType == Message::TM);
		CHECK(message.applicationId == 2);
		CHECK(message.packetSequenceCount == 77);
		CHECK(message.dataSize == 7);
		CHECK(message.serviceType == 22);
		CHECK(message.messageType == 17);
		CHECK(message.destinationId == 0);
		CHECK(memcmp(message.data.begin(), expectedData, message.dataSize) == 0);

		// Add ECSS and CCSDS header
		String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
		Time::DefaultCUC messageTime((createdPacket[16] & 0xFF) | ((createdPacket[15] & 0xFF) << 8) | ((createdPacket[14] & 0xFF) << 16) | ((createdPacket[13] & 0xFF) << 24));
		CHECK(messageTime.asTAIseconds() == time.formatAsBytes());
	}
}

TEST_CASE("TM Message parsing into a string", "[MessageParser]") {
	Message message;
	message.packetType = Message::TM;
	message.applicationId = 2;
	message.packetSequenceCount = 77;
	message.serviceType = 22;
	message.messageType = 17;
	message.destinationId = 0;
	String<7> sourceString = "hellohi";
	message.appendString(sourceString);

	Time::DefaultCUC time(TimeGetter::getCurrentTimeDefaultCUC());

	CHECK(message.dataSize == sourceString.size());

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

	if constexpr (ECSSCRCIncluded) {
		uint8_t wantedPacket[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x13, 0x20, 0x16,
	                        	0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                        	0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

		wantedPacket[13] = (time.formatAsBytes() >> 24) & 0xFF;
		wantedPacket[14] = (time.formatAsBytes() >> 16) & 0xFF;
		wantedPacket[15] = (time.formatAsBytes() >> 8) & 0xFF;
		wantedPacket[16] = (time.formatAsBytes()) & 0xFF;

		CHECK(createdPacket.size() == sizeof(wantedPacket) + CRCHelper::CRCField);
		CHECK(memcmp(createdPacket.data(), wantedPacket, sizeof(wantedPacket)) == 0);
		CHECK(CRCHelper::validateCRC(reinterpret_cast<uint8_t*>(createdPacket.data()), createdPacket.size()));
	} else {
		uint8_t wantedPacket[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x11, 0x20, 0x16,
	                        	0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                        	0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x68, 0x69};

		wantedPacket[13] = (time.formatAsBytes() >> 24) & 0xFF;
		wantedPacket[14] = (time.formatAsBytes() >> 16) & 0xFF;
		wantedPacket[15] = (time.formatAsBytes() >> 8) & 0xFF;
		wantedPacket[16] = (time.formatAsBytes()) & 0xFF;

		CHECK(createdPacket.size() == sizeof(wantedPacket));
		CHECK(memcmp(createdPacket.data(), wantedPacket, sizeof(wantedPacket)) == 0);
	}
}

TEST_CASE("TM compose and parse consistency", "[MessageParser]") {
	Message message;
	message.packetType = Message::TM;
	message.applicationId = 15;
	message.packetSequenceCount = 8199;
	message.serviceType = 129;
	message.messageType = 31;
	message.messageTypeCounter = 42;
	message.destinationId = 0;
	
	String<10> sourceString = "helloworld";
	message.appendString(sourceString);

	CHECK(message.dataSize == sourceString.size());

	String<CCSDSMaxMessageSize> createdPacket1 = MessageParser::compose(message);
	Message parsedMessage1 = MessageParser::parse(reinterpret_cast<const uint8_t*>(createdPacket1.data()), createdPacket1.size());

	String<CCSDSMaxMessageSize> createdPacket2 = MessageParser::compose(parsedMessage1);
	Message parsedMessage2 = MessageParser::parse(reinterpret_cast<const uint8_t*>(createdPacket2.data()), createdPacket2.size());

	CHECK(createdPacket1 == createdPacket2);
	CHECK(parsedMessage2.packetType == message.packetType);
	CHECK(parsedMessage2.applicationId == message.applicationId);
	CHECK(parsedMessage2.packetSequenceCount == message.packetSequenceCount);
	CHECK(parsedMessage2.serviceType == message.serviceType);
	CHECK(parsedMessage2.messageType == message.messageType);
	CHECK(parsedMessage2.messageTypeCounter == message.messageTypeCounter);
	CHECK(parsedMessage2.destinationId == message.destinationId);
	CHECK(parsedMessage2.dataSize == message.dataSize);
	CHECK(memcmp(parsedMessage2.data.begin(), message.data.begin(), message.dataSize) == 0);
}

TEST_CASE("TC packet too short returns empty message", "[MessageParser]") {
    uint8_t packet[] = {0x18, 0x07, 0xe0};
    Message message = MessageParser::parse(packet, sizeof(packet));

	// Verify it's a default constructed message
    CHECK(message.serviceType == 0);
    CHECK(message.messageType == 0);
    CHECK(message.dataSize == 0);
    CHECK(message.packetSequenceCount == 0);
    CHECK(message.sourceId == 0);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnacceptablePacket));
}

TEST_CASE("TC message parsing detects a bit flip by CRC", "[MessageParser]") {
    if constexpr (ECSSCRCIncluded) {
        uint8_t packet[] = {0x18, 0x07, 0xe0, 0x07, 0x00, 0x0b, 0x20, 0x81,
                            0x1f, 0x00, 0x00, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x9b, 0xe8};
		const char expectedData[] = "hello";

        // Verify it parses correctly before flipping
        Message validMessage = MessageParser::parse(packet, sizeof(packet));
        CHECK(validMessage.packetType == Message::TC);
        CHECK(validMessage.applicationId == 7);
        CHECK(validMessage.packetSequenceCount == 8199);
        CHECK(validMessage.serviceType == 129);
        CHECK(validMessage.messageType == 31);
        CHECK(validMessage.dataSize == std::strlen(expectedData));
        CHECK(memcmp(validMessage.data.begin(), expectedData, validMessage.dataSize) == 0);
        ServiceTests::resetErrors();

        // Flip a bit in the payload and verify CRC catches it (returning a default constructed message)
        packet[11] ^= 0x01;
        Message corruptedMessage = MessageParser::parse(packet, sizeof(packet));

        CHECK(corruptedMessage.serviceType == 0);
        CHECK(corruptedMessage.messageType == 0);
        CHECK(corruptedMessage.dataSize == 0);
        CHECK(corruptedMessage.packetSequenceCount == 0);
        CHECK(ServiceTests::thrownError(ErrorHandler::InvalidCRC));
    }
}

TEST_CASE("TC message initialization and consistency explicit constructor", "[MessageParser]") {
	Message message(129, 31, Message::TC, 7, 55);
	message.packetSequenceCount = 8199;
	
	String<10> sourceString = "helloworld";
	message.appendString(sourceString);

	CHECK(message.dataSize == sourceString.size());

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);
	
	// For TC, sourceId corresponds to bytes 3 and 4 of the ECSS secondary header,
	// meaning bytes 9 and 10 of the created packet.
	CHECK(static_cast<uint8_t>(createdPacket[9]) == (55 >> 8));
	CHECK(static_cast<uint8_t>(createdPacket[10]) == (55 & 0xFF));

	Message parsedMessage = MessageParser::parse(reinterpret_cast<const uint8_t*>(createdPacket.data()), createdPacket.size());

	CHECK(parsedMessage.packetType == message.packetType);
	CHECK(parsedMessage.applicationId == message.applicationId);
	CHECK(parsedMessage.packetSequenceCount == message.packetSequenceCount);
	CHECK(parsedMessage.serviceType == message.serviceType);
	CHECK(parsedMessage.messageType == message.messageType);
	CHECK(parsedMessage.sourceId == message.sourceId);
	CHECK(parsedMessage.sourceId == 55);
	CHECK(parsedMessage.dataSize == message.dataSize);
	CHECK(memcmp(parsedMessage.data.begin(), message.data.begin(), message.dataSize) == 0);
}

TEST_CASE("TM message initialization and consistency explicit constructor", "[MessageParser]") {
	Message message(22, 17, Message::TM, 2, 42);
	message.packetSequenceCount = 77;
	
	String<7> sourceString = "hellohi";
	message.appendString(sourceString);

	CHECK(message.dataSize == sourceString.size());

	String<CCSDSMaxMessageSize> createdPacket = MessageParser::compose(message);

	// For TM, destinationId corresponds to bytes 5 and 6 of the ECSS secondary header,
	// meaning bytes 11 and 12 of the created packet.
	CHECK(static_cast<uint8_t>(createdPacket[11]) == (42 >> 8));
	CHECK(static_cast<uint8_t>(createdPacket[12]) == (42 & 0xFF));

	Message parsedMessage = MessageParser::parse(reinterpret_cast<const uint8_t*>(createdPacket.data()), createdPacket.size());

	CHECK(parsedMessage.packetType == message.packetType);
	CHECK(parsedMessage.applicationId == message.applicationId);
	CHECK(parsedMessage.packetSequenceCount == message.packetSequenceCount);
	CHECK(parsedMessage.serviceType == message.serviceType);
	CHECK(parsedMessage.messageType == message.messageType);
	CHECK(parsedMessage.destinationId == message.destinationId);
	CHECK(parsedMessage.destinationId == 42);
	CHECK(parsedMessage.dataSize == message.dataSize);
	CHECK(memcmp(parsedMessage.data.begin(), message.data.begin(), message.dataSize) == 0);
}