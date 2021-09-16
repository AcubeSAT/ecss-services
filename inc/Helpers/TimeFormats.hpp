#include <cstdint>
#include <Message.hpp>
#include "macros.hpp"

// SEE CCSDS 301.0-B-4

bool is_leap_year(uint16_t year);

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
};
