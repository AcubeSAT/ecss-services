#ifndef ECSS_SERVICES_PACKET_H
#define ECSS_SERVICES_PACKET_H

// Forward declaration of the Message class, needed for the ErrorHandler
class Message;

#include "ECSS_Definitions.hpp"
#include <cstdint>
#include <cassert>
#include <etl/String.hpp>
#include <etl/wstring.h>
#include "ErrorHandler.hpp"
#include "macros.hpp"

/**
 * A telemetry (TM) or telecommand (TC) message (request/report), as specified in ECSS-E-ST-70-41C
 *
 * @todo Make sure that a message can't be written to or read from at the same time, or make
 *       readable and writable message different classes
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

	/**
	 * The destination APID of the message
	 *
	 * Maximum value of 2047 (5.4.2.1c)
	 */
	uint16_t applicationId;

	// 7.4.3.1b
	uint16_t messageTypeCounter = 0;

	// 7.4.1, as defined in CCSDS 133.0-B-1
	uint16_t packetSequenceCount = 0;

	// TODO: Find out if we need more than 16 bits for this
	uint16_t dataSize = 0;

	// Pointer to the contents of the message (excluding the PUS header)
	// We allocate this data statically, in order to make sure there is predictability in the
	// handling and storage of messages
	// TODO: Is it a good idea to not initialise this to 0?
	uint8_t data[ECSS_MAX_MESSAGE_SIZE] = {'\0'};

