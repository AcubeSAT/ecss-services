#include <cstdint>
#include <Message.hpp>
#include "macros.hpp"
#include "etl/String.hpp"

// SEE CCSDS 301.0-B-4

/**
 * An armada of utilities regarding timekeeping, timestamps and conversion between different internal and string
 * time formats. This file implements [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf).
 *
 * @author Baptiste Fournier
 */
namespace Time
{

///@{
inline constexpr uint8_t SECONDS_PER_MINUTE = 60;
inline constexpr uint16_t SECONDS_PER_HOUR = 3600;
inline constexpr uint32_t SECONDS_PER_DAY = 86400;
static constexpr uint8_t DAYSOFMONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
///@}

///@{
/** I have no idea what this means */
inline constexpr uint8_t ACUBESAT_CUC_SECONDS_COUNTER_BYTES = 2;
inline constexpr uint8_t ACUBESAT_CUC_FRACTIONAL_COUNTER_BYTES = 2;
///@}

/** The AcubeSAT epoch (clock measurement starting time) */
inline constexpr uint16_t ACUBESAT_EPOCH_YEAR = 2019;
/**
 * Starts counting from 0-11.
 */
inline constexpr uint8_t ACUBESAT_EPOCH_MONTH = 1;
/**
 * Counting between 0-30.
 */
inline constexpr uint8_t ACUBESAT_EPOCH_DAY = 1;

/**
 * Number of seconds elapsed between the UNIX epoch (1 January 1970) and the AcubeSAT epoch.
 *
 * The AcubeSAT epoch is defined by @ref ACUBESAT_EPOCH_YEAR, @ref ACUBESAT_EPOCH_MONTH, @ref ACUBESAT_EPOCH_DAY.
 * This constant is used for conversion between Unix and other timestamps.
 * Leap seconds are not taken into account here.
 * You can use a utility such as .
 *
 * @warning This value MUST be updated after every change of the AcubeSAT epoch. You can use utilities such as
 * https://www.unixtimestamp.com/ to obtain a correct result.
 *
 * @todo Update AcubeSAT epoch to 2020.01.01.
 */
inline constexpr uint32_t UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS = 1546300800;

inline constexpr uint8_t MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP = 9;

static_assert(ACUBESAT_EPOCH_YEAR >= 2019);
static_assert(ACUBESAT_EPOCH_MONTH < 11 && ACUBESAT_EPOCH_MONTH >= 0);
static_assert(ACUBESAT_EPOCH_DAY < DAYSOFMONTH[ACUBESAT_EPOCH_MONTH]);

/**
 * Builds the short P-field of the CUC (CCSDS Unsegmented Time Code) format, as defined in CCSDS 301.0-B-4.
 *
 * The short P-field contains only one byte. It is used when many octets are used to represent the basic or fractional
 * time units.
 *
 * @see CCSDS 301.0-B-4, Section 3.2.2
 * @tparam seconds_counter_bytes The number of octets used to represent the basic time units
 * @tparam fractional_counter_bytes The number of octets used to represent the fractional time units
 * @return A single byte, representing the P-field contents
 */
template <int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr uint8_t build_short_CUC_header() {
	static_assert(seconds_counter_bytes <= 4, "Use build_long_CUC_header instead");
	static_assert(fractional_counter_bytes <= 3, "Use build_long_CUC_header instead");

	uint8_t header = 0;

	// P-Field extension is 0, CUC header is not extended
	header += 0;

	// AcubeSAT is using custom TAI epoch ("agency-defined epoch")
	header <<= 3U;
	header += 0b010;

	// Number of bytes in the basic time unit
	header <<= 2U;
	header += seconds_counter_bytes - 1;

	// Number of bytes in the fractional unit
	header <<= 2U;
	header += fractional_counter_bytes;

	return header;
}

/**
 * Builds the long P-field of the CUC (CCSDS Unsegmented Time Code) format, as defined in CCSDS 301.0-B-4.
 *
 * The long P-field contains two bytes. The 2nd byte is used to define the size of the additional octets added to the
 * timestamp, which could not fit in a short P-field.
 *
 * @see CCSDS 301.0-B-4, Section 3.2.2
 * @tparam seconds_counter_bytes The number of octets used to represent the basic time units
 * @tparam fractional_counter_bytes The number of octets used to represent the fractional time units
 * @return Two bytes, representing the P-field contents
 */
template <int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr uint16_t build_long_CUC_header() {
	// cppcheck-suppress redundantCondition
	static_assert(seconds_counter_bytes > 4 || fractional_counter_bytes > 3, "Use build_short_CUC_header instead");
	static_assert(seconds_counter_bytes <= 7,
	              "Number of bytes for seconds over maximum number of octets allowed by CCSDS");
	static_assert(fractional_counter_bytes <= 6,
	              "Number of bytes for seconds over maximum number of octets allowed by CCSDS");

	uint16_t header = 0;

	uint8_t first_octet_number_of_seconds_bytes = std::min(4, seconds_counter_bytes);
	uint8_t second_octet_number_of_seconds_bytes = seconds_counter_bytes - first_octet_number_of_seconds_bytes;

	uint8_t first_octet_number_of_fractional_bytes = std::min(3, fractional_counter_bytes);
	uint8_t second_octet_number_of_fractional_bytes = fractional_counter_bytes - first_octet_number_of_fractional_bytes;

	// P-Field extension is 1, CUC header is extended
	header += 1;

	// AcubeSAT is using custom TAI epoch at 01 Jan 2020
	header <<= 3U;
	header += 0b010;

	// // Number of bytes in the basic time unit
	header <<= 2U;
	header += first_octet_number_of_seconds_bytes - 1;

	// Number of bytes in the fractional unit
	header <<= 2U;
	header += first_octet_number_of_fractional_bytes;

	// P-Field extension is 1, CUC header was extended
	header <<= 1U;
	header += 1;

	// Number of bytes in the extended basic time unit
	header <<= 2U;
	header += second_octet_number_of_seconds_bytes;

	// Number of bytes in the extended fractional unit
	header <<= 2U;
	header += second_octet_number_of_fractional_bytes;

	// Last 3 LSB are reserved for custom mission use
	header <<= 3U;
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
 *   - Time-code identification is set to an _agency-defined epoch_. This is represented by @ref ACUBESAT_EPOCH_YEAR.
 *   - Bits 6-7 of octet 2 (_reserved_) are set to `0`
 *
 * @note The P-field (header) does not contain the timestamp information, but only the description of the timestamp's
 * structure. It may be entirely omitted if the structure is known beforehand.
 *
 * @see CCSDS 301.0-B-4, Section 3.2.2
 * @tparam T An arbitrary `uint` return type of the header
 * @tparam seconds_counter_bytes The number of octets used to represent the basic time units
 * @tparam fractional_counter_bytes The number of octets used to represent the fractional time units
 * @return One or two bytes representing the header
 */
template <typename T, int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr T build_CUC_header() {
	// TODO: Gitlab issue #106
	static_assert((seconds_counter_bytes + fractional_counter_bytes) <= 8,
	              "Complete arbitrary precision not supported");
	// cppcheck-suppress syntaxError
	// cppcheck-suppress redundantCondition
	if constexpr (seconds_counter_bytes <= 4 &&
	              fractional_counter_bytes <= 3) // if constexpr not supported yet in cppcheck
		return build_short_CUC_header<seconds_counter_bytes, fractional_counter_bytes>();
	else
		return build_long_CUC_header<seconds_counter_bytes, fractional_counter_bytes>();
}

/**
 * Returns whether a year is a leap year according to the Gregorian calendar
 */
bool is_leap_year(uint16_t year);

} // namespace Time

/**
 * A class that represents a UTC time and date according to ISO 8601
 *
 * This class contains a human-readable representation of a timestamp, accurate down to 1 second. It is not used
 * for timestamp storage in the satellite due to its high performance and memory cost, but it can be used for
 * debugging and logging purposes.
 *
 * @note
 * This class represents UTC (Coordinated Universal Time) date
 */
class UTC_Timestamp {
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
	UTC_Timestamp();

