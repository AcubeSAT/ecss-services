#include "Helpers/TimeHelper.hpp"

bool TimeHelper::IsLeapYear(uint16_t year) {

	if (year % 4 != 0) return false;
	if (year % 100 != 0) return true;
	return (year % 400) == 0;
}

uint32_t TimeHelper::mkgmtime(struct TimeAndDate &timeInfo) {

	uint32_t secs = 0;
	for (uint16_t y = 1970; y < timeInfo.year; ++y)
		secs += (IsLeapYear(y) ? 366 : 365) * SecondsPerDay;
	for (uint16_t m = 1; m < timeInfo.month; ++m) {
		secs += DaysOfMonth[m - 1] * SecondsPerDay;
		if (m == 2 && IsLeapYear(timeInfo.year))
			secs += SecondsPerDay;
	}
	secs += (timeInfo.day - 1) * SecondsPerDay;
	secs += timeInfo.hour * SecondsPerHour;
	secs += timeInfo.minute * SecondsPerMinute;
	secs += timeInfo.second;
	return secs;
}

uint64_t TimeHelper::implementCDSTimeFormat(struct TimeAndDate &timeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */


	uint32_t seconds = mkgmtimeAccess.mkgmtime(timeInfo);

	/**
	 * The `DAY` segment, 16 bits as defined from standard. Actually the days passed from an
	 * Agency-defined epoch,that it will be 1 January 1970(1/1/1970) 00:00:00(hours:minutes:seconds)
	 * This epoch is configured from the current implementation, using mktime() function
	 */
	uint16_t elapsedDays = static_cast<uint16_t>(seconds / 86400);

	/**
	 * The `ms of day` segment, 32 bits as defined in standard. The `ms of the day` and DAY`
	 * should give the time passed from the defined epoch (1/1/1970)
	 */
	uint32_t msOfDay = static_cast<uint32_t >((seconds % 86400) * 1000);

	/**
	 * Define CDS time format
	 *
	 * Notes:
	 * Only the 48 bits of the 64 will be used for the timeFormat
	 *
	 * Shift operators have high priority. That's why we should do a type-casting first so we
	 * don't lose valuable bits
	*/
	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);


	return timeFormat;
}
