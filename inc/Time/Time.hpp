#ifndef ECSS_TIMEHPP
#define ECSS_TIMEHPP

#include <chrono>
#include <cstdint>
#include "ErrorHandler.hpp"
#include "etl/String.hpp"
#include "macros.hpp"

/**
 * @defgroup Time Time
 *
 * Group of classes, functions and variables to represent and operate on time
 *
 * The implementation of the ECSS services requires storing and handling timestamps quite often. However, in an embedded
 * system with mission-critical tasks, it is wide to avoid the complexity of a full time management system, as the one
 * that comes with Linux or contains [large timezone databases](https://en.wikipedia.org/wiki/Tz_database).
 *
 * This repository provides a set of classes and utilities that allow handling of on-board spacecraft time. Most formats
 * are compatible to the [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf) specification, and are
 * customisable to support different precisions, lengths and durations. Additionally, a number of formats and headers
 * are provided so that timestamps can be transmitted and received from the Ground Station.
 *
 * The base timestamp class representing an instant in time is @ref TimeStamp. This is used as the internal
 * representation of all moments in this repository. Additional formats are provided for convenience and compatibility
 * with other systems:
 *  - @ref UTCTimestamp
 *
 * You can convert between different types of timestamp, either by using constructors, or conversion functions
 * defined in each timestamp class.
 *
 * Note that you should try sticking to the default @ref TimeStamp class unless needed, as it provides the best
 * precision and performance for the least amount of memory, and is directly compatible with internal timestamps
 * used, without loss of precision.
 *
 * @section Epoch Epoch
 *
 * Internal timestamp representations can use a single point in time referring to $t=0$. This point is referred to as
 * the **Epoch** and is stored in @ref Time::Epoch. CSSDS suggests using _1 January 1958_ as the Epoch. UNIX uses _1
 * January 1970_. In this repository however, each implementor can choose a different epoch (e.g. the start of the
 * mission).
 *
 * @section DevUtils Developer utilities
 * Timestamp classes are designed to make the developer's life easier.
 *
 * You can easily compare timestamps without having to call any other functions:
 * @code
 * if (timestamp1 < timestamp2)
 * @endcode
 *
 * If you are building on x86, you can directly print UTC timestamps:
 * @code
 * std::cout << utcTimestamp << std::endl;
 * @endcode
 *
 * @section UTC UTC and Leap seconds
 * All internal timestamps are represented in the GMT+00:00 timezone due to the high expense of timezone
 * calculations.
 *
 * This implementation uses **TAI (International Atomic Time)** instead of UTC (Coordinated Universal Time).
 * [TAI](https://en.wikipedia.org/wiki/International_Atomic_Time) is equivalent to UTC, with the exception of leap
 * seconds, which are occasionally added to match terrestrial and astronomical time. As leap seconds are added after
 * human intervention to UTC, it is impossible for a satellite to know the exact UTC time without ground station
 * intervention.
 */

/**
 * An armada of utilities regarding timekeeping, timestamps and conversion between different internal and string
 * time formats. This file implements [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf).
 *
 * @ingroup Time
 * @author Baptiste Fournier
 */
namespace Time {
	inline constexpr uint8_t SecondsPerMinute = 60;
	inline constexpr uint16_t SecondsPerHour = 3600;
	inline constexpr uint32_t SecondsPerDay = 86400;
	inline constexpr uint8_t MonthsPerYear = 12;
	static constexpr uint8_t DaysOfMonth[MonthsPerYear] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	/**
	 * Number of bytes used for the basic time units of the CUC header for this mission
	 */
	inline constexpr uint8_t CUCSecondsBytes = 4;

	/**
	 * Number of bytes used for the fractional time units of the CUC header for this mission
	 */
	inline constexpr uint8_t CUCFractionalBytes = 0;

	/**
	 * The system epoch (clock measurement starting time).
	 * All timestamps emitted by the ECSS services will show the elapsed time (seconds, days etc.) from this epoch.
 	*/
	inline constexpr struct {
		uint16_t year;
		uint8_t month;
		uint8_t day;
	} Epoch{
	    2020,
	    1,
	    1,
	};

	/**
	 * Number of seconds elapsed between the UNIX epoch (1 January 1970) and the system epoch.
	 *
	 * The system epoch is defined by @ref Epoch.
	 * This constant is used for conversion between Unix and other timestamps.
	 * Leap seconds are not taken into account here.
	 *
	 * @warning This value MUST be updated after every change of the system @ref Epoch. You can use utilities such as
	 * https://www.unixtimestamp.com/ to obtain a correct result.
	 */
	inline constexpr uint32_t EpochSecondsFromUnix = 1577836800;

	/**
	 * The maximum theoretical size in bytes of a CUC timestamp, including headers (P-field and T-field)
	 */
	inline constexpr uint8_t CUCTimestampMaximumSize = 9;

	static_assert(Epoch.year >= 2019);
	static_assert(Epoch.month < 11 && Epoch.month >= 0);
	static_assert(Epoch.day < DaysOfMonth[Epoch.month]);

