#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TIME

#include "Services/TimeManagementService.hpp"

void TimeManagementService::cdsTimeReport(TimeAndDate& TimeInfo) {
	// TM[9,3] CDS time report

	Message timeReport = createTM(3);

	uint64_t timeFormat = TimeHelper::generateCDSTimeFormat(TimeInfo);

	timeReport.appendHalfword(static_cast<uint16_t>(timeFormat >> 32));
	timeReport.appendWord(static_cast<uint32_t>(timeFormat));

	storeMessage(timeReport);
}

TimeAndDate TimeManagementService::cdsTimeRequest(Message& message) {
	// TC[9,128] CDS time request
	message.assertTC(9, 128);

	// check if we have the correct size of the data. The size should be 6 (48 bits)
	ErrorHandler::assertRequest(message.dataSize == 6, message, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	TimeAndDate timeInfo = TimeHelper::parseCDStimeFormat(message.data);

	return timeInfo;
}

void TimeManagementService::execute(Message& message) {
	switch (message.messageType) {
		case 128:
			cdsTimeRequest(message); // TC[9,128]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}

#endif
