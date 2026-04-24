#include "Helpers/CRCHelper.hpp"
#include "Helpers/TypeDefinitions.hpp"

uint16_t CRCHelper::calculateCRC(const uint8_t* message, uint32_t length) {
	etl::crc16_ccitt crc_engine;

    crc_engine.add(message, message + length);
	
    return crc_engine.value();
}

bool CRCHelper::validateCRC(const uint8_t* message, uint32_t length) {
    if (length < 2U) {
        return false;
    }

	uint16_t computed = calculateCRC(message, length - 2);
    uint16_t appended = (static_cast<uint16_t>(message[length - 2]) << CRCHelper::BYTE_SHIFT) | message[length - 1];
	return computed == appended;
}
