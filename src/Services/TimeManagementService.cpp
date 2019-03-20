#include "Services/TimeManagementService.hpp"

void TimeManagementService::cdsTimeReport(TimeAndDate &TimeInfo) {
	// TM[9,3] CDS time report

	Message timeReport = createTM(3);

	uint64_t timeFormat = TimeHelper::generateCDStimeFormat(TimeInfo);

	timeReport.appendHalfword(static_cast<uint16_t >(timeFormat >> 32));
	timeReport.appendWord(static_cast<uint32_t >(timeFormat));

	storeMessage(timeReport);
}

TimeAndDate TimeManagementService::cdsTimeRequest(Message &message) {
	// TC{9,128] CDS time request

	// check if we have the correct size of the data. The size should be 6 (48 bits)
	ErrorHandler::assertRequest(message.dataSize == 6, message,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	TimeAndDate timeInfo = TimeHelper::parseCDStimeFormat(message.data);

	return timeInfo;
}
