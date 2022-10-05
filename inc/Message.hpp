#ifndef ECSS_SERVICES_MESSAGE_HPP
#define ECSS_SERVICES_MESSAGE_HPP

#include <cstdint>
#include "ECSS_Definitions.hpp"

class Message {
public:
	// TODO: Find out if we need more than 16 bits for this
	uint16_t dataSize = 0;

	// Pointer to the contents of the message (excluding the PUS header)
	// We allocate this data statically, in order to make sure there is predictability in the
	// handling and storage of messages
	//
	// @note This is initialized to 0 in order to prevent any mishaps with non-properly initialized values. \ref
	// ECSSMessage::appendBits() relies on this in order to easily OR the requested bits.
	uint8_t data[ECSSMaxMessageSize] = {0};

	// private:
	uint8_t currentBit = 0;

	// Next byte to read for read...() functions
	uint16_t readPosition = 0;
};

#endif //ECSS_SERVICES_MESSAGE_HPP
