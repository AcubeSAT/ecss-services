#ifndef ECSS_SERVICES_ECSSMESSAGE_H
#define ECSS_SERVICES_ECSSMESSAGE_H

#include <Time/TimeStamp.hpp>
#include <cstdint>
#include <etl/String.hpp>
#include <etl/wstring.h>
#include "ECSS_Definitions.hpp"
#include "Message.hpp"
#include "Time/Time.hpp"
#include "macros.hpp"

/**
 * A telemetry (TM) or telecommand (TC) message (request/report), as specified in ECSS-E-ST-70-41C
 *
 * @todo Make sure that a message can't be written to or read from at the same time, or make
 *       readable and writable message different classes
 */
class ECSSMessage : public Message {
public:
	static constexpr size_t MaxMessageSize = ECSSMaxMessageSize;

	ECSSMessage() = default;

	/**
	 * @brief Compare two messages
	 * @details Check whether two ECSSMessage objects are of the same type
	 * @param msg1 First message for comparison
	 * @param msg2 Second message for comparison
	 * @return A boolean value indicating whether the messages are of the same type
	 */
	static bool isSameType(const ECSSMessage& msg1, const ECSSMessage& msg2) {
		return (msg1.packetType == msg2.packetType) && (msg1.messageType == msg2.messageType) &&
		       (msg1.serviceType == msg2.serviceType);
	}

	/**
	 * @brief Overload the equality operator to compare messages
	 * @details Compare two @ref ::ECSSMessage objects, based on their contents and type
	 * @param msg The message content to compare against
	 * @return The result of comparison
	 */
	bool operator==(const ECSSMessage& msg) const {
		if (dataSize != msg.dataSize) {
			return false;
		}

		if (not isSameType(*this, msg)) {
			return false;
		}

		return std::equal(data, data + dataSize, msg.data);
	}

	/**
	 * Checks the first \ref ECSSMessage::dataSize bytes of \p msg for equality
	 *
	 * This performs an equality check for the first `[0, this->dataSize)` bytes of two messages. Useful to compare
	 * two messages that have the same content, but one of which does not know its length.
	 *
	 * @param msg The message to check. Its `dataSize` must be smaller than the object calling the function
	 * @return False if the messages are not of the same type, if `msg.dataSize < this->dataSize`, or if the first
	 * `this->dataSize` bytes are not equal between the two messages.
	 */
	bool bytesEqualWith(const ECSSMessage& msg) const {
		if (msg.dataSize < dataSize) {
			return false;
		}

		if (not isSameType(*this, msg)) {
			return false;
		}

		return std::equal(data, data + dataSize, msg.data);
	}

	enum PacketType {
		TM = 0, ///< Telemetry
		TC = 1  ///< Telecommand
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

	//> 7.4.3.1b
	uint16_t messageTypeCounter = 0;

	// 7.4.1, as defined in CCSDS 133.0-B-1
	uint16_t packetSequenceCount = 0;

	/**
	 * Appends the least significant \p numBits from \p data to the message
	 *
	 * Note: data MUST NOT contain any information beyond the most significant \p numBits bits
	 */
	void appendBits(uint8_t numBits, uint16_t data);

	/**
	 * Appends the remaining bits to complete a byte, in case the appendBits() is the last call
	 * and the packet data field isn't integer multiple of bytes
	 *
	 * @note Actually we should append the bits so the total length of the packets is an integer
	 * multiple of the padding word size declared for the application process
	 * @todo Confirm that the overall packet size is an integer multiple of the padding word size
	 * declared for every application process
	 * @todo check if we need to define the spare field for the telemetry and telecommand
	 * secondary headers
	 */
	void finalize();

	// Pointer to the contents of the message (excluding the PUS header)
	// We allocate this data statically, in order to make sure there is predictability in the
	// handling and storage of messages
	//
	// @note This is initialized to 0 in order to prevent any mishaps with non-properly initialized values. \ref
	// ECSSMessage::appendBits() relies on this in order to easily OR the requested bits.
	uint8_t data[MaxMessageSize] = {0};

	/**
	 * Appends any CUC timestamp to the message, including the header.
	 */
	template <class Ts>
	void appendCUCTimeStamp(const Ts& timestamp) {
		etl::array<uint8_t, Time::CUCTimestampMaximumSize> text = timestamp.formatAsCUC();

		appendString(String<Time::CUCTimestampMaximumSize>(text.data(), text.size()));
	}

	/**
	 * Appends a default timestamp object to the message, without the header
	 */
	void appendDefaultCUCTimeStamp(Time::DefaultCUC timestamp) {
		static_assert(std::is_same_v<uint32_t, decltype(timestamp.formatAsBytes())>, "The default timestamp should be 4 bytes");
		appendUint32(timestamp.formatAsBytes());
	}

