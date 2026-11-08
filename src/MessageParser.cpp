#include "MessageParser.hpp"
#include <ServicePool.hpp>
#include <algorithm>
#include <initializer_list>
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

static bool isOneOf(MessageTypeNum messageType, std::initializer_list<MessageTypeNum> messageTypes) {
	return std::find(messageTypes.begin(), messageTypes.end(), messageType) != messageTypes.end();
}

bool MessageParser::canExecute(const Message& message) {
	const MessageTypeNum messageType = message.messageType;
	switch (message.serviceType) {

#ifdef SERVICE_HOUSEKEEPING
		case HousekeepingService::ServiceType:
			return isOneOf(messageType, {HousekeepingService::CreateHousekeepingReportStructure,
			                             HousekeepingService::DeleteHousekeepingReportStructure,
			                             HousekeepingService::EnablePeriodicHousekeepingParametersReport,
			                             HousekeepingService::DisablePeriodicHousekeepingParametersReport,
			                             HousekeepingService::ReportHousekeepingStructures,
			                             HousekeepingService::GenerateOneShotHousekeepingReport,
			                             HousekeepingService::AppendParametersToHousekeepingStructure,
			                             HousekeepingService::ModifyCollectionIntervalOfStructures,
			                             HousekeepingService::ReportHousekeepingPeriodicProperties});
#endif

#ifdef SERVICE_PARAMETERSTATISTICS
		case ParameterStatisticsService::ServiceType:
			return isOneOf(messageType, {ParameterStatisticsService::ReportParameterStatistics,
			                             ParameterStatisticsService::ResetParameterStatistics,
			                             ParameterStatisticsService::EnablePeriodicParameterReporting,
			                             ParameterStatisticsService::DisablePeriodicParameterReporting,
			                             ParameterStatisticsService::AddOrUpdateParameterStatisticsDefinitions,
			                             ParameterStatisticsService::DeleteParameterStatisticsDefinitions,
			                             ParameterStatisticsService::ReportParameterStatisticsDefinitions});
#endif

#ifdef SERVICE_EVENTREPORT
		case EventReportService::ServiceType:
			return isOneOf(messageType, {EventReportService::EnableReportGenerationOfEvents,
			                             EventReportService::DisableReportGenerationOfEvents,
			                             EventReportService::ReportListOfDisabledEvents});
#endif

#ifdef SERVICE_MEMORY
		case MemoryManagementService::ServiceType:
			return isOneOf(messageType, {MemoryManagementService::LoadRawMemoryDataAreas,
			                             MemoryManagementService::DumpRawMemoryData,
			                             MemoryManagementService::CheckRawMemoryData});
#endif

#ifdef SERVICE_FUNCTION
		case FunctionManagementService::ServiceType:
			return isOneOf(messageType, {FunctionManagementService::PerformFunction});
#endif

#ifdef SERVICE_TIMESCHEDULING
		case TimeBasedSchedulingService::ServiceType:
			return isOneOf(messageType, {TimeBasedSchedulingService::EnableTimeBasedScheduleExecutionFunction,
			                             TimeBasedSchedulingService::DisableTimeBasedScheduleExecutionFunction,
			                             TimeBasedSchedulingService::ResetTimeBasedSchedule,
			                             TimeBasedSchedulingService::InsertActivities,
			                             TimeBasedSchedulingService::DeleteActivitiesById,
			                             TimeBasedSchedulingService::TimeShiftActivitiesById,
			                             TimeBasedSchedulingService::DetailReportActivitiesById,
			                             TimeBasedSchedulingService::ActivitiesSummaryReportById,
			                             TimeBasedSchedulingService::TimeShiftALlScheduledActivities,
			                             TimeBasedSchedulingService::DetailReportAllScheduledActivities});
#endif

#ifdef SERVICE_STORAGEANDRETRIEVAL
		case StorageAndRetrievalService::ServiceType:
			return isOneOf(messageType, {StorageAndRetrievalService::EnableStorageInPacketStores,
			                             StorageAndRetrievalService::DisableStorageInPacketStores,
			                             StorageAndRetrievalService::StartByTimeRangeRetrieval,
			                             StorageAndRetrievalService::DeletePacketStoreContent,
			                             StorageAndRetrievalService::ReportContentSummaryOfPacketStores,
			                             StorageAndRetrievalService::ChangeOpenRetrievalStartingTime,
			                             StorageAndRetrievalService::ResumeOpenRetrievalOfPacketStores,
			                             StorageAndRetrievalService::SuspendOpenRetrievalOfPacketStores,
			                             StorageAndRetrievalService::AbortByTimeRangeRetrieval,
			                             StorageAndRetrievalService::ReportStatusOfPacketStores,
			                             StorageAndRetrievalService::CreatePacketStores,
			                             StorageAndRetrievalService::DeletePacketStores,
			                             StorageAndRetrievalService::ReportConfigurationOfPacketStores,
			                             StorageAndRetrievalService::CopyPacketsInTimeWindow,
			                             StorageAndRetrievalService::ResizePacketStores,
			                             StorageAndRetrievalService::ChangeTypeToCircular,
			                             StorageAndRetrievalService::ChangeTypeToBounded,
			                             StorageAndRetrievalService::ChangeVirtualChannel});
#endif

#ifdef SERVICE_ONBOARDMONITORING
		case OnBoardMonitoringService::ServiceType:
			return isOneOf(messageType, {OnBoardMonitoringService::EnableParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::DisableParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::ChangeMaximumTransitionReportingDelay,
			                             OnBoardMonitoringService::DeleteAllParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::AddParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::DeleteParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::ModifyParameterMonitoringDefinitions,
			                             OnBoardMonitoringService::ReportParameterMonitoringDefinitions});
#endif

#ifdef SERVICE_TEST
		case TestService::ServiceType:
			return isOneOf(messageType, {TestService::AreYouAliveTest, TestService::OnBoardConnectionTest});
#endif

#ifdef SERVICE_EVENTACTION
		case EventActionService::ServiceType:
			return isOneOf(messageType, {EventActionService::AddEventAction,
			                             EventActionService::DeleteEventAction,
			                             EventActionService::DeleteAllEventAction,
			                             EventActionService::EnableEventAction,
			                             EventActionService::DisableEventAction,
			                             EventActionService::ReportStatusOfEachEventAction,
			                             EventActionService::EnableEventActionFunction,
			                             EventActionService::DisableEventActionFunction});
#endif

#ifdef SERVICE_PARAMETER
		case ParameterService::ServiceType:
			return isOneOf(messageType, {ParameterService::ReportParameterValues, ParameterService::SetParameterValues});
#endif

#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
		case RealTimeForwardingControlService::ServiceType:
			return isOneOf(messageType, {RealTimeForwardingControlService::AddReportTypesToAppProcessConfiguration,
			                             RealTimeForwardingControlService::DeleteReportTypesFromAppProcessConfiguration,
			                             RealTimeForwardingControlService::ReportAppProcessConfigurationContent});
#endif

#ifdef SERVICE_FILE_MANAGEMENT
		case FileManagementService::ServiceType:
			return isOneOf(messageType, {FileManagementService::CreateFile,
			                             FileManagementService::DeleteFile,
			                             FileManagementService::ReportAttributes,
			                             FileManagementService::LockFile,
			                             FileManagementService::UnlockFile,
			                             FileManagementService::CreateDirectory,
			                             FileManagementService::DeleteDirectory});
#endif
		default:
			return false;
	}
}

