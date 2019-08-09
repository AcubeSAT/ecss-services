#include <Services/EventActionService.hpp>
#include <ServicePool.hpp>
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "macros.hpp"
#include "Services/TestService.hpp"
#include "Services/RequestVerificationService.hpp"

void MessageParser::execute(Message& message) {
	switch (message.serviceType) {
		case 5:
			Services.eventReport.execute(message); // ST[05]
			break;
		case 6:
			Services.memoryManagement.execute(message); // ST[06]
			break;
		case 8:
			Services.functionManagement.execute(message); // ST[08]
			break;
		case 9:
			Services.timeManagement.execute(message); // ST[09]
			break;
		case 11:
			Services.timeBasedScheduling.execute(message); // ST[11]
			break;
		case 17:
			Services.testService.execute(message); // ST[17]
			break;
		case 19:
			Services.eventAction.execute(message); // ST[19]
			break;
		case 20:
			Services.parameterManagement.execute(message); // ST[20]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

Message MessageParser::parse(uint8_t* data, uint32_t length) {
	ASSERT_INTERNAL(length >= 6, ErrorHandler::UnacceptablePacket);

	uint16_t packetHeaderIdentification = (data[0] << 8) | data[1];
	uint16_t packetSequenceControl = (data[2] << 8) | data[3];
	uint16_t packetDataLength = (data[4] << 8) | data[5];

	// Individual fields of the CCSDS Space Packet primary header
	uint8_t versionNumber = data[0] >> 5;
	Message::PacketType packetType = ((data[0] & 0x10) == 0) ? Message::TM : Message::TC;
	uint8_t secondaryHeaderFlag = data[0] & static_cast<uint8_t>(0x08);
	uint16_t APID = packetHeaderIdentification & static_cast<uint16_t>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);

	// Returning an internal error, since the Message is not available yet
	ASSERT_INTERNAL(versionNumber == 0U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(secondaryHeaderFlag == 1U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(sequenceFlags == 0x3U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(packetDataLength == (length - 6U), ErrorHandler::UnacceptablePacket);

	Message message(0, 0, packetType, APID);

	if (packetType == Message::TC) {
		parseECSSTCHeader(data + 6, packetDataLength, message);
	} else {
		parseECSSTMHeader(data + 6, packetDataLength, message);
	}

	return message;
}

void MessageParser::parseECSSTCHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= 5, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TC header
	uint8_t pusVersion = data[0] >> 4;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	// todo: Fix this parsing function, because it assumes PUS header in data, which is not true
	//  with the current implementation
	ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= 5;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + 5, length);
	message.dataSize = length;
}

Message MessageParser::parseECSSTC(String<ECSS_TC_REQUEST_STRING_SIZE> data) {
	Message message;
	auto* dataInt = reinterpret_cast<uint8_t*>(data.data());
	message.packetType = Message::TC;
	parseECSSTCHeader(dataInt, ECSS_TC_REQUEST_STRING_SIZE, message);
	return message;
}

Message MessageParser::parseECSSTC(uint8_t* data) {
	Message message;
	message.packetType = Message::TC;
	parseECSSTCHeader(data, ECSS_TC_REQUEST_STRING_SIZE, message);
	return message;
}

String<ECSS_TC_REQUEST_STRING_SIZE> MessageParser::createECSSTC(Message& message) {
	uint8_t tempString[ECSS_TC_REQUEST_STRING_SIZE] = {0};

	tempString[0] = ECSS_PUS_VERSION << 4; // Assign the pusVersion = 2
	tempString[1] = message.serviceType;
	tempString[2] = message.messageType;
	memcpy(tempString + 5, message.data, ECSS_TC_REQUEST_STRING_SIZE - 5);
	String<ECSS_TC_REQUEST_STRING_SIZE> dataString(tempString);

	return dataString;
}

void MessageParser::parseECSSTMHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= 5, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TM header
	uint8_t pusVersion = data[0] >> 4;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= 5;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + 5, length);
	message.dataSize = length;
}