//private:
	uint8_t currentBit = 0;

	// Next byte to read for read...() functions
	uint16_t readPosition = 0;

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
	 * Appends a number of bytes to the message
	 *
	 * @param string The string to insert
	 */
	template<const size_t SIZE>
	void appendString(const String<SIZE> & string);

	/**
	 * Reads the next \p numBits bits from the the message in a big-endian format
	 * @param numBits
	 * @return A maximum number of 16 bits is returned (in big-endian format)
	 */
	uint16_t readBits(uint8_t numBits);

	/**
	 * Reads the next 1 byte from the message
	 */
	uint8_t readByte();

	/**
	 * Reads the next 2 bytes from the message
	 */
	uint16_t readHalfword();

	/**
	 * Reads the next 4 bytes from the message
	 */
	uint32_t readWord();

	/**
	 * Reads the next \p size bytes from the message, and stores them into the allocated \p string
	 *
	 * NOTE: We assume that \p string is already allocated, and its size is at least
	 * ECSS_MAX_STRING_SIZE. This function does placs a \0 at the end of the created string.
	 */
	void readString(char *string, uint8_t size);

	/**
	* Reads the next \p size bytes from the message, and stores them into the allocated \p string
	*
	* NOTE: We assume that \p string is already allocated, and its size is at least
	* ECSS_MAX_STRING_SIZE. This function does placs a \0 at the end of the created string
	* @todo Is uint16_t size too much or not enough? It has to be defined
	*/
	void readString(uint8_t *string, uint16_t size);

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
	 * PTC = 2, PFC = \p bits
	 */
	void appendEnumerated(uint8_t bits, uint32_t value) {
		// TODO: Implement 32-bit enums, if needed

		return appendBits(bits, value);
	}

	/**
	 * Adds an enumerated parameter consisting of 1 byte to the end of the message
	 *
	 * PTC = 2, PFC = 8
	 */
	void appendEnum8(uint8_t value) {
		return appendByte(value);
	};

	/**
	 * Adds an enumerated parameter consisting of 2 bytes to the end of the message
	 *
	 * PTC = 2, PFC = 16
	 */
	void appendEnum16(uint16_t value) {
		return appendHalfword(value);
	}

	/**
	 * Adds an enumerated parameter consisting of 4 bytes to the end of the message
	 *
	 * PTC = 2, PFC = 32
	 */
	void appendEnum32(uint32_t value) {
		return appendWord(value);
	}

	/**
	 * Adds a 1 byte unsigned integer to the end of the message
	 *
	 * PTC = 3, PFC = 4
	 */
	void appendUint8(uint8_t value) {
		return appendByte(value);
	}

	/**
	 * Adds a 2 byte unsigned integer to the end of the message
	 *
	 * PTC = 3, PFC = 8
	 */
	void appendUint16(uint16_t value) {
		return appendHalfword(value);
	}

	/**
	 * Adds a 4 byte unsigned integer to the end of the message
	 *
	 * PTC = 3, PFC = 14
	 */
	void appendUint32(uint32_t value) {
		return appendWord(value);
	}

	/**
	 * Adds an 8 byte unsigned integer to the end of the message
	 *
	 * PTC = 3, PFC = 16
	 */
	void appendUint64(uint64_t value) {
		appendWord(static_cast<uint32_t >(value >> 32));
		appendWord(static_cast<uint32_t >(value));
	}

	/**
	 * Adds a 1 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 4
	 */
	void appendSint8(int8_t value) {
		return appendByte(reinterpret_cast<uint8_t &>(value));
	}

	/**
	 * Adds a 2 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 8
	 */
	void appendSint16(int16_t value) {
		return appendHalfword(reinterpret_cast<uint16_t &>(value));
	}

	/**
	 * Adds a 4 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 14
	 */
	void appendSint32(int32_t value) {
		return appendWord(reinterpret_cast<uint32_t &>(value));
	}

	/**
	 * Adds a 4-byte single-precision floating point number to the end of the message
	 *
	 * PTC = 5, PFC = 1
	 */
	void appendFloat(float value) {
		static_assert(sizeof(uint32_t) == sizeof(value),
		              "Floating point numbers must be 32 bits long");

		return appendWord(reinterpret_cast<uint32_t &>(value));
	}

	/**
	 * Adds a N-byte string to the end of the message
	 *
	 *
	 * PTC = 7, PFC = 0
	 */
	template<const size_t SIZE>
	void appendOctetString(const String<SIZE> & string) {
		// Make sure that the string is large enough to count
		assertI(string.size() <= (std::numeric_limits<uint16_t>::max)(),
			ErrorHandler::StringTooLarge);

		appendUint16(string.size());
		appendString(string);
	}

	/**
	 * Fetches a single-byte boolean value from the current position in the message
	 *
	 * PTC = 1, PFC = 0
	 */
	bool readBoolean() {
		return static_cast<bool>(readByte());
	}

	/**
	 * Fetches an enumerated parameter consisting of an arbitrary number of bits from the current
	 * position in the message
	 *
	 * PTC = 2, PFC = \p bits
	 */
	uint32_t readEnumerated(uint8_t bits) {
		return readBits(bits);
	}

	/**
	 * Fetches an enumerated parameter consisting of 1 byte from the current position in the message
	 *
	 * PTC = 2, PFC = 8
	 */
	uint8_t readEnum8() {
		return readByte();
	}

	/**
	 * Fetches an enumerated parameter consisting of 2 bytes from the current position in the
	 * message
	 *
	 * PTC = 2, PFC = 16
	 */
	uint16_t readEnum16() {
		return readHalfword();
	}

	/**
	 * Fetches an enumerated parameter consisting of 4 bytes from the current position in the
	 * message
	 *
	 * PTC = 2, PFC = 32
	 */
	uint32_t readEnum32() {
		return readWord();
	}

	/**
	 * Fetches an 1-byte unsigned integer from the current position in the message
	 *
	 * PTC = 3, PFC = 4
	 */
	uint8_t readUint8() {
		return readByte();
	}

	/**
	 * Fetches a 2-byte unsigned integer from the current position in the message
	 *
	 * PTC = 3, PFC = 8
	 */
	uint16_t readUint16() {
		return readHalfword();
	}

	/**
	 * Fetches a 4-byte unsigned integer from the current position in the message
	 *
	 * PTC = 3, PFC = 14
	 */
	uint32_t readUint32() {
		return readWord();
	}

	/**
	 * Fetches an 8-byte unsigned integer from the current position in the message
	 *
	 * PTC = 3, PFC = 16
	 */
	uint64_t readUint64() {
		return (static_cast<uint64_t >(readWord()) << 32) | static_cast<uint64_t >(readWord());
	}

	/**
	 * Fetches an 1-byte signed integer from the current position in the message
	 *
	 * PTC = 4, PFC = 4
	 */
	int8_t readSint8() {
		uint8_t value = readByte();
		return reinterpret_cast<int8_t &>(value);
	}

	/**
	 * Fetches a 2-byte unsigned integer from the current position in the message
	 *
	 * PTC = 4, PFC = 8
	 */
	int16_t readSint16() {
		uint16_t value = readHalfword();
		return reinterpret_cast<int16_t &>(value);
	}

	/**
	 * Fetches a 4-byte unsigned integer from the current position in the message
	 *
	 * PTC = 4, PFC = 14
	 */
	int32_t readSint32() {
		uint32_t value = readWord();
		return reinterpret_cast<int32_t &>(value);
	}

	/**
	 * Fetches an 4-byte single-precision floating point number from the current position in the
	 * message
	 *
	 * @todo Check if endianness matters for this
	 *
	 * PTC = 5, PFC = 1
	 */
	float readFloat() {
		static_assert(sizeof(uint32_t) == sizeof(float),
		              "Floating point numbers must be 32 bits long");

		uint32_t value = readWord();
		return reinterpret_cast<float &>(value);
	}

	/**
	 * Fetches a N-byte string from the current position in the message
	 *
	 * @details In the current implementation we assume that a preallocated array of
	 * 			sufficient size is provided as the argument
	 * @todo Specify if the provided array size is too small or too large
	 *
	 * PTC = 7, PFC = 0
	 */
	uint16_t readOctetString(uint8_t *byteString) {
		uint16_t size = readUint16(); // Get the data length from the message
		readString(byteString, size); // Read the string data

		return size; // Return the string size
	}

	/**
	 * Reset the message reading status, and start reading data from it again
	 */
	void resetRead();
};

template<const size_t SIZE>
inline void Message::appendString(const String<SIZE> & string) {
	assertI(dataSize + string.size() < ECSS_MAX_MESSAGE_SIZE, ErrorHandler::MessageTooLarge);
	// TODO: Do we need to keep this check? How does etl::string handle it?
	assertI(string.size() < string.capacity(), ErrorHandler::StringTooLarge);

	memcpy(data + dataSize, string.data(), string.size());

	dataSize += string.size();
}


#endif //ECSS_SERVICES_PACKET_H