uint32_t MessageParser::packetSize(const uint8_t* data) {
	return CCSDSPrimaryHeaderSize + ((static_cast<uint32_t>(data[4]) << 8U) | data[5]) + 1U;
}

Message MessageParser::parse(const uint8_t* data, uint32_t length) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL(length >= CCSDSPrimaryHeaderSize, ErrorHandler::UnacceptablePacket)) {
		return {};
	}

	uint16_t const packetHeaderIdentification = (data[0] << 8) | data[1];
	uint16_t const packetSequenceControl = (data[2] << 8) | data[3];
	uint32_t const packetDataLength = packetSize(data) - CCSDSPrimaryHeaderSize;

	// Individual fields of the CCSDS Space Packet primary header
	uint8_t const versionNumber = data[0] >> 5;
	Message::PacketType const packetType = ((data[0] & 0x10) == 0) ? Message::TM : Message::TC;
	bool const secondaryHeaderFlag = (data[0] & 0x08U) != 0U;
	ApplicationProcessId const APID = packetHeaderIdentification & static_cast<ApplicationProcessId>(0x07ff);
	auto sequenceFlags = static_cast<uint8_t>(packetSequenceControl >> 14);
	SequenceCount const packetSequenceCount = packetSequenceControl & (~0xc000U); // keep last 14 bits

	// Returning an internal error, since the Message is not available yet
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL(versionNumber == 0U, ErrorHandler::UnacceptablePacket)) {
		return {};
	}
	if (not ASSERT_INTERNAL(secondaryHeaderFlag, ErrorHandler::UnacceptablePacket)) {
		return {};
	}
	if (not ASSERT_INTERNAL(sequenceFlags == 0x3U, ErrorHandler::UnacceptablePacket)) {
		return {};
	}
	if (not ASSERT_INTERNAL(packetDataLength == (length - CCSDSPrimaryHeaderSize), ErrorHandler::UnacceptablePacket)) {
		return {};
	}

	Message message(0, 0, packetType, APID);
	message.packetSequenceCount = packetSequenceCount;

	uint16_t payloadLength = packetDataLength;
	if constexpr (ECSSCRCIncluded) {
		if (not ASSERT_INTERNAL(packetDataLength >= CRCHelper::CRCField, ErrorHandler::UnacceptablePacket)) {
			return {};
		}

		if (not ASSERT_INTERNAL(CRCHelper::validateCRC(data, length), ErrorHandler::InvalidCRC)) {
			return {};
		}

		payloadLength -= CRCHelper::CRCField;
	}

	if (packetType == Message::TC) {
		if (not parseECSSTCHeader(data + CCSDSPrimaryHeaderSize, payloadLength, message).has_value()) {
			return {};
		}
	} else {
		if (not parseECSSTMHeader(data + CCSDSPrimaryHeaderSize, payloadLength, message).has_value()) {
			return {};
		}
	}

	return message;
}

