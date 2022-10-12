#ifndef ECSS_SERVICES_MESSAGE_HPP
#define ECSS_SERVICES_MESSAGE_HPP

#include <cstdint>
#include "ECSS_Definitions.hpp"

class Message {
public:
	static constexpr size_t MaxMessageSize = 1;

	// TODO: Find out if we need more than 16 bits for this
	uint16_t dataSize = 0;

	uint8_t data[MaxMessageSize] = {0};

	// private:
	uint8_t currentBit = 0;

	// Next byte to read for read...() functions
	uint16_t readPosition = 0;

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
	 * Adds a single-byte boolean value to the end of the message
	 *
	 * PTC = 1, PFC = 0
	 */
	void appendBoolean(bool value) {
		return appendByte(static_cast<uint8_t>(value));
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
		appendWord(static_cast<uint32_t>(value >> 32));
		appendWord(static_cast<uint32_t>(value));
	}

	/**
	 * Adds a 1 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 4
	 */
	void appendSint8(int8_t value) {
		return appendByte(reinterpret_cast<uint8_t&>(value));
	}

	/**
	 * Adds a 2 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 8
	 */
	void appendSint16(int16_t value) {
		return appendHalfword(reinterpret_cast<uint16_t&>(value));
	}

	/**
	 * Adds a 4 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 14
	 */
	void appendSint32(int32_t value) {
		return appendWord(reinterpret_cast<uint32_t&>(value));
	}

	/**
	 * Adds a 8 byte signed integer to the end of the message
	 *
	 * PTC = 4, PFC = 16
	 */
	void appendSint64(int64_t value) {
		return appendUint64(reinterpret_cast<uint64_t&>(value));
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

	/**
	 * Adds a double to the end of the message
	 */
	void appendDouble(double value) {
		static_assert(sizeof(uint64_t) == sizeof(value), "Double numbers must be 64 bits long");

		return appendUint64(reinterpret_cast<uint64_t&>(value));
	}

	/**
	 * Generic function to append any type of data to the message. The amount of bytes appended is equal to the size of
	 * the @ref T value.
	 *
	 * The data is appended on the current write position (deduced by @ref dataSize)
	 *
	 * Calling this or any of the other `append...` functions for equivalent types is exactly the same.
	 *
	 * @tparam T The type of the value to be appended
	 * @return The value to append
	 */
	template <typename T>
	void append(const T& value);

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
		return (static_cast<uint64_t>(readWord()) << 32) | static_cast<uint64_t>(readWord());
	}

	/**
	 * Fetches an 1-byte signed integer from the current position in the message
	 *
	 * PTC = 4, PFC = 4
	 */
	int8_t readSint8() {
		uint8_t value = readByte();
		return reinterpret_cast<int8_t&>(value);
	}

	/**
	 * Fetches a 2-byte unsigned integer from the current position in the message
	 *
	 * PTC = 4, PFC = 8
	 */
	int16_t readSint16() {
		uint16_t value = readHalfword();
		return reinterpret_cast<int16_t&>(value);
	}

	/**
	 * Fetches a 4-byte unsigned integer from the current position in the message
	 *
	 * PTC = 4, PFC = 14
	 */
	int32_t readSint32() {
		uint32_t value = readWord();
		return reinterpret_cast<int32_t&>(value);
	}

	/**
	 * Fetches a 4-byte unsigned integer from the current position in the message
	 *
	 * PTC = 4, PFC = 14
	 */
	int64_t readSint64() {
		uint64_t value = readUint64();
		return reinterpret_cast<int64_t&>(value);
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
		static_assert(sizeof(uint32_t) == sizeof(float), "Floating point numbers must be 32 bits long");

		uint32_t value = readWord();
		return reinterpret_cast<float&>(value);
	}

	double readDouble() {
		static_assert(sizeof(uint64_t) == sizeof(double), "Double numbers must be 64 bits long");

		uint64_t value = readUint64();
		return reinterpret_cast<double&>(value);
	}

	/**
	 * Generic function to read any type of data from the message. The amount of bytes read is equal to the size of
	 * the @ref T value.
	 *
	 * After the data is read, the message pointer @ref readPosition moves forward so that the next amount of data
	 * can be read.
	 *
	 * Calling this or any of the other `read...` functions for equivalent types is exactly the same.
	 *
	 * @tparam T The type to be read
	 * @return The value that has been read from the string
	 */
	template <typename T>
	T read();
	
	/**
	 * @brief Skip read bytes in the read string
	 * @details Skips the provided number of bytes, by incrementing the readPosition and this is
	 * done to avoid accessing the `readPosition` variable directly
	 * @param numberOfBytes The number of bytes to be skipped
	 */
	void skipBytes(uint16_t numberOfBytes) {
		readPosition += numberOfBytes;
	}
	
	/**
	 * Reset the message reading status, and start reading data from it again
	 */
	void resetRead();

};


template <>
inline void Message::append(const uint8_t& value) {
	appendUint8(value);
}
template <>
inline void Message::append(const uint16_t& value) {
	appendUint16(value);
}
template <>
inline void Message::append(const uint32_t& value) {
	appendUint32(value);
}
template <>
inline void Message::append(const uint64_t& value) {
	appendUint64(value);
}

template <>
inline void Message::append(const int8_t& value) {
	appendSint8(value);
}
template <>
inline void Message::append(const int16_t& value) {
	appendSint16(value);
}
template <>
inline void Message::append(const int32_t& value) {
	appendSint32(value);
}

template <>
inline void Message::append(const bool& value) {
	appendBoolean(value);
}
template <>
inline void Message::append(const char& value) {
	appendByte(value);
}
template <>
inline void Message::append(const float& value) {
	appendFloat(value);
}
template <>
inline void Message::append(const double& value) {
	appendDouble(value);
}

template <>
inline uint8_t Message::read() {
	return readUint8();
}
template <>
inline uint16_t Message::read() {
	return readUint16();
}
template <>
inline uint32_t Message::read() {
	return readUint32();
}
template <>
inline uint64_t Message::read() {
	return readUint64();
}

template <>
inline int8_t Message::read() {
	return readSint8();
}
template <>
inline int16_t Message::read() {
	return readSint16();
}
template <>
inline int32_t Message::read() {
	return readSint32();
}

template <>
inline bool Message::read<bool>() {
	return readBoolean();
}
template <>
inline char Message::read() {
	return readByte();
}
template <>
inline float Message::read() {
	return readFloat();
}
template <>
inline double Message::read() {
	return readDouble();
}

#endif //ECSS_SERVICES_MESSAGE_HPP
