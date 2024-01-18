#include "Helpers/CRCHelper.hpp"
#include "Helpers/TypeDefinitions.hpp"

uint16_t CRCHelper::calculateCRC(const uint8_t* message, uint32_t length) {
	// shift register contains all 1's initially (ECSS-E-ST-70-41C, Annex B - CRC and ISO checksum)
	CRCSize shiftReg = InitialShiftRegisterValue;

	for (uint32_t i = 0; i < length; i++) {
		// "copy" (XOR w/ existing contents) the current msg bits into the MSB of the shift register
		shiftReg ^= (message[i] << BitNumber);

		for (int j = 0; j < BitNumber; j++) {
			// if the MSB is set, the bitwise AND gives 1
			if ((shiftReg & MSBMask) != 0U) {
				// toss out of the register the MSB and divide (XOR) its content with the generator
				shiftReg = ((shiftReg << 1U) ^ Polynomial);
			} else {
				// just toss out the MSB and make room for a new bit
				shiftReg <<= 1U;
			}
		}
	}
	return shiftReg;
}

uint16_t CRCHelper::validateCRC(const uint8_t* message, uint32_t length) {
	return calculateCRC(message, length);
	// CRC result of a correct msg w/checksum appended is 0
}