etl::expected<void, ErrorHandler::AcceptanceErrorType> MessageParser::parseECSSTCHeader(const uint8_t* data, uint16_t length, Message& message) {
	if (not ErrorHandler::assertRequest((length >= ECSSSecondaryTCHeaderSize) and (length <= ECSSSecondaryTCHeaderSize + ECSSMaxMessageSize), message, ErrorHandler::UnacceptableMessage)) {
		return etl::unexpected<ErrorHandler::AcceptanceErrorType>(ErrorHandler::UnacceptableMessage);
	}

	// Individual fields of the TC header
	uint8_t const pusVersion = data[0] >> 4;
	ServiceTypeNum const serviceType = data[1];
	MessageTypeNum const messageType = data[2];
	ApplicationProcessUserId const sourceId = (data[3] << 8) + data[4];

	if (not ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage)) {
		return etl::unexpected<ErrorHandler::AcceptanceErrorType>(ErrorHandler::UnacceptableMessage);
	}

	// Remove the length of the header
	length -= ECSSSecondaryTCHeaderSize;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	message.sourceId = sourceId;
	std::copy(data + ECSSSecondaryTCHeaderSize, data + ECSSSecondaryTCHeaderSize + length, message.data.begin());
	message.dataSize = length;

	return {};
}

Message MessageParser::parseECSSTC(String<ECSSTCRequestStringSize> data) {
	Message message;
	const auto* dataInt = reinterpret_cast<uint8_t*>(data.data());
	message.packetType = Message::TC;
	if (not parseECSSTCHeader(dataInt, ECSSTCRequestStringSize, message).has_value()) {
		return {};
	}
	return message;
}

