#include "Helpers/TimeHelper.hpp"

bool TimeHelper::IsLeapYear(uint16_t year) {
	if (year % 4 != 0) {
		return false;
	}
	if (year % 100 != 0) {
		return true;
	}
	return (year % 400) == 0;
}

uint32_t TimeHelper::mkUTCtime(struct TimeAndDate &TimeInfo) {
	uint32_t secs = 1546300800; // elapsed seconds from Unix epoch until 1/1/2019 00:00:00(UTC date)
	for (uint16_t y = 2019; y < TimeInfo.year; ++y) {
		secs += (IsLeapYear(y) ? 366 : 365) * SECONDS_PER_DAY;
	}
	for (uint16_t m = 1; m < TimeInfo.month; ++m) {
		secs += DaysOfMonth[m - 1] * SECONDS_PER_DAY;
		if (m == 2 && IsLeapYear(TimeInfo.year)) {
			secs += SECONDS_PER_DAY;
		}
	}
	secs += (TimeInfo.day - 1) * SECONDS_PER_DAY;
	secs += TimeInfo.hour * SECONDS_PER_HOUR;
	secs += TimeInfo.minute * SECONDS_PER_MINUTE;
	secs += TimeInfo.second;
	return secs;
}

struct TimeAndDate TimeHelper::utcTime(uint32_t seconds) {
	seconds -= 1546300800; // elapsed seconds from Unix epoch until 1/1/2019 00:00:00(UTC date)
	struct TimeAndDate TimeInfo = {0};
	TimeInfo.year = 2019;
	TimeInfo.month = 1;
	TimeInfo.day = 0;
	TimeInfo.hour = 0;
	TimeInfo.minute = 0;
	TimeInfo.second = 0;

	// calculate years
	while (seconds >= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SECONDS_PER_DAY) {
		seconds -= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SECONDS_PER_DAY;
		TimeInfo.year++;
	}

	// calculate months
	uint8_t i = 0;
	while (seconds >= (DaysOfMonth[i] * SECONDS_PER_DAY)) {
		TimeInfo.month++;
		seconds -= (DaysOfMonth[i] * SECONDS_PER_DAY);
		i++;
		if (i == 1 && IsLeapYear(TimeInfo.year)) {
			if (seconds <= (28 * SECONDS_PER_DAY)) {
				break;
			}
			TimeInfo.month++;
			seconds -= 29 * SECONDS_PER_DAY;
			i++;
		}
	}

	// calculate days
	TimeInfo.day = seconds/SECONDS_PER_DAY;
	seconds -= TimeInfo.day * SECONDS_PER_DAY;
	TimeInfo.day++; // add 1 day because we start count from 1 January(and not 0 January!)

	// calculate hours
	TimeInfo.hour = seconds/SECONDS_PER_HOUR;
	seconds -= TimeInfo.hour * SECONDS_PER_HOUR;

	// calculate minutes
	TimeInfo.minute = seconds/SECONDS_PER_MINUTE;
	seconds -= TimeInfo.minute * SECONDS_PER_MINUTE;

	// calculate seconds
	TimeInfo.second = seconds;

	return TimeInfo;
}

uint64_t TimeHelper::generateCDStimeFormat(struct TimeAndDate &TimeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */


	uint32_t seconds = mkUTCtime(TimeInfo);

	/**
	 * The `DAY` segment, 16 bits as defined from standard. Actually the days passed since Unix
	 * epoch
	 */
	auto elapsedDays = static_cast<uint16_t>(seconds / SECONDS_PER_DAY);

	/**
	 * The `ms of day` segment, 32 bits as defined in standard. The `ms of the day` and DAY`
	 * should give the time passed since Unix epoch
	 */
	auto msOfDay = static_cast<uint32_t >((seconds % SECONDS_PER_DAY) * 1000);

	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);

	return timeFormat;
}

struct TimeAndDate TimeHelper::parseCDStimeFormat(const uint8_t *data) {
	uint16_t elapsedDays = (static_cast<uint16_t >(data[0])) << 8 | static_cast<uint16_t >
	(data[1]);
	uint32_t msOfDay = (static_cast<uint32_t >(data[2])) << 24 |
	                   (static_cast<uint32_t >(data[3])) << 16 |
	                   (static_cast<uint32_t >(data[4])) << 8 |
	                   static_cast<uint32_t >(data[5]);

	uint32_t seconds = elapsedDays * SECONDS_PER_DAY + msOfDay / 1000;

	return utcTime(seconds);
}
