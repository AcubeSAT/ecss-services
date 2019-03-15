#ifndef ECSS_SERVICES_TIMEANDDATE_HPP
#define ECSS_SERVICES_TIMEANDDATE_HPP

#include <cstdint>

/**
 * A class that represents the time and date.
 *
 * @notes
 * This struct is similar to the `struct tm` of <ctime> library but it is more embedded-friendly
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

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is equal to \p Date
	 */
	bool operator==(const TimeAndDate &Date);
};

#endif //ECSS_SERVICES_TIMEANDDATE_HPP
