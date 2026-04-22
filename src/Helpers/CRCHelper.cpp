#include "Helpers/CRCHelper.hpp"
#include "Helpers/TypeDefinitions.hpp"

uint16_t CRCHelper::calculateCRC(const uint8_t* message, uint32_t length) {
	etl::crc16_ccitt crc_engine;

    crc_engine.add(message, message + length);
	
    return crc_engine.value();
}

bool CRCHelper::validateCRC(const uint8_t* message, uint32_t length) {
	// CRC result of a correct msg w/checksum appended is 0
	return calculateCRC(message, length) == 0U;
}
