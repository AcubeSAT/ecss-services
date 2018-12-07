#include "Helpers/TimeHelper.hpp"

void TimeHelper::implementCUCTimeFormat(uint32_t seconds, Message &data) {
	// the total number of octets including the p-field (1 octet) and t-field(4 octets) is 5

	// define the P-field
	uint8_t bit0 = 0; // P-field extension(‘zero’: no extension; ‘one’: field is extended)
	uint8_t bits1_3 = 1; // Time code identification ( 001 -> 1958 January 1 epoch )
	uint8_t bits4_5 = 4 - 1; // Number of octets of the basic time unit minus one
	uint8_t bits6_7 = 0; // Number of octets of the fractional time unit

	// just a reminder to be careful with the assigned values
	assert(bit0 < 2);
	assert(bits1_3 < 16);
	assert(bits4_5 < 4);
	assert(bits6_7 < 4);

	/**
	 * Define the T-field, the seconds passed from the defined epoch 1 January 1958
	 * We use 4 octets(32 bits)for the time unit(seconds) because 32 bits for the seconds are enough
	 * to count 136 years! But if we use 24 bits for the seconds then it will count 0,5 years and
	 * this isn't enough. Remember we can use only integers numbers of octets for the time unit
	 * (second)
	 *
	 * @todo the implementation of the total seconds depends on the structure of the RTC
	 */
	uint32_t totalSeconds = seconds;

	// define data
	data.appendBits(1, bit0);
	data.appendBits(3, bits1_3);
	data.appendBits(2, bits4_5);
	data.appendBits(2, bits6_7);
	data.appendWord(totalSeconds);
}
