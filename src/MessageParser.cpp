#include "MessageParser.hpp"
#include <ServicePool.hpp>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "Services/RequestVerificationService.hpp"
#include "macros.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers) The reason is that we do a lot of bit shifts
static_assert(sizeof(ServiceTypeNum) == 1);

static_assert(sizeof(MessageTypeNum) == 1);

void MessageParser::execute(Message& message) { //cppcheck-suppress[constParameter,constParameterReference]
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

#ifdef SERVICE_STORAGEANDRETRIEVAL
		case StorageAndRetrievalService::ServiceType:
			Services.storageAndRetrieval.execute(message);
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

#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
		case RealTimeForwardingControlService::ServiceType:
			Services.realTimeForwarding.execute(message);
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

Message MessageParser::parse(const uint8_t* data, uint32_t length) {
	ASSERT_INTERNAL(length >= CCSDSPrimaryHeaderSize, ErrorHandler::UnacceptablePacket);

	uint16_t const packetHeaderIdentification = (data[0] << 8) | data[1];
	uint16_t const packetSequenceControl = (data[2] << 8) | data[3];
	uint16_t const packetDataLength = (data[4] << 8) | data[5];

	// Individual fields of the CCSDS Space Packet primary header
	uint8_t const versionNumber = data[0] >> 5;
	Message::PacketType const packetType = ((data[0] & 0x10) == 0) ? Message::TM : Message::TC;
	bool const secondaryHeaderFlag = (data[0] & 0x08U) != 0U;
	ApplicationProcessId const APID = packetHeaderIdentification & static_cast<ApplicationProcessId>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);
	SequenceCount const packetSequenceCount = packetSequenceControl & (~0xc000U); // keep last 14 bits

	// Returning an internal error, since the Message is not available yet
	ASSERT_INTERNAL(versionNumber == 0U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(secondaryHeaderFlag, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(sequenceFlags == 0x3U, ErrorHandler::UnacceptablePacket);
	ASSERT_INTERNAL(packetDataLength == (length - CCSDSPrimaryHeaderSize), ErrorHandler::UnacceptablePacket);

	Message message(0, 0, packetType, APID);
	message.packetSequenceCount = packetSequenceCount;

	if (packetType == Message::TC) {
		parseECSSTCHeader(data + CCSDSPrimaryHeaderSize, packetDataLength, message);
	} else {
		parseECSSTMHeader(data + CCSDSPrimaryHeaderSize, packetDataLength, message);
	}

	return message;
}

void MessageParser::parseECSSTCHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSSSecondaryTCHeaderSize, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TC header
	uint8_t const pusVersion = data[0] >> 4;
	ServiceTypeNum const serviceType = data[1];
	MessageTypeNum const messageType = data[2];
	SourceId const sourceId = (data[3] << 8) + data[4];

	ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= ECSSSecondaryTCHeaderSize;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	message.sourceId = sourceId;
	std::copy(data + ECSSSecondaryTCHeaderSize, data + ECSSSecondaryTCHeaderSize + length, message.data.begin());
	message.dataSize = length;
}

Message MessageParser::parseECSSTC(String<ECSSTCRequestStringSize> data) {
	Message message;
	const auto* dataInt = reinterpret_cast<uint8_t*>(data.data());
	message.packetType = Message::TC;
	parseECSSTCHeader(dataInt, ECSSTCRequestStringSize, message);
	return message;
}

Message MessageParser::parseECSSTC(const uint8_t* data) {
	Message message;
	message.packetType = Message::TC;
	parseECSSTCHeader(data, ECSSTCRequestStringSize, message);
	return message;
}

String<CCSDSMaxMessageSize> MessageParser::composeECSS(const Message& message, uint16_t size) {
	// Unfortunately to avoid using VLAs, we will create an array with the maximum size.
	etl::array<uint8_t, ECSSSecondaryTMHeaderSize> header = {};

	if (message.packetType == Message::TC) {
		header[0] = ECSSPUSVersion << 4U; // Assign the pusVersion = 2
		header[0] |= 0x00;                //ack flags
		header[1] = message.serviceType;
		header[2] = message.messageType;
		header[3] = message.applicationId >> 8U;
		header[4] = message.applicationId;
	} else {
		header[0] = ECSSPUSVersion << 4U; // Assign the pusVersion = 2
		header[0] |= 0x00;                // Spacecraft time reference status
		header[1] = message.serviceType;
		header[2] = message.messageType;
		header[3] = static_cast<uint8_t>(message.messageTypeCounter >> 8U);
		header[4] = static_cast<uint8_t>(message.messageTypeCounter & 0xffU);
		header[5] = message.applicationId >> 8U; // DestinationID
		header[6] = message.applicationId;
		uint32_t ticks = TimeGetter::getCurrentTimeDefaultCUC().formatAsBytes();
		header[7] = (ticks >> 24) & 0xffU;
		header[8] = (ticks >> 16) & 0xffU;
		header[9] = (ticks >> 8) & 0xffU;
		header[10] = (ticks) & 0xffU;
	}

	String<CCSDSMaxMessageSize> dataString(header.data(), ((message.packetType == Message::TM) ? ECSSSecondaryTMHeaderSize : ECSSSecondaryTCHeaderSize));
	dataString.append(message.data.begin(), message.dataSize);

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
	etl::array<uint8_t, CCSDSPrimaryHeaderSize> header = {};

	// First, compose the ECSS part
	String<CCSDSMaxMessageSize> ecssMessage = MessageParser::composeECSS(message);

	// Sanity check that there is enough space for the string
	ASSERT_INTERNAL((ecssMessage.size() + CCSDSPrimaryHeaderSize) <= CCSDSMaxMessageSize, ErrorHandler::StringTooLarge);

	// Parts of the header
	ApplicationProcessId packetId = message.applicationId;
	packetId |= (1U << 11U);                                              // Secondary header flag
	packetId |= (message.packetType == Message::TC) ? (1U << 12U) : (0U); // Ignore-MISRA
	SequenceCount const packetSequenceControl = message.packetSequenceCount | (3U << 14U);
	uint16_t packetDataLength = ecssMessage.size() - 1;

	// Compile the header
	header[0] = packetId >> 8U;
	header[1] = packetId & 0xffU;
	header[2] = packetSequenceControl >> 8U;
	header[3] = packetSequenceControl & 0xffU;
	header[4] = packetDataLength >> 8U;
	header[5] = packetDataLength & 0xffU;

	// Compile the final message by appending the header
	String<CCSDSMaxMessageSize> ccsdsMessage(header.data(), CCSDSPrimaryHeaderSize);
	ccsdsMessage.append(ecssMessage);


	if constexpr (CRCHelper::EnableCRC) {
		const CRCSize crcField = CRCHelper::calculateCRC(reinterpret_cast<uint8_t*>(ccsdsMessage.data()), CCSDSPrimaryHeaderSize + ecssMessage.size());
		etl::array<uint8_t, CRCField> crcMessage = {static_cast<uint8_t>(crcField >> 8U), static_cast<uint8_t>
		                                            (crcField &  0xFF)};
		String<CCSDSMaxMessageSize> crcString(crcMessage.data(), 2);
		ccsdsMessage.append(crcString);
	}

	return ccsdsMessage;
}

void MessageParser::parseECSSTMHeader(const uint8_t* data, uint16_t length, Message& message) {
	ErrorHandler::assertRequest(length >= ECSSSecondaryTMHeaderSize, message, ErrorHandler::UnacceptableMessage);

	// Individual fields of the TM header
	uint8_t const pusVersion = data[0] >> 4;
	ServiceTypeNum const serviceType = data[1];
	MessageTypeNum const messageType = data[2];

	ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage);

	// Remove the length of the header
	length -= ECSSSecondaryTMHeaderSize;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	std::copy(data + ECSSSecondaryTMHeaderSize, data + ECSSSecondaryTMHeaderSize + length, message.data.begin());
	message.dataSize = length;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
