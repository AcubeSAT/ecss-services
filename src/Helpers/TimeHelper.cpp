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

uint32_t TimeHelper::utcToSeconds(const TimeAndDate& TimeInfo) {
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
		secs += (IsLeapYear(y) ? 366 : 365) * SecondsPerDay;
	}
	for (uint16_t m = 1; m < TimeInfo.month; ++m) {
		secs += DaysOfMonth[m - 1U] * SecondsPerDay;
		if ((m == 2U) && IsLeapYear(TimeInfo.year)) {
			secs += SecondsPerDay;
		}
	}
	secs += (TimeInfo.day - 1) * SecondsPerDay;
	secs += TimeInfo.hour * SecondsPerHour;
	secs += TimeInfo.minute * SecondsPerMinute;
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
	while (seconds >= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SecondsPerDay) {
		seconds -= (IsLeapYear(TimeInfo.year) ? 366 : 365) * SecondsPerDay;
		TimeInfo.year++;
	}

	// calculate months
	uint8_t i = 0;
	while (seconds >= (DaysOfMonth[i] * SecondsPerDay)) {
		TimeInfo.month++;
		seconds -= (DaysOfMonth[i] * SecondsPerDay);
		i++;
		if ((i == 1U) && IsLeapYear(TimeInfo.year)) {
			if (seconds <= (28 * SecondsPerDay)) {
				break;
			}
			TimeInfo.month++;
			seconds -= 29 * SecondsPerDay;
			i++;
		}
	}

	// calculate days
	TimeInfo.day = seconds / SecondsPerDay;
	seconds -= TimeInfo.day * SecondsPerDay;
	TimeInfo.day++; // add 1 day because we start count from 1 January (and not 0 January!)

	// calculate hours
	TimeInfo.hour = seconds / SecondsPerHour;
	seconds -= TimeInfo.hour * SecondsPerHour;

	// calculate minutes
	TimeInfo.minute = seconds / SecondsPerMinute;
	seconds -= TimeInfo.minute * SecondsPerMinute;

	// calculate seconds
	TimeInfo.second = seconds;

	return TimeInfo;
}

uint64_t TimeHelper::generateCDSTimeFormat(const TimeAndDate& TimeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */

	uint32_t seconds = utcToSeconds(TimeInfo);

	/**
	 * The `DAY` segment, 16 bits as defined from standard. Actually the days passed since Unix
	 * epoch
	 */
	auto elapsedDays = static_cast<uint16_t>(seconds / SecondsPerDay);

	/**
	 * The `ms of day` segment, 32 bits as defined in standard. The `ms of the day` and DAY`
	 * should give the time passed since Unix epoch
	 */
	auto msOfDay = static_cast<uint32_t>((seconds % SecondsPerDay) * 1000);

	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32) | msOfDay;

	return timeFormat;
}

TimeAndDate TimeHelper::parseCDStimeFormat(const uint8_t* data) {
	uint16_t elapsedDays = ((static_cast<uint16_t>(data[0])) << 8) | (static_cast<uint16_t>(data[1]));
	uint32_t msOfDay = ((static_cast<uint32_t>(data[2])) << 24) | ((static_cast<uint32_t>(data[3]))) << 16 |
	                   ((static_cast<uint32_t>(data[4]))) << 8 | (static_cast<uint32_t>(data[5]));

	uint32_t seconds = (elapsedDays * SecondsPerDay) + (msOfDay / 1000U);

	return secondsToUTC(seconds);
}

uint32_t TimeHelper::generateCUCTimeFormat(const struct TimeAndDate& TimeInfo) {
	return (utcToSeconds(TimeInfo) + LeapSeconds);
}

TimeAndDate TimeHelper::parseCUCTimeFormat(const uint8_t* data) {
	uint32_t seconds = ((static_cast<uint32_t>(data[0])) << 24) | ((static_cast<uint32_t>(data[1]))) << 16 |
	                   ((static_cast<uint32_t>(data[2]))) << 8 | (static_cast<uint32_t>(data[3]));
	seconds -= LeapSeconds;

	return secondsToUTC(seconds);
}