Message MessageParser::parseECSSTC(const uint8_t* data) {
	Message message;
	message.packetType = Message::TC;
	if (not parseECSSTCHeader(data, ECSSTCRequestStringSize, message).has_value()) {
		return {};
	}
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
		header[3] = message.sourceId >> 8U;
		header[4] = message.sourceId;
	} else {
		header[0] = ECSSPUSVersion << 4U; // Assign the pusVersion = 2
		header[0] |= (message.timeReferenceStatus & 0x0FU); // Spacecraft time reference status
		header[1] = message.serviceType;
		header[2] = message.messageType;
		header[3] = static_cast<uint8_t>(message.messageTypeCounter >> 8U);
		header[4] = static_cast<uint8_t>(message.messageTypeCounter & 0xffU);
		header[5] = message.destinationId >> 8U;
		header[6] = message.destinationId;
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

	// TODO(#59): Proper error handling if assert fails
	// Sanity check that there is enough space for the string
	if (not ASSERT_INTERNAL((ecssMessage.size() + CCSDSPrimaryHeaderSize) <= CCSDSMaxMessageSize, ErrorHandler::StringTooLarge)) {
		return {""};
	}

	// Parts of the header
	ApplicationProcessId packetId = message.applicationId;
	packetId |= (1U << 11U);                                              // Secondary header flag
	packetId |= (message.packetType == Message::TC) ? (1U << 12U) : (0U); // Ignore-MISRA
	SequenceCount const packetSequenceControl = message.packetSequenceCount | (3U << 14U);
	uint16_t packetDataLength = ecssMessage.size() - 1;

	if constexpr (ECSSCRCIncluded) {
		if (not ASSERT_INTERNAL(packetDataLength <= std::numeric_limits<uint16_t>::max() - CRCHelper::CRCField, ErrorHandler::StringTooLarge)) {
			return {""};
		}
		packetDataLength += CRCHelper::CRCField;
	}

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


	if constexpr (ECSSCRCIncluded) {
		const CRCSize crcField = CRCHelper::calculateCRC(reinterpret_cast<uint8_t*>(ccsdsMessage.data()), CCSDSPrimaryHeaderSize + ecssMessage.size());
		etl::array<uint8_t, CRCHelper::CRCField> crcMessage = {static_cast<uint8_t>(crcField >> 8U), static_cast<uint8_t>
		                                            (crcField &  0xFF)};
		ccsdsMessage.append(crcMessage.data(), crcMessage.size());
	}

	return ccsdsMessage;
}

etl::expected<void, ErrorHandler::AcceptanceErrorType> MessageParser::parseECSSTMHeader(const uint8_t* data, uint16_t length, Message& message) {
	if (not ErrorHandler::assertRequest((length >= ECSSSecondaryTMHeaderSize) and (length <= ECSSSecondaryTMHeaderSize + ECSSMaxMessageSize), message, ErrorHandler::UnacceptableMessage)) {
		return etl::unexpected<ErrorHandler::AcceptanceErrorType>(ErrorHandler::UnacceptableMessage);
	}

	// Individual fields of the TM header
	uint8_t const pusVersion = data[0] >> 4;
	uint8_t const timeReferenceStatus = data[0] & 0x0FU;
	ServiceTypeNum const serviceType = data[1];
	MessageTypeNum const messageType = data[2];
	uint16_t const messageTypeCounter = (data[3] << 8) | data[4];
	ApplicationProcessUserId const destinationId = (data[5] << 8) | data[6];

	if (not ErrorHandler::assertRequest(pusVersion == 2U, message, ErrorHandler::UnacceptableMessage)) {
		return etl::unexpected<ErrorHandler::AcceptanceErrorType>(ErrorHandler::UnacceptableMessage);
	}

	// Remove the length of the header
	length -= ECSSSecondaryTMHeaderSize;

	// Copy the data to the message
	message.serviceType = serviceType;
	message.messageType = messageType;
	message.messageTypeCounter = messageTypeCounter;
	message.timeReferenceStatus = timeReferenceStatus;
	message.destinationId = destinationId;
	std::copy(data + ECSSSecondaryTMHeaderSize, data + ECSSSecondaryTMHeaderSize + length, message.data.begin());
	message.dataSize = length;

	return {};
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
