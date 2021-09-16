#include <cstdint>
#include <Message.hpp>
#include "macros.hpp"
#include "etl/String.hpp"

// SEE CCSDS 301.0-B-4

//////// HELPER CONSTEXPR DO NOT TOUCH ////////
template <int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr uint8_t build_short_CUC_header() {
	static_assert( seconds_counter_bytes <= 4, "Use build_long_CUC_header instead");
	static_assert( fractional_counter_bytes <= 3, "Use build_long_CUC_header instead");

	uint8_t header = 0;

  // P-Field extension is 0, CUC header is not extended
	header += 0;
	header << 1;

  // Acubesat is using custom TAI epoch at 01 Jan 2020
	header += 0b010;
	header << 3;

  // Number of bytes in the basic time unit
	header += seconds_counter_bytes - 1;
	header << 2;

  // Number of bytes in the fractional unit
	header += fractional_counter_bytes;
  //header << 0;

	return header;
}

template <int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr uint16_t build_long_CUC_header() {
	static_assert( seconds_counter_bytes > 4 || fractional_counter_bytes > 3, "Use build_short_CUC_header instead");
	static_assert( seconds_counter_bytes <= 7, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");
	static_assert( fractional_counter_bytes <= 6, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");

	uint16_t header = 0;

	uint8_t first_octet_number_of_seconds_bytes = std::max(4, seconds_counter_bytes);
	uint8_t second_octet_number_of_seconds_bytes = seconds_counter_bytes - first_octet_number_of_seconds_bytes;

	uint8_t first_octet_number_of_fractional_bytes = std::max(3, fractional_counter_bytes);
	uint8_t second_octet_number_of_fractional_bytes = fractional_counter_bytes - first_octet_number_of_fractional_bytes;

  // P-Field extension is 1, CUC header is extended
	header += 1;
	header << 1;

	// Acubesat is using custom TAI epoch at 01 Jan 2020
	header += 0b010;
	header << 3;

  // Number of bytes in the basic time unit
	header += first_octet_number_of_seconds_bytes - 1;
	header << 2;

  // Number of bytes in the fractional unit
	header += first_octet_number_of_fractional_bytes;
  header << 2;

	// P-Field extension is 1, CUC header was extended
	header += 1;
	header << 1;

	// Number of bytes in the extended basic time unit
	header += second_octet_number_of_seconds_bytes;
	header << 2;

  // Number of bytes in the extended fractional unit
	header += second_octet_number_of_fractional_bytes;
  header << 2;

	// Last 3 LSB are reserved for custom mission use
	//header += 0;
  //header << 3;

	return header;
}

template <typename T, int seconds_counter_bytes, int fractional_counter_bytes>
inline constexpr T build_CUC_header() {
	static_assert((seconds_counter_bytes + fractional_counter_bytes ) <= 8, "Complete arbitrary precision not yet supported"); //TODO improve and get rid of this
	//cppcheck-suppress syntaxError
	if constexpr (seconds_counter_bytes <= 4 && fractional_counter_bytes <= 3) //if constexpr not supported yet in cppcheck
		return build_short_CUC_header<seconds_counter_bytes,fractional_counter_bytes>();
	else
		return build_long_CUC_header<seconds_counter_bytes,fractional_counter_bytes>();
}

inline constexpr uint8_t build_Acubesat_CDS_header() {
	uint8_t header = 0;

  // bit 0 is at 0
	header += 0;
	header << 1;

  // timecode identification
  header += 0b100;
  header << 3;

  // Acubesat is using custom TAI epoch at 01 Jan 2020
	header += 1;
	header << 1;

  // Acubesat is using 16 bits day count segment
  header += 0;
  header << 1;

  // Acubesat is using picosecond resolution
	header += 0b10;
	//header << 2;

	return header;
}
////////////////////////////////////////////////

//////////////// CONSTANTS ////////////////////
inline constexpr uint8_t Acubesat_CUC_seconds_counter_bytes = 2; // PER DDJF_TTC
inline constexpr uint8_t Acubesat_CUC_fractional_counter_bytes = 2; // PER DDJF_TTC
//////////////////////////////////////////////

////////// CONVENIENCE FUNCTIONS ////////////
bool is_leap_year(uint16_t year);

////////// Transitory timestamps ////////////
// CUSTOM EPOCH FOR ALL ACUBESAT TIMESTAMPS IS 01 JAN 2020, EXCEPT UTC (UNIX)
class Acubesat_CDS_timestamp{
public:
  static constexpr uint8_t P_FIELD = build_Acubesat_CDS_header();
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
	UTC_Timestamp(String<32> text_timestamp); // TODO change to ETL string type
};
