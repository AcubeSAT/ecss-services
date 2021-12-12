#include <ServicePool.hpp>
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "macros.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Helpers/CRCHelper.hpp"

void MessageParser::execute(Message& message) {
	switch (message.serviceType) {
#ifdef SERVICE_PARAMETERSTATISTICS
		case ParameterStatisticsService::ServiceType:
			Services.parameterStatistics.execute(message);
			break;
#endif

#ifdef SERVICE_EVENTREPORT
		case EventReportService::ServiceType:
			Services.eventReport.execute(message);
			break;
#endif

#ifdef SERVICE_MEMORY
		case MemoryManagementService::ServiceType:
			Services.memoryManagement.execute(message);
			break;
#endif

#ifdef SERVICE_FUNCTION
		case FunctionManagementService::ServiceType:
			Services.functionManagement.execute(message);
			break;
#endif

#ifdef SERVICE_TIMESCHEDULING
		case TimeBasedSchedulingService::ServiceType:
			Services.timeBasedScheduling.execute(message);
			break;
#endif

#ifdef SERVICE_TEST
		case TestService::ServiceType:
			Services.testService.execute(message);
			break;
#endif

#ifdef SERVICE_EVENTACTION
		case EventActionService::ServiceType:
			Services.eventAction.execute(message);
			break;
#endif

#ifdef SERVICE_PARAMETER
		case ParameterService::ServiceType:
			Services.parameterManagement.execute(message);
			break;
#endif

		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

Message MessageParser::parse(uint8_t* data, uint32_t length) {
	ASSERT_INTERNAL(length >= CCSDSPrimaryHeaderLength, ErrorHandler::UnacceptablePacket);

	uint16_t packetHeaderIdentification = ((data[0] & 0b11111) << 8) | data[1];
	uint16_t packetSequenceControl = (data[2] << 8) | data[3];
	uint16_t packetDataLength = (data[4] << 8) | data[5];

	uint8_t versionNumber = data[0] >> 5;
	Message::PacketType packetType = ((data[0] & 0x10) == 0) ? Message::TM : Message::TC;
	bool secondaryHeaderFlag = (data[0] & 0x08U) != 0U;
	uint16_t APID = packetHeaderIdentification & static_cast<uint16_t>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);
	uint16_t packetSequenceCount = packetSequenceControl & (~0xc000U); // keep last 14 bits

	ASSERT_INTERNAL(versionNumber == 0U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(secondaryHeaderFlag, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(sequenceFlags == 0x3U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(packetDataLength == (length - CCSDSPrimaryHeaderLength), ErrorHandler::UnacceptablePacket);

	Message message(0, 0, packetType, APID);
	message.packetSequenceCount = packetSequenceCount;

	if (packetType == Message::TC) {
		parseECSSTCHeader(data + CCSDSPrimaryHeaderLength, packetDataLength, message);
	} else {
		parseECSSTMHeader(data + CCSDSPrimaryHeaderLength, packetDataLength, message);
	}

	return message;
}

void MessageParser::parseECSSTCHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSSSecondaryTCHeaderLength, message, ErrorHandler::UnacceptableMessage);

	uint8_t response_band = (data[0] & 0b10000) >> 4;
	uint8_t acknowledgement_flags = data[0] & 0b1111;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	length -= ECSSSecondaryTCHeaderLength;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	message.responseBand = Message::ResponseBand(response_band);
	message.acknowledgementFlags = acknowledgement_flags;
	//memcpy(message.data, data + ECSSSecondaryTCHeaderLength, length);
	std::copy(data + ECSSSecondaryTCHeaderLength, data + ECSSSecondaryTCHeaderLength + length, message.data);
	message.dataSize = length;
}

Message MessageParser::parseECSSTC(String<ECSSTCRequestStringSize> data) {
	Message message;
	auto* dataInt = reinterpret_cast<uint8_t*>(data.data());
	message.packetType = Message::TC;
	parseECSSTCHeader(dataInt, ECSSTCRequestStringSize, message);
	return message;
}

Message MessageParser::parseECSSTC(uint8_t* data) {
	Message message;
	message.packetType = Message::TC;
	parseECSSTCHeader(data, ECSSTCRequestStringSize, message);
	return message;
}

String<CCSDSMaxMessageSize> MessageParser::composeECSS(const Message& message, uint16_t size) {
	if (message.packetType == Message::TC) {
		return composeECSSTC(message, size);
	}
	return composeECSSTM(message, size);
}

String<CCSDSMaxMessageSize> MessageParser::composeECSSTC(const Message& message, uint16_t size){
	uint8_t header[ECSSSecondaryTCHeaderLength];

	header[0] = message.responseBand << 4;
	header[0] |= message.acknowledgementFlags & 0b1111;
	header[1] = message.serviceType;
	header[2] = message.messageType;


	String<CCSDSMaxMessageSize> dataString(header, ECSSSecondaryTCHeaderLength);

	dataString.append(message.data, message.dataSize);

	return reachRequestedSize(dataString, size, message);
}

String<CCSDSMaxMessageSize> MessageParser::composeECSSTM(const Message& message, uint16_t size){
	uint8_t header[ECSSSecondaryTMHeaderLength];

	header[0] = message.serviceType;
	header[1] = message.messageType;
	header[2] = static_cast<uint8_t>(message.messageTypeCounter);

	header[3] = 0;
	header[4] = 0;
	header[5] = 0;
	header[6] = 0;

	String<CCSDSMaxMessageSize> dataString(header, ECSSSecondaryTMHeaderLength);
	dataString.append(message.data, message.dataSize);

	return reachRequestedSize(dataString, size, message);
}

String<CCSDSMaxMessageSize> MessageParser::reachRequestedSize(String<CCSDSMaxMessageSize> dataString, uint16_t
size, const Message& message){

	if (size != 0) {
		if (dataString.size() > size) {
			ErrorHandler::reportInternalError(ErrorHandler::NestedMessageTooLarge);
		} else if (dataString.size() < size) {
			dataString.append(size - dataString.size(), 0);
		}
	}

	return dataString;
}

String<CCSDSMaxMessageSize> MessageParser::compose(const Message& message) {
	uint8_t header[6];


	String<CCSDSMaxMessageSize> ecssMessage = composeECSS(message);

	ASSERT_INTERNAL((ecssMessage.size() + 6U) <= CCSDSMaxMessageSize, ErrorHandler::StringTooLarge);


	uint16_t packetId = message.applicationId;
	packetId |= (1U << 11U); // Secondary header flag
	packetId |= (message.packetType == Message::TC) ? (1U << 12U) : (0U); // Ignore-MISRA
	uint16_t packetSequenceControl = message.packetSequenceCount | (3U << 14U);
	uint16_t packetDataLength = ecssMessage.size();

	header[0] = packetId >> 8U;
	header[1] = packetId & 0xffU;
	header[2] = packetSequenceControl >> 8U;
	header[3] = packetSequenceControl & 0xffU;
	header[4] = packetDataLength >> 8U;
	header[5] = packetDataLength & 0xffU;


	String<CCSDSMaxMessageSize> ccsdsMessage(header, CCSDSPrimaryHeaderLength);
	ccsdsMessage.append(ecssMessage);

	if(ECSSCRCIncluded) {
		uint16_t crcField =
		    CRCHelper::calculateCRC(reinterpret_cast<uint8_t*>(ccsdsMessage.data()), CCSDSPrimaryHeaderLength + packetDataLength);
		ccsdsMessage.push_back(static_cast<uint8_t>(crcField >> 8U));
		ccsdsMessage.push_back(static_cast<uint8_t>(crcField & 0xFF));
	}

	return ccsdsMessage;
}

void MessageParser::parseECSSTMHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSSSecondaryTMHeaderLength, message, ErrorHandler::UnacceptableMessage);

	uint8_t serviceType = data[0];
	uint8_t messageType = data[1];
	uint8_t messageTypeCounter = data[2];
	uint32_t messageTime = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];
	length -= ECSSSecondaryTMHeaderLength;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;

	message.timeAtCreation = messageTime;
	message.messageTypeCounter = messageTypeCounter;
	memcpy(message.data, data + ECSSSecondaryTMHeaderLength, length);
	message.dataSize = length;
}
