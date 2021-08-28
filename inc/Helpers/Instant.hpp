#ifndef ECSS_SERVICES_TIME_HPP
#define ECSS_SERVICES_TIME_HPP

#include <cstdint>
#include <stdexcept>
#include "macros.hpp"

// SEE CCSDS 301.0-B-4

//////// USER SETTABLE RANGE PARAMETERS ////////
inline constexpr uint8_t CUC_seconds_counter_bytes = 4; //4 bytes of seconds is approx 3000 years, enough to use recommended epoch
inline constexpr uint8_t CUC_fractional_counter_bytes = 2; //2 byte of fractional second gives 2 us resolution
////////////////////////////////////////////////

inline constexpr uint8_t build_CUC_header(uint8_t CUC_seconds_counter_bytes, uint8_t CUC_fractional_counter_bytes){
  uint8_t header = 0;
  header += 0; header << 1; // P-Field extension is 0, CUC is not extended
  header += 0b001; header << 3; // Acubesat is using recommended TAI epoch
  header += CUC_seconds_counter_bytes - 1 ; header << 2; // CUC_seconds_counter_bytes in the basic time unit
  header += CUC_fractional_counter_bytes; // CUC_fractional_counter_bytes in the fractional unit
  return header;
}

inline constexpr uint64_t seconds_mask_from_bytes(uint8_t seconds_counter_bytes){
  if(seconds_counter_bytes > 4){
    throw std::invalid_argument("Seconds counter is too big for the internal representation of time");
  }
  uint64_t mask = 0;
  for (int i = 0; i < seconds_counter_bytes; i++){
    mask >> 8;
    mask += 0xFF000000;
  }
  return mask;
}

inline constexpr uint64_t fractional_mask_from_bytes(uint8_t fractional_counter_bytes){
  if(fractional_counter_bytes > 4){
    throw std::invalid_argument("Seconds counter is too big for the internal representation of time");
  }
  uint64_t mask = 0;
  for (int i = 0; i < fractional_counter_bytes; i++){
    mask << 8;
    mask += 0x000000FF;
  }
  return mask;
}

/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 */
template<uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
class Instant{
private:
  static constexpr uint64_t seconds_mask = fractional_mask_from_bytes(fractional_counter_bytes);
  static constexpr uint64_t fractional_mask = fractional_mask_from_bytes(fractional_counter_bytes);
  uint64_t tai_counter;
public:
  /**
	 * Initialize the instant as seconds from epoch in TAI
	 *
	 */
  int from_TAI_seconds(int seconds);

  /**
  * Get the representation as seconds from epoch in TAI
  *
  * @return the seconds elapsed in TAI since 1 Jan 1958, cut to the integer part
  */
  int as_TAI_seconds();

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

#endif
