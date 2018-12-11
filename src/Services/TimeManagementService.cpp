#include "Services/TimeManagementService.hpp"

void TimeManagementService::cucTimeReport(uint32_t seconds) {
	// TM[9,2] CUC time report

	Message timeReport = createTM(2);

	timeReport.appendByte(TimeHelper::implementCUCTimeFormat(seconds)); // append the P-field
	timeReport.appendWord(TimeHelper::implementCUCTimeFormat(seconds) >> 8); // append the T-field

	storeMessage(timeReport);
}
