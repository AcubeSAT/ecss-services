#include "Services/TimeManagementService.hpp"

void TimeManagementService::cdsTimeReport() {
	// TM[9,3] CDS time report

	Message timeReport = createTM(3);

	/**
	 * For the time being we will use C++ functions to get a time value, but this will change
	 * when the RTC will be implemented
	 */
	time_t currTime = time(nullptr); // seconds have passed since 00:00:00 GMT, Jan 1, 1970
	struct tm* timeInfo = gmtime(&currTime); // UTC time

	uint64_t timeFormat = TimeHelper::implementCDSTimeFormat(timeInfo); // store the return value

	timeReport.appendByte(static_cast<uint8_t >(timeFormat >> 32)); // append the first byte
	timeReport.appendWord(static_cast<uint32_t >(timeFormat)); // append the rest bytes(4 bytes)

	storeMessage(timeReport);
}

void TimeManagementService::parseTime(Message &messageTime) {

}
