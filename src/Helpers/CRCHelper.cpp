#include "Helpers/CRCHelper.hpp"
#include <etl/crc16_ccitt.h>

uint16_t CRCHelper::calculateCRC(const uint8_t* message, uint32_t length) {
	etl::crc16_ccitt CRCEngine;

    CRCEngine.add(message, message + length);
	
    return CRCEngine.value();
}

bool CRCHelper::validateCRC(const uint8_t* message, uint32_t length) {
    if (length < 2U) {
        return false;
    }

	uint16_t computed = calculateCRC(message, length - CRCField);
    uint16_t appended = (static_cast<uint16_t>(message[length - CRCField]) << ByteShift) | message[length - 1];
	return computed == appended;
}
