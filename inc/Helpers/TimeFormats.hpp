#include <cstdint>
#include <Message.hpp>
#include "macros.hpp"
#include "etl/String.hpp"

/**
 * An armada of utilities regarding timekeeping, timestamps and conversion between different internal and string
 * time formats. This file implements [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf).
 *
 * @author Baptiste Fournier
 */
namespace Time
{
inline constexpr uint8_t SecondsPerMinute = 60;
inline constexpr uint16_t SecondsPerHour = 3600;
inline constexpr uint32_t SecondsPerDay = 86400;
static constexpr uint8_t DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * Number of bytes used for the basic time units of the CUC header for this mission
 */
inline constexpr uint8_t CUCSecondsBytes = 2;

/**
 * Number of bytes used for the fractional time units of the CUC header for this mission
 */
inline constexpr uint8_t CUCFractionalBytes = 2;

/**
 * The system epoch (clock measurement starting time)
 * All timestamps emitted by the ECSS services will show the elapsed time (seconds, days etc.) from this epoch.
 * @todo Update AcubeSAT epoch to 2020.01.01.
 */
inline constexpr struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
} Epoch { 2019, 1, 1, };

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
inline constexpr uint32_t EpochSecondsFromUnix = 1546300800;

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

	// AcubeSAT is using custom TAI epoch ("agency-defined epoch")
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
	static_assert(secondsBytes <= 7,
	              "Number of bytes for seconds over maximum number of octets allowed by CCSDS");
	static_assert(fractionalBytes <= 6,
	              "Number of bytes for seconds over maximum number of octets allowed by CCSDS");

	uint16_t header = 0;

	uint8_t octet1secondsBytes = std::min(4, secondsBytes);
	uint8_t octet2secondsBytes = secondsBytes - octet1secondsBytes;

	uint8_t octet1fractionalBytes = std::min(3, fractionalBytes);
	uint8_t octet2fractionalBytes = fractionalBytes - octet1fractionalBytes;

	// P-Field extension is 1, CUC header is extended
	header += 1;

	// AcubeSAT is using custom TAI epoch at 01 Jan 2020
	header <<= 3U;
	header += 0b010;

	// // Number of bytes in the basic time unit
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
	header <<= 2U;
	header += octet2fractionalBytes;

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
 * @tparam secondsBytes The number of octets used to represent the basic time units
 * @tparam fractionalBytes The number of octets used to represent the fractional time units
 * @return One or two bytes representing the header
 */
template <typename T, int secondsBytes, int fractionalBytes>
inline constexpr T buildCUCHeader() {
	// TODO: Gitlab issue #106
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
	UTCTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	/**
	 * @param textTimestamp the timestamp to parse into a UTC date
	 * @todo Too expensive to implement (?). It is better to remove this and open it as another issue, or create
	 * a platform-specific converter that will be only used in x86.
	 */
	UTCTimestamp(etl::string<32> textTimestamp);

	/**
	 * Compare two timestamps.
	 * @param Date the date that will be compared with the pointer `this`
	 */
	bool operator<(const UTCTimestamp& Date);
	bool operator>(const UTCTimestamp& Date); ///< @copydoc UTCTimestamp::operator<
	bool operator==(const UTCTimestamp& Date); ///< @copydoc UTCTimestamp::operator<
	bool operator<=(const UTCTimestamp& Date); ///< @copydoc UTCTimestamp::operator<
	bool operator>=(const UTCTimestamp& Date); ///< @copydoc UTCTimestamp::operator<

	/**
	 * Pretty-print timestamp.
	 *
	 * @todo Find if we can forego including <ostream> here
	 * @param Date the date that will be output
	 */
	friend std::ostream& operator<<(std::ostream& o, UTCTimestamp const& Date);
};
