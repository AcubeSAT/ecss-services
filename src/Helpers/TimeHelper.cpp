#include "Helpers/TimeHelper.hpp"

bool TimeHelper::IsLeapYear(uint16_t year) {

	if (year % 4 != 0) return false;
	if (year % 100 != 0) return true;
	return (year % 400) == 0;
}

uint32_t TimeHelper::mkUTCtime(struct TimeAndDate &timeInfo) {

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

struct TimeAndDate TimeHelper::utcTime(uint32_t seconds) {

	struct TimeAndDate TimeInfo = {0};
	// Unix epoch 1/1/1970 00:00:00
	TimeInfo.year = 1970;
	TimeInfo.month = 1;
	TimeInfo.day = 1;
	TimeInfo.hour = 0;
	TimeInfo.minute = 0;
	TimeInfo.second = 0;

	// calculate years
	while (seconds >= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SecondsPerDay) {
		TimeInfo.year++;
		seconds -= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SecondsPerDay;
	}

	// calculate months
	uint8_t i = 0;
	while (seconds >= (DaysOfMonth[i] * SecondsPerDay)) {
		TimeInfo.month++;
		seconds -= (DaysOfMonth[i] * SecondsPerDay);
		i++;
		if (i == 1 && (seconds >= (IsLeapYear(TimeInfo.year) ? 29 : 28) * SecondsPerDay)) {
			TimeInfo.month++;
			seconds -= (IsLeapYear(TimeInfo.year) ? 29 : 28) * SecondsPerDay;
			i++;
		}
	}

	// calculate days
	while (seconds >= SecondsPerDay) {
		TimeInfo.day++;
		seconds -= SecondsPerDay;
	}

	// calculate hours
	while (seconds >= SecondsPerHour) {
		TimeInfo.hour++;
		seconds -= SecondsPerHour;
	}

	// calculate minutes
	while (seconds >= SecondsPerMinute) {
		TimeInfo.minute++;
		seconds -= SecondsPerMinute;
	}

	// calculate seconds
	TimeInfo.second = seconds;

	return TimeInfo;
}

uint64_t TimeHelper::implementCDSTimeFormat(struct TimeAndDate &TimeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */


	uint32_t seconds = Access.mkUTCtime(TimeInfo);

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

struct TimeAndDate TimeHelper::parseCDSTimeFormat(uint8_t *timeData, uint8_t length) {
	// check if we have the correct length of the packet data
	assertI(length != 48, ErrorHandler::InternalErrorType::UnknownInternalError);

	uint16_t elapsedDays = (static_cast<uint16_t >(timeData[0])) << 8 | static_cast<uint16_t >
	(timeData[1]);
	uint32_t msOfDay = (static_cast<uint32_t >(timeData[2])) << 24 |
	                   (static_cast<uint32_t >(timeData[3])) << 16 |
	                   (static_cast<uint32_t >(timeData[4])) << 8 |
	                   static_cast<uint32_t >(timeData[5]);

	uint32_t seconds = elapsedDays * 86400 + msOfDay / 1000;

	return Access.utcTime(seconds);
}