	/**
	 * Appends a number of bytes to the message
	 *
	 * Note that this doesn't append the number of bytes that the string contains. For this, you
	 * need to use a function like ECSSMessage::appendOctetString(), or have specified the size of the
	 * string beforehand. Note that the standard does not support null-terminated strings.
	 *
	 * This does not append the full size of the string, just its current size. Use
	 * ECSSMessage::appendFixedString() to have a constant number of characters added.
	 *
	 * @param string The string to insert
	 */
	void appendString(const etl::istring& string);

	/**
	 * Appends a number of bytes to the message
	 *
	 * Note that this doesn't append the number of bytes that the string contains. For this, you
	 * need to use a function like ECSSMessage::appendOctetString(), or have specified the size of the
	 * string beforehand. Note that the standard does not support null-terminated strings.
	 *
	 * The number of bytes appended is equal to \p SIZE. To append variable-sized parameters, use
	 * ECSSMessage::appendString() instead. Missing bytes are padded with zeros, until the length of SIZE
	 * is reached.
	 *
	 * @param string The string to insert
	 */
	void appendFixedString(const etl::istring& string);
	/**
	 * Reads the next \p size bytes from the message, and stores them into the allocated \p string
	 *
	 * NOTE: We assume that \p string is already allocated, and its size is at least
	 * ECSS_MAX_STRING_SIZE. This function does NOT place a \0 at the end of the created string.
	 */
	void readString(char* string, uint16_t size);

	/**
	 * Reads the next \p size bytes from the message, and stores them into the allocated \p string
	 *
	 * NOTE: We assume that \p string is already allocated, and its size is at least
	 * ECSS_MAX_STRING_SIZE. This function does NOT place a \0 at the end of the created string
	 * @todo Is uint16_t size too much or not enough? It has to be defined
	 */
	void readString(uint8_t* string, uint16_t size);

	/**
	 * Reads the next \p size bytes from the message, and stores them into the allocated \p string
	 *
	 * NOTE: We assume that \p string is already allocated, and its size is at least
	 * ECSS_MAX_STRING_SIZE + 1. This function DOES place a \0 at the end of the created string,
	 * meaning that \p string should contain 1 more byte than the string stored in the message.
	 */
	void readCString(char* string, uint16_t size);

public:
	ECSSMessage(uint8_t serviceType, uint8_t messageType, PacketType packetType, uint16_t applicationId);
	ECSSMessage(uint8_t serviceType, uint8_t messageType, ECSSMessage::PacketType packetType);

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
	 * Adds an 8 byte time Offset to the message
	 */
	void appendRelativeTime(Time::RelativeTime value) {
		return appendSint64(value);
	}

	/**
	 * Adds a N-byte string to the end of the message
	 *
	 *
	 * PTC = 7, PFC = 0
	 */
	void appendOctetString(const etl::istring& string);

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
	 * Adds a nested TC or TM ECSSMessage within the current ECSSMessage
	 *
	 * As a design decision, nested TC & TM Messages always have a fixed width, specified in \ref ECSSDefinitions. This
	 * reduces the uncertainty and complexity of having to parse the nested ECSSMessage itself to see how long it is, at
	 * the cost of more data to be transmitted.
	 * @param message The message to append
	 * @param size The fixed number of bytes that the message will take up. The empty last bytes are padded with 0s.
	 */
	void appendMessage(const ECSSMessage& message, uint16_t size);

	/**
	 * Fetches an 8 byte time Offset from the current position in the message
	 */
	Time::RelativeTime readRelativeTime() {
		return readSint64();
	};

	/**
	 * Fetches a timestamp in a custom CUC format consisting of 4 bytes from the current position in the message
	 */
	Time::DefaultCUC readDefaultCUCTimeStamp() {
		auto time = readUint32();
		std::chrono::duration<uint32_t, Time::DefaultCUC::Ratio> duration(time);

		return Time::DefaultCUC(duration);
	}

	/**
	 * Fetches a N-byte string from the current position in the message
	 *
	 * In the current implementation we assume that a preallocated array of sufficient size
	 * is provided as the argument. This does NOT append a trailing `\0` to \p byteString.
	 * @todo Specify if the provided array size is too small or too large
	 *
	 * PTC = 7, PFC = 0
	 */
	uint16_t readOctetString(uint8_t* byteString) {
		uint16_t size = readUint16(); // Get the data length from the message
		readString(byteString, size); // Read the string data

		return size; // Return the string size
	}

