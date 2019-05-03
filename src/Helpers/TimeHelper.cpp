#include "Helpers/TimeHelper.hpp"

bool TimeHelper::IsLeapYear(uint16_t year) {
	if ((year % 4) != 0) {
		return false;
	}
	if ((year % 100) != 0) {
		return true;
	}
	return (year % 400) == 0;
}

uint32_t TimeHelper::utcToSeconds(TimeAndDate& TimeInfo) {
	// the date, that \p TimeInfo represents, should be greater than or equal to 1/1/2019 and the
	// date should be valid according to Gregorian calendar
	ASSERT_INTERNAL(TimeInfo.year >= 2019, ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL((1 <= TimeInfo.month) && (TimeInfo.month <= 12), ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL((1 <= TimeInfo.day) && (TimeInfo.day <= 31), ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL(TimeInfo.hour <= 24, ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL(TimeInfo.minute <= 60, ErrorHandler::InternalErrorType::InvalidDate);
	ASSERT_INTERNAL(TimeInfo.second <= 60, ErrorHandler::InternalErrorType::InvalidDate);

	uint32_t secs = 1546300800; // elapsed seconds from Unix epoch until 1/1/2019 00:00:00 (UTC)
	for (uint16_t y = 2019; y < TimeInfo.year; ++y) {
		secs += (IsLeapYear(y) ? 366 : 365) * SECONDS_PER_DAY;
	}
	for (uint16_t m = 1; m < TimeInfo.month; ++m) {
		secs += DaysOfMonth[m - 1u] * SECONDS_PER_DAY;
		if ((m == 2u) && IsLeapYear(TimeInfo.year)) {
			secs += SECONDS_PER_DAY;
		}
	}
	secs += (TimeInfo.day - 1) * SECONDS_PER_DAY;
	secs += TimeInfo.hour * SECONDS_PER_HOUR;
	secs += TimeInfo.minute * SECONDS_PER_MINUTE;
	secs += TimeInfo.second;
	return secs;
}

struct TimeAndDate TimeHelper::secondsToUTC(uint32_t seconds) {
	// elapsed seconds should be between dates, that are after 1/1/2019 and Unix epoch
	ASSERT_INTERNAL(seconds >= 1546300800, ErrorHandler::InternalErrorType::InvalidDate);

	seconds -= 1546300800; // elapsed seconds from Unix epoch until 1/1/2019 00:00:00 (UTC)
	TimeAndDate TimeInfo;
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
		if ((i == 1u) && IsLeapYear(TimeInfo.year)) {
			if (seconds <= (28 * SECONDS_PER_DAY)) {
				break;
			}
			TimeInfo.month++;
			seconds -= 29 * SECONDS_PER_DAY;
			i++;
		}
	}

	// calculate days
	TimeInfo.day = seconds / SECONDS_PER_DAY;
	seconds -= TimeInfo.day * SECONDS_PER_DAY;
	TimeInfo.day++; // add 1 day because we start count from 1 January (and not 0 January!)

	// calculate hours
	TimeInfo.hour = seconds / SECONDS_PER_HOUR;
	seconds -= TimeInfo.hour * SECONDS_PER_HOUR;

	// calculate minutes
	TimeInfo.minute = seconds / SECONDS_PER_MINUTE;
	seconds -= TimeInfo.minute * SECONDS_PER_MINUTE;

	// calculate seconds
	TimeInfo.second = seconds;

	return TimeInfo;
}

uint64_t TimeHelper::generateCDSTimeFormat(TimeAndDate& TimeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */

	uint32_t seconds = utcToSeconds(TimeInfo);

	/**
	 * The `DAY` segment, 16 bits as defined from standard. Actually the days passed since Unix
	 * epoch
	 */
	auto elapsedDays = static_cast<uint16_t>(seconds / SECONDS_PER_DAY);

	/**
	 * The `ms of day` segment, 32 bits as defined in standard. The `ms of the day` and DAY`
	 * should give the time passed since Unix epoch
	 */
	auto msOfDay = static_cast<uint32_t>((seconds % SECONDS_PER_DAY) * 1000);

	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32) | msOfDay;

	return timeFormat;
}

TimeAndDate TimeHelper::parseCDStimeFormat(const uint8_t* data) {
	uint16_t elapsedDays = ((static_cast<uint16_t>(data[0])) << 8) | (static_cast<uint16_t>(data[1]));
	uint32_t msOfDay = ((static_cast<uint32_t>(data[2])) << 24) | ((static_cast<uint32_t>(data[3]))) << 16 |
	                   ((static_cast<uint32_t>(data[4]))) << 8 | (static_cast<uint32_t>(data[5]));

	uint32_t seconds = (elapsedDays * SECONDS_PER_DAY) + (msOfDay / 1000u);

	return secondsToUTC(seconds);
}

uint32_t TimeHelper::generateCUCTimeFormat(struct TimeAndDate& TimeInfo) {
	return (utcToSeconds(TimeInfo) + LEAP_SECONDS);
}

TimeAndDate TimeHelper::parseCUCTimeFormat(const uint8_t* data) {
	uint32_t seconds = ((static_cast<uint32_t>(data[0])) << 24) | ((static_cast<uint32_t>(data[1]))) << 16 |
	                   ((static_cast<uint32_t>(data[2]))) << 8 | (static_cast<uint32_t>(data[3]));
	seconds -= LEAP_SECONDS;

	return secondsToUTC(seconds);
}
