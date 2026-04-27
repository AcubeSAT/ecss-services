#ifndef ECSS_SERVICES_CRCHELPER_HPP
#define ECSS_SERVICES_CRCHELPER_HPP

#include <cstdint>
#include "Helpers/TypeDefinitions.hpp"

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
	 * Number of bits to shift the high CRC byte when reassembling the appended CRC
	 */
	 inline static constexpr uint8_t ByteShift = 8U;
public:
	/**
	 * The number of bytes in the CRC field
	 */
	static constexpr CRCSize CRCField = 2U;
	
	/**
	 * Actual CRC calculation function.
	 * Uses ETL crc16_ccitt implementation for the calculation.
	 * @param  message (pointer to the data to be checksummed)
	 * @param  length (size in bytes)
	 * @return the CRC16 checksum of the input data
	 */
	static uint16_t calculateCRC(const uint8_t* message, uint32_t length);

	/**
	 * CRC validation function. Make sure the passed message actually contains a CRC checksum
	 * appended at the very end!
	 * @param message (pointer to the data to be validated)
	 * @param length (in bytes, plus 2 bytes for the CRC checksum)
	 * @return
 	 *  - true when the CRC check passes.
 	 *  - false when:
 	 *    - `length < 2` (not enough bytes for a 2‑byte CRC), or
 	 *    - computed CRC does not equal the appended CRC (CRC mismatch).
 	 *
 	 * @note This function does not log or report errors. Callers are responsible for reporting.
	 */
	static bool validateCRC(const uint8_t* message, uint32_t length);
};

#endif // ECSS_SERVICES_CRCHELPER_HPP