	/**
	 *
	 * @todo See if this implements leap seconds
	 * @todo Implement leap seconds as ST[20] parameter
	 * @param year the year as it used in Gregorian calendar
	 * @param month the month as it used in Gregorian calendar (1-12 inclusive)
	 * @param day the day as it used in Gregorian calendar (1-31 inclusive)
	 * @param hour UTC hour in 24-hour format
	 * @param minute UTC minutes
	 * @param second UTC seconds
	 */
	UTC_Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	/**
	 * @param text_timestamp the timestamp to parse into a UTC date
	 * @todo Too expensive to implement (?). It is better to remove this and open it as another issue, or create
	 * a platform-specific converter that will be only used in x86.
	 */
	UTC_Timestamp(etl::string<32> text_timestamp);

	/**
	 * Compare two timestamps.
	 * @param Date the date that will be compared with the pointer `this`
	 */
	bool operator<(const UTC_Timestamp& Date);
	bool operator>(const UTC_Timestamp& Date); ///< @copydoc UTC_Timestamp::operator<
	bool operator==(const UTC_Timestamp& Date); ///< @copydoc UTC_Timestamp::operator<
	bool operator<=(const UTC_Timestamp& Date); ///< @copydoc UTC_Timestamp::operator<
	bool operator>=(const UTC_Timestamp& Date); ///< @copydoc UTC_Timestamp::operator<

	/**
	 * Pretty-print timestamp.
	 *
	 * @todo Find if we can forego including <ostream> here
	 * @param Date the date that will be output
	 */
	friend std::ostream& operator<<(std::ostream& o, UTC_Timestamp const& Date);
};
