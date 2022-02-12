#include <ServicePool.hpp>
#include "ErrorHandler.hpp"
#include "MessageParser.hpp"
#include "macros.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Helpers/CRCHelper.hpp"

void MessageParser::execute(Message& message) {
	switch (message.serviceType) {

#ifdef SERVICE_HOUSEKEEPING
		case HousekeepingService::ServiceType:
			Services.housekeeping.execute(message);
			break;
#endif

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

#ifdef SERVICE_ONBOARDMONITORING
		case OnBoardMonitoringService::ServiceType:
			Services.onBoardMonitoringService.execute(message);
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

#ifdef SERVICE_FILE_MANAGEMENT
        case FileManagementService::ServiceType:
            Services.fileManagement.execute(message); // ST[23]
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
	uint16_t packetSequenceCount = packetSequenceControl & (~0xc000U); // keep last 14 bits

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
	ErrorHandler::assertRequest(length >= 5, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TC header
	uint8_t pusVersion = data[0] >> 4;
	uint8_t serviceType = data[1];
	uint8_t messageType = data[2];

	ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= 5;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	std::copy(data + 5, data + 5 + length, message.data);
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
	uint8_t header[5];

	if (message.packetType == Message::TC) {
		header[0] = ECSSPUSVersion << 4U; // Assign the pusVersion = 2
		header[1] = message.serviceType;
		header[2] = message.messageType;
		header[3] = 0;
		header[4] = 0;
	} else {
		header[0] = ECSSPUSVersion << 4U; // Assign the pusVersion = 2
		header[1] = message.serviceType;
		header[2] = message.messageType;
		header[3] = static_cast<uint8_t>(message.messageTypeCounter >> 8U);
		header[4] = static_cast<uint8_t>(message.messageTypeCounter & 0xffU);
	}

	String<CCSDSMaxMessageSize> dataString(header, 5);
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

String<CCSDSMaxMessageSize> MessageParser::compose(const Message& message) {
	uint8_t header[6];

	// First, compose the ECSS part
	String<CCSDSMaxMessageSize> ecssMessage = MessageParser::composeECSS(message);

	// Sanity check that there is enough space for the string
	ASSERT_INTERNAL((ecssMessage.size() + 6U) <= CCSDSMaxMessageSize, ErrorHandler::StringTooLarge);

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
	String<CCSDSMaxMessageSize> ccsdsMessage(header, 6);
	ccsdsMessage.append(ecssMessage);

#if ECSS_CRC_INCLUDED
	// Append CRC field
	uint16_t crcField = CRCHelper::calculateCRC(reinterpret_cast<uint8_t*>(ccsdsMessage.data()), 6 + packetDataLength);
	ccsdsMessage.push_back(static_cast<uint8_t>(crcField >> 8U));
	ccsdsMessage.push_back(static_cast<uint8_t>(crcField & 0xFF));
#endif

	return ccsdsMessage;
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
	message.serviceType = serviceType;
	message.messageType = messageType;
	std::copy(data + 5, data + 5 + length, message.data);
	message.dataSize = length;
}
