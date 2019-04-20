#include "Helpers/CRCHelper.hpp"

// TODO: THIS IS TEMPORARY CODE, WILL SURELY BE REPLACED

uint16_t CRCHelper::calculateCRC(const uint8_t* message, uint32_t length) {
	// shift register contains all 1's initially (ECSS-E-ST-70-41C, Annex B - CRC and ISO checksum)
	uint16_t shiftReg = 0xFFFFu;

	// CRC16-CCITT generator polynomial (as specified in standard)
	uint16_t polynomial = 0x1021u;

	for (uint32_t i = 0; i < length; i++) {
		// "copy" (XOR w/ existing contents) the current msg bits into the MSB of the shift register
		shiftReg ^= (message[i] << 8u);

		for (int j = 0; j < 8; j++) {
			// if the MSB is set, the bitwise AND gives 1
			if ((shiftReg & 0x8000u) != 0u) {
				// toss out of the register the MSB and divide (XOR) its content with the generator
				shiftReg = ((shiftReg << 1u) ^ polynomial);
			} else {
				// just toss out the MSB and make room for a new bit
				shiftReg <<= 1u;
			}
		}
	}
	return shiftReg;
}

uint16_t CRCHelper::validateCRC(const uint8_t* message, uint32_t length) {
	return calculateCRC(message, length);
	// CRC result of a correct msg w/checksum appended is 0
}
