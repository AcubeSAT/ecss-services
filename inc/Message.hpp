#ifndef ECSS_SERVICES_MESSAGE_HPP
#define ECSS_SERVICES_MESSAGE_HPP

#include <cstdint>
#include "ECSS_Definitions.hpp"

class Message {
public:
	// TODO: Find out if we need more than 16 bits for this
	uint16_t dataSize = 0;

	uint8_t data[1] = {0};

	// private:
	uint8_t currentBit = 0;

	// Next byte to read for read...() functions
	uint16_t readPosition = 0;
};

#endif //ECSS_SERVICES_MESSAGE_HPP
