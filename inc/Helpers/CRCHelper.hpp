#ifndef ECSS_SERVICES_CRCHELPER_HPP
#define ECSS_SERVICES_CRCHELPER_HPP

#include <cstdint>

class CRCHelper {
	/**
	 * CRC16 calculation helper class
	 * This class declares a function which calculates the CRC16 checksum of the given data.
	 *
	 * For now the actual implementation is the CRC16/CCITT variant (ECSS-E-ST-70-41C, pg.615)
	 * (polynomial 0x1021, normal input), but this can change at any time
	 * (even to a hardware CRC implementation, if available)
	 *
	 * Please report all found bugs.
	 *
	 * @author (CRC explanation) http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
	 * @author (class code & dox) Grigoris Pavlakis <grigpavl@ece.auth.gr>
	 */
private:
	/**
	 * shift register contains all 1's initially (ECSS-E-ST-70-41C, Annex B - CRC and ISO checksum)
	 */
	inline static const uint16_t InitialShiftRegisterValue = 0xFFFFU;
	// TODO (#204): Change this to hardware implementation or a trusted software one
	/**
	 * CRC16-CCITT generator polynomial (as specified in standard)
	 */
	inline static const uint16_t Polynomial = 0x1021U;
	/**
	 * MSB mask (for shifting)
	 * if the MSB is set, the bitwise AND gives 1
	 */
	inline static const uint16_t MSBMask = 0x8000U;

	/**
	* Number of bits in a byte
	 */
	inline static const uint16_t BitNumber = 8U;

public:
	/**
	 * Actual CRC calculation function.
	 * @param  message (pointer to the data to be checksummed)
	 * @param  length (size in bytes)
	 * @return the CRC16 checksum of the input data
	 */
	static uint16_t calculateCRC(const uint8_t* message, uint32_t length);

	/**
	 * CRC validation function. Make sure the passed message actually contains a CRC checksum
	 * appended at the very end!
	 * @param  message (pointer to the data to be validated)
	 * @param  length (in bytes, plus 2 bytes for the CRC checksum)
	 * @return 0 when the data is valid, a nonzero uint16 when the data is corrupted
	 */
	static uint16_t validateCRC(const uint8_t* message, uint32_t length);

	/**
	 * Config bool to enable or disable CRC
	 */
	static constexpr bool EnableCRC = true;
};

#endif // ECSS_SERVICES_CRCHELPER_HPP