	/**
	 * Builds the short P-field of the CUC (CCSDS Unsegmented Time Code) format, as defined in CCSDS 301.0-B-4.
	 *
	 * The short P-field contains only one byte. It is used when many octets are used to represent the basic or fractional
	 * time units.
	 *
	 * @see CCSDS 301.0-B-4, Section 3.2.2
	 * @tparam secondsBytes The number of octets used to represent the basic time units
	 * @tparam fractionalBytes The number of octets used to represent the fractional time units
	 * @return A single byte, representing the P-field contents
	 */
	template <int secondsBytes, int fractionalBytes>
	inline constexpr uint8_t buildShortCUCHeader() {
		static_assert(secondsBytes <= 4, "Use buildLongCUCHeader instead");
		static_assert(fractionalBytes <= 3, "Use buildLongCUCHeader instead");

		uint8_t header = 0;

		// P-Field extension is 0, CUC header is not extended
		header += 0;

		// We are using a custom TAI epoch ("agency-defined epoch")
		header <<= 3U;
		header += 0b010;

		// Number of bytes in the basic time unit
		header <<= 2U;
		header += secondsBytes - 1;

		// Number of bytes in the fractional unit
		header <<= 2U;
		header += fractionalBytes;

		return header;
	}

	/**
	 * Builds the long P-field of the CUC (CCSDS Unsegmented Time Code) format, as defined in CCSDS 301.0-B-4.
	 *
	 * The long P-field contains two bytes. The 2nd byte is used to define the size of the additional octets added to the
	 * timestamp, which could not fit in a short P-field.
	 *
	 * @see CCSDS 301.0-B-4, Section 3.2.2
	 * @tparam secondsBytes The number of octets used to represent the basic time units
	 * @tparam fractionalBytes The number of octets used to represent the fractional time units
	 * @return Two bytes, representing the P-field contents
	 */
	template <int secondsBytes, int fractionalBytes>
	inline constexpr uint16_t buildLongCUCHeader() {
		// cppcheck-suppress redundantCondition
		static_assert(secondsBytes > 4 || fractionalBytes > 3, "Use buildShortCUCHeader instead");
		static_assert(secondsBytes <= 7, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");
		static_assert(fractionalBytes <= 6, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");

		uint16_t header = 0;

		uint8_t octet1secondsBytes = std::min(4, secondsBytes);
		uint8_t octet2secondsBytes = secondsBytes - octet1secondsBytes;

		uint8_t octet1fractionalBytes = std::min(3, fractionalBytes);
		uint8_t octet2fractionalBytes = fractionalBytes - octet1fractionalBytes;

		// P-Field extension is 1, CUC header is extended
		header += 1;

		// We are using custom a TAI epoch
		header <<= 3U;
		header += 0b010;

		// Number of bytes in the basic time unit
		header <<= 2U;
		header += octet1secondsBytes - 1;

		// Number of bytes in the fractional unit
		header <<= 2U;
		header += octet1fractionalBytes;

		// P-Field extension is 1, CUC header was extended
		header <<= 1U;
		header += 1;

		// Number of bytes in the extended basic time unit
		header <<= 2U;
		header += octet2secondsBytes;

		// Number of bytes in the extended fractional unit
		header <<= 3U;
		header += octet2fractionalBytes;

		// Last 3 LSB are reserved for custom mission use
		header <<= 2U;
		header += 0;

		return header;
	}

	/**
	 * Builds the entire P-field of the CUC (CCSDS Unsegmented Time Code) format, as defined in CCSDS 301.0-B-4.
	 *
	 * The P-field contains the metadata of the timestamp, including information about its size and epoch. This function
	 * is implemented for arbitrary sizes (_octet count_) for the basic and fractional time units.
	 *
	 * The following options cannot be changed:
	 *   - Time-code identification is set to an _agency-defined epoch_. This is represented by @ref Epoch.
	 *   - Bits 6-7 of octet 2 (_reserved_) are set to `0`
	 *
	 * @note The P-field (header) does not contain the timestamp information, but only the description of the timestamp's
	 * structure. It may be entirely omitted if the structure is known beforehand.
	 *
	 * @see CCSDS 301.0-B-4, Section 3.2.2
	 * @tparam T An arbitrary `uint` return type of the header
	 * @tparam secondsBytes The number of octets used to represent the basic time units
	 * @tparam fractionalBytes The number of octets used to represent the fractional time units
	 * @return One or two bytes representing the header
	 */
	template <typename T, int secondsBytes, int fractionalBytes>
	inline constexpr T buildCUCHeader() {
		static_assert((secondsBytes + fractionalBytes) <= 8, "Complete arbitrary precision not supported");
		// cppcheck-suppress syntaxError
		// cppcheck-suppress redundantCondition
		if constexpr (secondsBytes <= 4 && fractionalBytes <= 3) {
			return buildShortCUCHeader<secondsBytes, fractionalBytes>();
		} else {
			return buildLongCUCHeader<secondsBytes, fractionalBytes>();
		}
	}

	/**
	* Returns whether a year is a leap year according to the Gregorian calendar
	*/
	constexpr bool isLeapYear(uint16_t year) {
		if ((year % 4) != 0) {
			return false;
		}
		if ((year % 100) != 0) {
			return true;
		}
		return (year % 400) == 0;
	}

	/**
 	* A time shift for scheduled activities measured in seconds
 	*/
	typedef int64_t RelativeTime;

	/**
	 * is_duration definition to check if a variable is std::chrono::duration
	 */
	template <typename T>
	struct is_duration
	    : std::false_type {};

	/**
	 * is_duration definition to check if a variable is std::chrono::duration
	 */
	template <typename Rep, typename Period>
	struct is_duration<std::chrono::duration<Rep, Period>>
	    : std::true_type {};

	/**
	 * True if T is std::chrono::duration, false if not
	 */
	template <class T>
	inline constexpr bool is_duration_v = is_duration<T>::value;
} // namespace Time

#endif
