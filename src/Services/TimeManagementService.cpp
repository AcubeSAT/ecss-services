#include "Services/TimeManagementService.hpp"

void TimeManagementService::cdsTimeReport(struct TimeAndDate &TimeInfo) {
	// TM[9,3] CDS time report

	Message timeReport = createTM(3);

	uint64_t timeFormat = TimeHelper::implementCDSTimeFormat(TimeInfo);

	timeReport.appendHalfword(static_cast<uint16_t >(timeFormat >> 32));
	timeReport.appendWord(static_cast<uint32_t >(timeFormat));

	storeMessage(timeReport);
}

struct TimeAndDate TimeManagementService::cdsTimeRequest(Message &message) {
	// TC{9,128] CDS time request

	struct TimeAndDate timeInfo = TimeHelper::parseCDSTimeFormat(message.data, 48);

	return timeInfo;
}

