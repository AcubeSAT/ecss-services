#include <Services/EventActionService.hpp>
#include <ServicePool.hpp>
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "macros.hpp"
#include "Services/TestService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Helpers/CRCHelper.hpp"

void MessageParser::execute(Message& message) {
	switch (message.serviceType) {
#ifdef SERVICE_EVENTREPORT
		case 5:
			Services.eventReport.execute(message); // ST[05]
			break;
#endif

#ifdef SERVICE_MEMORY
		case 6:
			Services.memoryManagement.execute(message); // ST[06]
			break;
#endif

#ifdef SERVICE_FUNCTION
		case 8:
			Services.functionManagement.execute(message); // ST[08]
			break;
#endif

#ifdef SERVICE_TIMESCHEDULING
		case 11:
			Services.timeBasedScheduling.execute(message); // ST[11]
			break;
#endif

#ifdef SERVICE_TEST
		case 17:
			Services.testService.execute(message); // ST[17]
			break;
#endif

#ifdef SERVICE_EVENTACTION
		case 19:
			Services.eventAction.execute(message); // ST[19]
			break;
#endif

#ifdef SERVICE_PARAMETER
		case 20:
			Services.parameterManagement.execute(message); // ST[20]
			break;
#endif

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
	bool secondaryHeaderFlag = (data[0] & 0x08U) != 0U;
	uint16_t APID = packetHeaderIdentification & static_cast<uint16_t>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);
	uint16_t packetSequenceCount = packetSequenceControl & (~ 0xc000U); // keep last 14 bits

	// Returning an internal error, since the Message is not available yet
	ASSERT_INTERNAL(versionNumber == 0U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(secondaryHeaderFlag, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(sequenceFlags == 0x3U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(packetDataLength == (length - 6U), ErrorHandler::UnacceptablePacket);

	Message message(0, 0, packetType, APID);
	message.packetSequenceCount = packetSequenceCount;

	if (packetType == Message::TC) {
		parseECSSTCHeader(data + 6, packetDataLength, message);
	} else {
		parseECSSTMHeader(data + 6, packetDataLength, message);
	}

	return message;
}

void MessageParser::parseECSSTCHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSS_SECONDARY_TC_HEADER_LENGTH, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TC header
	uint8_t response_band = (data[0] & 00010000) >> 4;
	uint8_t acknowledgement_flags = data[0] & 00001111;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	// Remove the length of the header
	length -= ECSS_SECONDARY_TC_HEADER_LENGTH;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + ECSS_SECONDARY_TC_HEADER_LENGTH, length);
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

String<CCSDS_MAX_MESSAGE_SIZE> MessageParser::composeECSS(const Message& message, uint16_t size) {
	uint8_t header[ECSS_SECONDARY_TM_HEADER_LENGTH];

	if (message.packetType == Message::TC) {
		header[0] = message.serviceType;
		header[1] = message.messageType;
	} else {
		header[0] = message.serviceType;
		header[1] = message.messageType;
		header[2] = static_cast<uint8_t>(message.messageTypeCounter);
	}

	String<CCSDS_MAX_MESSAGE_SIZE> dataString(header, 5);
	dataString.append(message.data, message.dataSize);

	// Make sure to reach the requested size
	if (size != 0) {
		if (dataString.size() > size) {
			// Message overflow
			ErrorHandler::reportInternalError(ErrorHandler::NestedMessageTooLarge);
		} else if (dataString.size() < size) {
			// Append some 0s
			dataString.append(size - dataString.size(), 0);
		} else {
			// The message has an equal size to the requested one - do nothing
		}
	}

	return dataString;
}

String<CCSDS_MAX_MESSAGE_SIZE> MessageParser::compose(const Message& message) {
	uint8_t header[6];

	// First, compose the ECSS part
	String<CCSDS_MAX_MESSAGE_SIZE> ecssMessage = MessageParser::composeECSS(message);

	// Sanity check that there is enough space for the string
	ASSERT_INTERNAL((ecssMessage.size() + 6U) <= CCSDS_MAX_MESSAGE_SIZE, ErrorHandler::StringTooLarge);

	// Parts of the header
	uint16_t packetId = message.applicationId;
	packetId |= (1U << 11U); // Secondary header flag
	packetId |= (message.packetType == Message::TC) ? (1U << 12U) : (0U); // Ignore-MISRA
	uint16_t packetSequenceControl = message.packetSequenceCount | (3U << 14U);
	uint16_t packetDataLength = ecssMessage.size();

	// Compile the header
	header[0] = packetId >> 8U;
	header[1] = packetId & 0xffU;
	header[2] = packetSequenceControl >> 8U;
	header[3] = packetSequenceControl & 0xffU;
	header[4] = packetDataLength >> 8U;
	header[5] = packetDataLength & 0xffU;

	// Compile the final message by appending the header
	String<CCSDS_MAX_MESSAGE_SIZE> ccsdsMessage(header, 6);
	ccsdsMessage.append(ecssMessage);

#if ECSS_CRC_INCLUDED
	// Append CRC field
	uint16_t crcField = CRCHelper::calculateCRC(reinterpret_cast<uint8_t*>(ccsdsMessage.data()), 6 +
	packetDataLength);
	ccsdsMessage.push_back(static_cast<uint8_t>(crcField >> 8U));
	ccsdsMessage.push_back(static_cast<uint8_t>(crcField & 0xFF));
#endif

	return ccsdsMessage;
}


void MessageParser::parseECSSTMHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSS_SECONDARY_TM_HEADER_LENGTH, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TM header
	uint8_t serviceType = data[0];
	uint8_t messageType = data[1];

	// Remove the length of the header
	length -= ECSS_SECONDARY_TM_HEADER_LENGTH;

	// Copy the data to the message
	// TODO: See if memcpy is needed for this
	message.serviceType = serviceType;
	message.messageType = messageType;
	memcpy(message.data, data + ECSS_SECONDARY_TM_HEADER_LENGTH, length);
	message.dataSize = length;
}
