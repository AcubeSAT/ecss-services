#ifndef ECSS_SERVICES_TIME_HPP
#define ECSS_SERVICES_TIME_HPP

#include <cstdint>
#include <stdexcept>
#include "macros.hpp"

// SEE CCSDS 301.0-B-4

//////// USER SETTABLE RANGE PARAMETERS ////////
inline constexpr uint8_t CUC_seconds_counter_bytes = 4; // 4 bytes of seconds is approx 3000 years, enough to use recommended epoch
inline constexpr uint8_t CUC_fractional_counter_bytes = 2; // 2 byte of fractional second gives 2 us resolution
////////////////////////////////////////////////

//////// HELPER CONSTEXPR DO NOT TOUCH ////////
inline constexpr uint8_t build_short_CUC_header(uint8_t CUC_seconds_counter_bytes, uint8_t CUC_fractional_counter_bytes) {
	static_assert( CUC_seconds_counter_bytes <= 4, "Use build_long_CUC_header instead");
	static_assert( CUC_fractional_counter_bytes <= 3, "Use build_long_CUC_header instead");

	uint8_t header = 0;

  // P-Field extension is 0, CUC header is not extended
	header += 0;
	header << 1;

  // Acubesat is using custom TAI epoch at 01 Jan 2020
	header += 0b010;
	header << 3;

  // Number of bytes in the basic time unit
	header += CUC_seconds_counter_bytes - 1;
	header << 2;

  // Number of bytes in the fractional unit
	header += CUC_fractional_counter_bytes;
  //header << 0;

	return header;
}

inline constexpr uint16_t build_long_CUC_header(uint8_t CUC_seconds_counter_bytes, uint8_t CUC_fractional_counter_bytes) {
	static_assert( CUC_seconds_counter_bytes > 4 | CUC_fractional_counter_bytes > 3, "Use build_short_CUC_header instead");
	static_assert( CUC_seconds_counter_bytes <= 7, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");
	static_assert( CUC_fractional_counter_bytes <= 6, "Number of bytes for seconds over maximum number of octets allowed by CCSDS");

	uint16_t header = 0;

	uint8_t first_octet_number_of_seconds_bytes = max(4, CUC_seconds_counter_bytes);
	uint8_t second_octet_number_of_seconds_bytes = CUC_seconds_counter_bytes - first_octet_number_of_seconds_bytes;

	uint8_t first_octet_number_of_fractional_bytes = max(3, CUC_fractional_counter_bytes);
	uint8_t second_octet_number_of_fractional_bytes = CUC_fractional_counter_bytes - first_octet_number_of_fractional_bytes;

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


////////////////////////////////////////////////

///////////// CLASS DECLARATION ////////////////
/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 */
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
class Instant {
	static_assert((seconds_counter_bytes + fractional_counter_bytes) <= 8);

private:
	static constexpr uint8_t CUC_header = build_CUC_header(seconds_counter_bytes, fractional_counter_bytes);
	uint64_t tai_counter = 0;

public:
	/**
	 * Initialize the instant as seconds from epoch in TAI
	 *
	 */
	void update_from_TAI_seconds(int seconds);

	/**
	 * Get the representation as seconds from epoch in TAI
	 *
	 * @return the seconds elapsed in TAI since 1 Jan 1958, cut to the integer part
	 */
	int as_TAI_seconds();

	/**
	 * Get the representation as CUC formatted bytes
	 *
	 * @return the instant, represented in the CCSDS CUC format
	 */
	void as_CUC_timestamp();

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than \p Instant
	 */
	bool operator<(const Instant& Instant);

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than \p Instant
	 */
	bool operator>(const Instant& Instant);

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is equal to \p Instant
	 */
	bool operator==(const Instant& Instant);

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is smaller than or equal to \p Instant
	 */
	bool operator<=(const Instant& Instant);

	/**
	 * Compare two instants.
	 *
	 * @param Instant the date that will be compared with the pointer `this`
	 * @return true if the pointer `this` is greater than or equal to \p Instant
	 */
	bool operator>=(const Instant& Instant);
};
////////////////////////////////////////////////

////// TEMPLATED METHODS INSTANCIATION /////////
#include "Instant.tpp"
////////////////////////////////////////////////

#endif
