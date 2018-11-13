#ifndef ECSS_SERVICES_PACKET_H
#define ECSS_SERVICES_PACKET_H

#include "ECSS_Definitions.hpp"
#include <cstdint>
#include <cassert>

/**
 * A telemetry (TM) or telecommand (TC) message (request/report), as specified in ECSS-E-ST-70-41C
 */
class Message {
public:
	enum PacketType {
		TM = 0, // Telemetry
		TC = 1 // Telecommand
	};

	// The service and message IDs are 8 bits (5.3.1b, 5.3.3.1d)
	uint8_t serviceType;
	uint8_t messageType;

	// As specified in CCSDS 133.0-B-1 (TM or TC)
	PacketType packetType;

	// Maximum value of 2047 (5.4.2.1c)
	uint16_t applicationId;

	// 7.4.3.1b
	uint16_t messageTypeCounter = 0;

	// TODO: Find out if we need more than 16 bits for this
	uint16_t dataSize = 0;

	// Pointer to the contents of the message (excluding the PUS header)
	// We allocate this data statically, in order to make sure there is predictability in the
	// handling and storage of messages
	// TODO: Is it a good idea to not initialise this to 0?
	uint8_t data[ECSS_MAX_MESSAGE_SIZE] = {'\0'};

//private:
	uint8_t currentBit = 0;

	/**
	 * Appends the least significant \p numBits from \p data to the message
	 *
	 * Note: data MUST NOT contain any information beyond the most significant \p numBits bits
	 * @todo Error handling for failed asserts
	 */
	void appendBits(uint8_t numBits, uint16_t data);

	/**
	 * Appends 1 byte to the message
	 */
	void appendByte(uint8_t value);

	/**
	 * Appends 2 bytes to the message
	 */
	void appendHalfword(uint16_t value);

	/**
	 * Appends 4 bytes to the message
	 */
	void appendWord(uint32_t value);

	/**
	 * Appends \p size bytes to the message
	 *
	 * @param size The amount of byte to append
	 * @param value An array containing at least \p size bytes
	 * @todo See if more than uint8_t strings will be supported
	 */
	void appendString(uint8_t size, const char *value);

public:
	Message(uint8_t serviceType, uint8_t messageType, PacketType packetType,
	        uint16_t applicationId);

	/**
	 * Adds a single-byte boolean value to the end of the message
	 *
	 * PTC = 1, PFC = 0
	 */
	void appendBoolean(bool value) {
		return appendByte(static_cast<uint8_t>(value));
	}

	/**
	 * Adds an enumerated parameter consisting of an arbitrary number of bits to the end of the
	 * message
	 *
	 * PTC = 1, PFC = \p bits
	 */
	void appendEnumerated(uint8_t bits, uint32_t value);

	/**
	 * Adds an enumerated parameter consisting of 1 byte to the end of the message
	 *
	 * PTC = 1, PFC = 8
	 */
	void appendEnum8(uint8_t value) {
		return appendByte(value);
	};

	/**
	 * Adds an enumerated parameter consisting of 2 bytes to the end of the message
	 *
	 * PTC = 1, PFC = 16
	 */
	void appendEnum16(uint16_t value) {
		return appendHalfword(value);
	}

	/**
	 * Adds an enumerated parameter consisting of 4 bytes to the end of the message
	 *
	 * PTC = 1, PFC = 32
	 */
	void appendEnum32(uint32_t value) {
		return appendWord(value);
	}

	/**
	 * Adds a 1 byte unsigned integer to the end of the message
	 *
	 * PTC = 2, PFC = 4
	 */
	void appendUint8(uint8_t value) {
		return appendByte(value);
	}

	/**
	 * Adds a 2 byte unsigned integer to the end of the message
	 *
	 * PTC = 2, PFC = 8
	 */
	void appendUint16(uint16_t value) {
		return appendHalfword(value);
	}

	/**
	 * Adds a 4 byte unsigned integer to the end of the message
	 *
	 * PTC = 2, PFC = 14
	 */
	void appendUint32(uint32_t value) {
		return appendWord(value);
	}

	/**
	 * Adds a 1 byte signed integer to the end of the message
	 *
	 * PTC = 3, PFC = 4
	 */
	void appendSint8(int8_t value) {
		return appendByte(reinterpret_cast<uint8_t&>(value));
	}

	/**
	 * Adds a 2 byte signed integer to the end of the message
	 *
	 * PTC = 3, PFC = 8
	 */
	void appendSint16(int16_t value) {
		return appendHalfword(reinterpret_cast<uint16_t&>(value));
	}

	/**
	 * Adds a 4 byte signed integer to the end of the message
	 *
	 * PTC = 3, PFC = 14
	 */
	void appendSint32(int32_t value) {
		return appendWord(reinterpret_cast<uint32_t&>(value));
	}

	/**
	 * Adds a 4-byte single-precision floating point number to the end of the message
	 *
	 * PTC = 5, PFC = 1
	 */
	void appendFloat(float value) {
		static_assert(sizeof(uint32_t) == sizeof(value), "Floating point numbers must be 32 bits long");

		return appendWord(reinterpret_cast<uint32_t&>(value));
	}
};


#endif //ECSS_SERVICES_PACKET_H
