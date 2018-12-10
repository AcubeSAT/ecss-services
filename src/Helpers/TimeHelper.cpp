#include "Helpers/TimeHelper.hpp"

uint64_t TimeHelper::implementCUCTimeFormat(uint32_t seconds) {
	// the total number of octets including the P-field (1 octet) and T-field(4 octets) is 5

	// define the P-field
	const uint8_t bit0 = 0; // P-field extension(‘zero’: no extension, ‘one’: field is extended)
	const uint8_t bits1_3 = 1; // Time code identification ( 001 -> 1958 January 1 epoch )
	const uint8_t bits4_5 = 4 - 1; // Number of octets of the basic time unit minus one
	const uint8_t bits6_7 = 0; // Number of octets of the fractional time unit
	const uint8_t pField = (bits6_7 << 6 | bits4_5 << 4 | bits1_3 << 1 | bit0);

	// just a reminder to be careful with the assigned values
	static_assert(bit0 < 2);
	static_assert(bits1_3 < 16);
	static_assert(bits4_5 < 4);
	static_assert(bits6_7 < 4);

	/**
	 * Define the T-field, the seconds passed from the defined epoch 1 January 1958
	 * We use 4 octets(32 bits) for the time unit(seconds) because 32 bits for the seconds are
	 * enough to count 136 years! But if we use 24 bits for the seconds then it will count 0,5
	 * years and this isn't enough. Remember we can use only integers numbers of octets for the
	 * time unit(second)
	 *
	 * @todo the implementation of the total seconds depends on the structure of the RTC
	 */
	uint32_t totalSeconds = seconds;

	/**
	 * Define time format including P-field and T-Field
	 *
	 * Note: Only the 40 bits of the 64 will be used for the timeFormat(0-7 : P-field, 8-39:
	 * T-field)
	*/
	uint64_t timeFormat = (totalSeconds << 8 | pField);

	return timeFormat;
}
