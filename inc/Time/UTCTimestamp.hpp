#pragma once

#include <cstdint>
#include <etl/String.hpp>
#include "Time.hpp"

/**
 * A class that represents a UTC time and date according to ISO 8601
 *
 * This class contains a human-readable representation of a timestamp, accurate down to 1 second. It is not used
 * for timestamp storage in the satellite due to its high performance and memory cost, but it can be used for
 * debugging and logging purposes.
 *
 * @ingroup Time
 * @note
 * This class represents UTC (Coordinated Universal Time) date
 */
class UTCTimestamp {
public:
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

	/**
	 * Initialise a timestamp with the Unix epoch 1/1/1970 00:00:00
	 */
	UTCTimestamp();

	/**
	 * @todo Add support for leap seconds
	 * @todo Implement leap seconds as ST[20] parameter
	 * @param year the year as it used in Gregorian calendar
	 * @param month the month as it used in Gregorian calendar (1-12 inclusive)
	 * @param day the day as it used in Gregorian calendar (1-31 inclusive)
	 * @param hour UTC hour in 24-hour format
	 * @param minute UTC minutes
	 * @param second UTC seconds
	 */
	UTCTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	/**
	 * Add a duration to the timestamp
	 *
	 * @note Overflow checks are not performed.
	 * @tparam Duration A duration of type std::chrono::duration. You can use the default values offered by C++, or anything
	 * used by the TimeStamp class. Negative duration values are not supported.
	 */
	template<class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	void operator+=(const Duration& in) {
		using namespace std::chrono;
		using namespace Time;

		if (in < Duration::zero()) {
			ErrorHandler::reportInternalError(ErrorHandler::InvalidTimeStampInput);
			return;
		}

	    uint64_t seconds = duration_cast<duration<uint64_t>>(in).count();

		while (seconds >= (isLeapYear(year) ? 366L : 365L) * SecondsPerDay) {
			seconds -= (isLeapYear(year) ? 366L : 365L) * SecondsPerDay;
			year++;
		}

		while (seconds >= (daysOfMonth() * uint64_t{SecondsPerDay})) {
			seconds -= daysOfMonth() * uint64_t{SecondsPerDay};
		    month++;

			if (month > MonthsPerYear) {
				// Month overflow needs to be taken care here, so that daysOfMonth() knows
				// what month it is.
				month -= MonthsPerYear;
				year++;
			}
		}

		day += seconds / SecondsPerDay;
		seconds -= (seconds / SecondsPerDay) * SecondsPerDay;

		hour += seconds / SecondsPerHour;
		seconds -= (seconds / SecondsPerHour) * SecondsPerHour;

		minute += seconds / SecondsPerMinute;
		seconds -= (seconds / SecondsPerMinute) * SecondsPerMinute;

		second += seconds;

		repair();
	}

	/**
	 * @name Comparison operators
	 * @{
	 */
	bool operator<(const UTCTimestamp& Date) const;
	bool operator>(const UTCTimestamp& Date) const;
	bool operator==(const UTCTimestamp& Date) const;
	bool operator<=(const UTCTimestamp& Date) const;
	bool operator>=(const UTCTimestamp& Date) const;
	/**
	 * @}
	 */

private:
	/**
	 * Makes sure that all time fields are within their bounds
	 *
	 * For example, if `hours == 1, minutes == 63`, then this function will carry over the numbers so that
	 * `hours == 2, minutes == 3`.
	 *
	 * @note This performs max one propagation for every field.
	 * For example, if `hours == 1, minutes == 123`, then only the first 60 minutes will be carried over.
	 */
	void repair();

	/**
	 * Find the number of days within the current @ref month.
	 * Includes leap year calculation.
	 */
	uint8_t daysOfMonth() const {
		using namespace Time;

		uint8_t daysOfMonth = DaysOfMonth[month - 1];
		if (month == 2 && isLeapYear(year)) {
			daysOfMonth++;
		}

		return daysOfMonth;
	}
};
