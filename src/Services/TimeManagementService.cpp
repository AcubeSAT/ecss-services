#include "Services/TimeManagementService.hpp"

void TimeManagementService::cucTimeReport() {
	// TM[9,2] CUC time report

	Message timeReport = createTM(2);

	/**
	 * For the time being we will use C++ functions to get a time value, but this will change
	 * when the RTC will be implemented
	 */
	uint32_t seconds;
	seconds = time(nullptr); // seconds have passed since 00:00:00 GMT, Jan 1, 1970
	uint64_t timeFormat = TimeHelper::implementCUCTimeFormat(seconds); // store the return value

	timeReport.appendByte(timeFormat); // append the P-field
	timeReport.appendWord(timeFormat >> 8); // append the T-field

	storeMessage(timeReport);
}
