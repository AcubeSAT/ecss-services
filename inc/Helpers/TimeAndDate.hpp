#ifndef ECSS_SERVICES_TIMEANDDATE_HPP
#define ECSS_SERVICES_TIMEANDDATE_HPP

#include <cstdint>
#include "macros.hpp"

/**
 * A class that represents the time and date.
 *
 * @note
 * This class represents UTC date
 */
class TimeAndDate {
public:
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

	/**
	 * Assign the instances with the Unix epoch 1/1/1970 00:00:00
	 */
	TimeAndDate(); //

	/**
	 * @param year the year as it used in Gregorian calendar (the well known civil calendar). For
	 * example 2019
	 * @param month the month as it used in Gregorian calendar. There isn't 0 month so we are
	 * start counting from 1!
	 * @param day the day as it used in Gregorian calendar. There isn't 0 day so we are start
	 * counting from 1!
	 * @param hour UTC (Coordinated Universal Time) hour in 24 format
	 * @param minute UTC minutes
	 * @param second UTC seconds
	 */
	TimeAndDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t
	second);

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than \p Date
	 */
	bool operator<(const TimeAndDate &Date);

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than \p Date
	 */
	bool operator>(const TimeAndDate &Date);

	/**
 	 * Compare two timestamps.
 	 *
 	 * @param Date the date that will be compared with the pointer `this`
 	 * @return true if the pointer `this` is equal to \p Date
 	 */
	bool operator==(const TimeAndDate &Date);

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than or equal to \p Date
	 */
	bool operator<=(const TimeAndDate &Date);

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than or equal to \p Date
	 */
	bool operator>=(const TimeAndDate &Date);
};

#endif //ECSS_SERVICES_TIMEANDDATE_HPP