	/**
	 * Fetches an N-byte string from the current position in the message. The string can be at most MAX_SIZE long.
	 *
	 * @note This function was not implemented as ECSSMessage::read() due to an inherent C++ limitation, see
	 * https://www.fluentcpp.com/2017/08/15/function-templates-partial-specialization-cpp/
	 * @tparam MAX_SIZE The memory size of the string in bytes, which corresponds to the max string size
	 */
	template <const size_t MAX_SIZE>
	String<MAX_SIZE> readOctetString() {
		String<MAX_SIZE> string("");

		uint16_t length = readUint16();
		ASSERT_REQUEST(length <= string.max_size(), ErrorHandler::StringTooShort);
		ASSERT_REQUEST((readPosition + length) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

		string.append(data + readPosition, length);
		readPosition += length;

		return std::move(string);
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
	 * Compare the message type to an expected one. An unexpected message type will throw an
	 * OtherMessageType error.
	 *
	 * @return True if the message is of correct type, false if not
	 */
	bool assertType(ECSSMessage::PacketType expectedPacketType, uint8_t expectedServiceType, uint8_t expectedMessageType) {
		bool status = true;

		if ((packetType != expectedPacketType) || (serviceType != expectedServiceType) ||
		    (messageType != expectedMessageType)) {
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			status = false;
		}

		return status;
	}

	/**
	 * Alias for ECSSMessage::assertType(ECSSMessage::TC, \p expectedServiceType, \p
	 * expectedMessageType)
	 */
	bool assertTC(uint8_t expectedServiceType, uint8_t expectedMessageType) {
		return assertType(TC, expectedServiceType, expectedMessageType);
	}

	/**
	 * Alias for ECSSMessage::assertType(ECSSMessage::TM, \p expectedServiceType, \p
	 * expectedMessageType)
	 */
	bool assertTM(uint8_t expectedServiceType, uint8_t expectedMessageType) {
		return assertType(TM, expectedServiceType, expectedMessageType);
	}
};

template <>
inline void ECSSMessage::append(const uint8_t& value) {
	appendUint8(value);
}
template <>
inline void ECSSMessage::append(const uint16_t& value) {
	appendUint16(value);
}
template <>
inline void ECSSMessage::append(const uint32_t& value) {
	appendUint32(value);
}
template <>
inline void ECSSMessage::append(const uint64_t& value) {
	appendUint64(value);
}

template <>
inline void ECSSMessage::append(const int8_t& value) {
	appendSint8(value);
}
template <>
inline void ECSSMessage::append(const int16_t& value) {
	appendSint16(value);
}
template <>
inline void ECSSMessage::append(const int32_t& value) {
	appendSint32(value);
}

template <>
inline void ECSSMessage::append(const bool& value) {
	appendBoolean(value);
}
template <>
inline void ECSSMessage::append(const char& value) {
	appendByte(value);
}
template <>
inline void ECSSMessage::append(const float& value) {
	appendFloat(value);
}
template <>
inline void ECSSMessage::append(const double& value) {
	appendDouble(value);
}
template <>
inline void ECSSMessage::append(const Time::DefaultCUC& timeCUC) {
	appendDefaultCUCTimeStamp(timeCUC);
}
template <>
inline void ECSSMessage::append(const Time::RelativeTime& value) {
	appendRelativeTime(value);
}

/**
 * Appends an ETL string to the message. ETL strings are handled as ECSS octet strings, meaning that the string size
 * is appended as a byte before the string itself. To append other string sequences, see the ECSSMessage::appendString()
 * functions
 */
template <>
inline void ECSSMessage::append(const etl::istring& value) {
	appendOctetString(value);
}

template <>
inline uint8_t ECSSMessage::read() {
	return readUint8();
}
template <>
inline uint16_t ECSSMessage::read() {
	return readUint16();
}
template <>
inline uint32_t ECSSMessage::read() {
	return readUint32();
}
template <>
inline uint64_t ECSSMessage::read() {
	return readUint64();
}

template <>
inline int8_t ECSSMessage::read() {
	return readSint8();
}
template <>
inline int16_t ECSSMessage::read() {
	return readSint16();
}
template <>
inline int32_t ECSSMessage::read() {
	return readSint32();
}

template <>
inline bool ECSSMessage::read<bool>() {
	return readBoolean();
}
template <>
inline char ECSSMessage::read() {
	return readByte();
}
template <>
inline float ECSSMessage::read() {
	return readFloat();
}
template <>
inline double ECSSMessage::read() {
	return readDouble();
}
template <>
inline Time::DefaultCUC ECSSMessage::read() {
	return readDefaultCUCTimeStamp();
}
template <>
inline Time::RelativeTime ECSSMessage::read() {
	return readRelativeTime();
}

#endif // ECSS_SERVICES_ECSSMESSAGE_H
