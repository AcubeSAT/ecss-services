#include "Message.hpp"
#include <ErrorHandler.hpp>
#include <MessageParser.hpp>
#include <cstring>
#include "ServicePool.hpp"
#include "macros.hpp"


Message::Message(ServiceTypeNum serviceType, MessageTypeNum messageType, PacketType packetType, ApplicationProcessId applicationId)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(applicationId) {}

Message::Message(ServiceTypeNum serviceType, MessageTypeNum messageType, PacketType packetType)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(ApplicationId) {}

void Message::appendBits(uint8_t numBits, uint16_t data) {
	// TODO(#271): Add assertion that data does not contain 1s outside of numBits bits
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL(numBits <= 16, ErrorHandler::TooManyBitsAppend)) {
		return;
	}

	while (numBits > 0) { // For every sequence of 8 bits...
		// TODO(#59): Proper error handling if assert fails
		if (not ASSERT_INTERNAL(dataSize < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
			return;
		}

		if ((currentBit + numBits) >= 8) { // NOLINT(cppcoreguidelines-avoid-magic-numbers)
			// Will have to shift the bits and insert the next ones later
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit); // NOLINT(cppcoreguidelines-avoid-magic-numbers)

			this->data[dataSize] |= static_cast<uint8_t>(data >> (numBits - bitsToAddNow));

			// Remove used bits
			data &= (1 << (numBits - bitsToAddNow)) - 1;
			numBits -= bitsToAddNow;

			currentBit = 0;
			dataSize++;
		} else {
			// Just add the remaining bits
			this->data[dataSize] |= static_cast<uint8_t>(data << (8 - currentBit - numBits)); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
			currentBit += numBits;
			numBits = 0;
		}
	}
}

void Message::finalize() {
	// Define the spare field in telemetry and telecommand user data field (7.4.3.2.c and 7.4.4.2.c)
	if (currentBit != 0) {
		currentBit = 0;
		dataSize++;
	}

	if (packetType == PacketType::TM) {
		messageTypeCounter = Services.getAndUpdateMessageTypeCounter(serviceType, messageType);
		packetSequenceCount = Services.getAndUpdatePacketSequenceCounter();
	}
}

void Message::appendByte(uint8_t value) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL(dataSize < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}
	if (not ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits)) {
		return;
	}

	data[dataSize] = value;
	dataSize++;
}

void Message::appendHalfword(uint16_t value) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL((dataSize + 2) <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}
	if (not ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits)) {
		return;
	}

	data[dataSize] = static_cast<uint8_t>((value >> 8) & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	data[dataSize + 1] = static_cast<uint8_t>(value & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)

	dataSize += 2;
}

void Message::appendWord(uint32_t value) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL((dataSize + 4) <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}
	if (not ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits)) {
		return;
	}

	data[dataSize] = static_cast<uint8_t>((value >> 24) & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	data[dataSize + 1] = static_cast<uint8_t>((value >> 16) & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	data[dataSize + 2] = static_cast<uint8_t>((value >> 8) & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	data[dataSize + 3] = static_cast<uint8_t>(value & 0xFF); // NOLINT(cppcoreguidelines-avoid-magic-numbers)

	dataSize += 4;
}

uint16_t Message::readBits(uint8_t numBits) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST(numBits <= 16, ErrorHandler::TooManyBitsRead)) {
		return 0;
	}

	uint16_t value = 0x0;

	while (numBits > 0) {
		// TODO(#59): Proper error handling if assert fails
		if (not ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
			return 0;
		}

		if ((currentBit + numBits) >= 8) { // NOLINT(cppcoreguidelines-avoid-magic-numbers)
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit); // NOLINT(cppcoreguidelines-avoid-magic-numbers)

			uint8_t const mask = ((1U << bitsToAddNow) - 1U);
			uint8_t const maskedData = data[readPosition] & mask; // NOLINT (cppcoreguidelines-init-variables)
			value |= maskedData << (numBits - bitsToAddNow);

			numBits -= bitsToAddNow;
			currentBit = 0;
			readPosition++;
		} else {
			value |= (data[readPosition] >> (8 - currentBit - numBits)) & ((1 << numBits) - 1); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
			currentBit = currentBit + numBits;
			numBits = 0;
		}
	}

	return value;
}

uint8_t Message::readByte() {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
		return 0;
	}

	uint8_t const value = data[readPosition]; // NOLINT(cppcoreguidelines-init-variables)
	readPosition++;

	return value;
}

uint16_t Message::readHalfword() {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST((readPosition + 2) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
		return 0;
	}

	uint16_t const value = (data[readPosition] << 8) | data[readPosition + 1]; // NOLINT (cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-init-variables)
	readPosition += 2;

	return value;
}

uint32_t Message::readWord() {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST((readPosition + 4) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
		return 0;
	}

	uint32_t const value = (data[readPosition] << 24) | (data[readPosition + 1] << 16) | (data[readPosition + 2] << 8) | // NOLINT(cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-init-variables)
	                 data[readPosition + 3];
	readPosition += 4;

	return value;
}

void Message::readString(char* string, uint16_t size) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
		return;
	}
	if (not ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort)) {
		return;
	}
	std::copy(data.begin() + readPosition, data.begin() + readPosition + size, string);
	readPosition += size;
}

void Message::readString(uint8_t* string, uint16_t size) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort)) {
		return;
	}
	if (not ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort)) {
		return;
	}
	std::copy(data.begin() + readPosition, data.begin() + readPosition + size, string);
	readPosition += size;
}

void Message::readCString(char* string, uint16_t size) {
	readString(string, size);
	string[size] = 0;
}

void Message::resetRead() {
	readPosition = 0;
	currentBit = 0;
}

void Message::appendMessage(const Message& message, uint16_t size) {
	appendString(MessageParser::composeECSS(message, size));
}

void Message::appendString(const etl::istring& string) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL(dataSize + string.size() <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}
	// TODO(#272): Do we need to keep this check? How does etl::string handle it?
	if (not ASSERT_INTERNAL(string.size() <= string.capacity(), ErrorHandler::StringTooLarge)) {
		return;
	}
	std::copy(string.data(), string.data() + string.size(), data.begin() + dataSize);
	dataSize += string.size();
}

void Message::appendFixedString(const etl::istring& string) {
	// TODO(#59): Proper error handling if assert fails
	if (not ASSERT_INTERNAL((dataSize + string.max_size()) < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}
	std::copy(string.data(), string.data() + string.size(), data.begin() + dataSize);
	(void) memset(data.begin() + dataSize + string.size(), 0, string.max_size() - string.size());
	dataSize += string.max_size();
}

void Message::appendOctetString(const etl::istring& string) {
	// TODO(#59): Proper error handling if assert fails
	// Make sure that the string is large enough to count
	if (not ASSERT_INTERNAL(string.size() <= (std::numeric_limits<uint16_t>::max)(), ErrorHandler::StringTooLarge)) {
		return;
	}
	// Redundant check to make sure we fail before appending string.size()
	if (not ASSERT_INTERNAL(dataSize + 2 + string.size() < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge)) {
		return;
	}

	appendUint16(string.size());
	appendString(string);
}
