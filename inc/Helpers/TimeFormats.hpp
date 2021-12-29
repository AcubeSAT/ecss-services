#include <cstdint>
#include <Message.hpp>
#include "macros.hpp"
#include "etl/String.hpp"

// SEE CCSDS 301.0-B-4

//////////////// CONSTANTS ////////////////////
inline constexpr uint8_t SECONDS_PER_MINUTE = 60;
inline constexpr uint16_t SECONDS_PER_HOUR = 3600;
inline constexpr uint32_t SECONDS_PER_DAY = 86400;
static constexpr uint8_t DAYSOFMONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

inline constexpr uint8_t ACUBESAT_CUC_SECONDS_COUNTER_BYTES = 2; // PER DDJF_TTC
inline constexpr uint8_t ACUBESAT_CUC_FRACTIONAL_COUNTER_BYTES = 2; // PER DDJF_TTC
inline constexpr uint32_t UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS = 1546300800; // TODO correct for 2020.01.01
inline constexpr uint16_t ACUBESAT_EPOCH_YEAR = 2019;
inline constexpr uint8_t ACUBESAT_EPOCH_MONTH = 1;
inline constexpr uint8_t ACUBESAT_EPOCH_DAY = 1;

inline constexpr uint8_t MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP = 9;

static_assert(ACUBESAT_EPOCH_YEAR >= 2019);
static_assert(ACUBESAT_EPOCH_MONTH < 11 && ACUBESAT_EPOCH_MONTH >= 0);
static_assert(ACUBESAT_EPOCH_DAY < DAYSOFMONTH[ACUBESAT_EPOCH_MONTH]);
//////////////////////////////////////////////

//////// HELPER CONSTEXPR ////////
template <int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr uint8_t build_short_CUC_header() {
	static_assert(seconds_counter_bytes <= 4, "Use build_long_CUC_header instead");
	static_assert(fractional_counter_bytes <= 3, "Use build_long_CUC_header instead");

	uint8_t header = 0;

	// P-Field extension is 0, CUC header is not extended
	header += 0;

	// AcubeSAT is using custom TAI epoch at 01 Jan 2020
	header = header << 3;
	header += 0b010;

	// Number of bytes in the basic time unit
	header = header << 2;
	header += seconds_counter_bytes - 1;

	// Number of bytes in the fractional unit
	header = header << 2;
	header += fractional_counter_bytes;

	return header;
}

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
	header = header << 3;
	header += 0b010;

	// // Number of bytes in the basic time unit
	header = header << 2;
	header += first_octet_number_of_seconds_bytes - 1;

	// Number of bytes in the fractional unit
	header = header << 2;
	header += first_octet_number_of_fractional_bytes;

	// P-Field extension is 1, CUC header was extended
	header = header << 1;
	header += 1;

	// Number of bytes in the extended basic time unit
	header = header << 2;
	header += second_octet_number_of_seconds_bytes;

	// Number of bytes in the extended fractional unit
	header = header << 3;
	header += second_octet_number_of_fractional_bytes;

	// Last 3 LSb are reserved for custom mission use
	header = header << 2;
	header += 0;

	return header;
}

template <typename T, int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr T build_CUC_header() {
	static_assert((seconds_counter_bytes + fractional_counter_bytes) <= 8,
	              "Complete arbitrary precision not yet supported"); // TODO: see Issue #106 on Gitlab
	// cppcheck-suppress syntaxError
	// cppcheck-suppress redundantCondition
	if constexpr (seconds_counter_bytes <= 4 &&
	              fractional_counter_bytes <= 3) // if constexpr not supported yet in cppcheck
		return build_short_CUC_header<seconds_counter_bytes, fractional_counter_bytes>();
	else
		return build_long_CUC_header<seconds_counter_bytes, fractional_counter_bytes>();
}

inline constexpr uint8_t build_AcubeSAT_CDS_header() {
	uint8_t header = 0;

	// bit 0 is at 0
	header += 0;
	header << 1;

	// timecode identification
	header += 0b100;
	header << 3;

	// AcubeSAT is using custom TAI epoch at 01 Jan 2020
	header += 1;
	header << 1;

	// AcubeSAT is using 16 bits day count segment
	header += 0;
	header << 1;

	// AcubeSAT is using picosecond resolution
	header += 0b10;
	// header << 2;

	return header;
}
////////////////////////////////////////////////

////////// CONVENIENCE FUNCTIONS ////////////
bool is_leap_year(uint16_t year);

////////// Transitory timestamps ////////////
// CUSTOM EPOCH FOR ALL ACUBESAT TIMESTAMPS IS 01 JAN 2020, EXCEPT UTC (UNIX)
class AcubeSAT_CDS_timestamp {
public:
	static constexpr uint8_t P_FIELD = build_AcubeSAT_CDS_header();
	uint16_t day;
	uint16_t ms_of_day;
	uint32_t submilliseconds;

	uint64_t to_CDS_timestamp();
	void from_CDS_timestamp(uint64_t);
};

// CUSTOM EPOCH FOR ALL ACUBESAT TIMESTAMPS IS 01 JAN 2020, EXCEPT UTC (UNIX)
/**
 * A class that represents UTC time and date, from UNIX EPOCH
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
	 * Assign the instances with the Unix epoch 1/1/1970 00:00:00
	 */
	UTC_Timestamp();

	/**
	 * @param year the year as it used in Gregorian calendar
	 * @param month the month as it used in Gregorian calendar
	 * @param day the day as it used in Gregorian calendar
	 * @param hour UTC hour in 24 format
	 * @param minute UTC minutes
	 * @param second UTC seconds
	 */
	UTC_Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	/**
	 * @param text_timestamp the timestamp to parse into a UTC date
	 */
	UTC_Timestamp(etl::string<32> text_timestamp);

	/**
	 * Compare two timestamps.
	 *
	 * @param Date the date that will be compared with the pointer `this`
	 * @return true if the condition is satisfied
	 */
	bool operator<(const UTC_Timestamp& Date);
	bool operator>(const UTC_Timestamp& Date);
	bool operator==(const UTC_Timestamp& Date);
	bool operator<=(const UTC_Timestamp& Date);
	bool operator>=(const UTC_Timestamp& Date);

	/**
	 * Pretty-print timestamp.
	 *
	 * @param Date the date that will be output
	 */
	friend std::ostream& operator<<(std::ostream& o, UTC_Timestamp const& Date);
};
