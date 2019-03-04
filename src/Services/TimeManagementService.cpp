#include "Services/TimeManagementService.hpp"

void TimeManagementService::cdsTimeReport(struct TimeAndDate &timeInfo) {
	// TM[9,3] CDS time report

	Message timeReport = createTM(3);

	uint64_t timeFormat = TimeHelper::implementCDSTimeFormat(timeInfo); 

	timeReport.appendByte(static_cast<uint8_t >(timeFormat >> 32)); // append the first byte
	timeReport.appendWord(static_cast<uint32_t >(timeFormat)); // append the rest 4 bytes

	storeMessage(timeReport);
}

void TimeManagementService::parseTime(uint8_t *timeData, uint8_t lenght) {

}
