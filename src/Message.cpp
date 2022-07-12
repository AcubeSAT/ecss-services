#include "Message.hpp"
#include <ErrorHandler.hpp>
#include <MessageParser.hpp>
#include <cstring>
#include "ServicePool.hpp"
#include "macros.hpp"


Message::Message(uint8_t serviceType, uint8_t messageType, PacketType packetType, uint16_t applicationId)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(applicationId) {}

Message::Message(uint8_t serviceType, uint8_t messageType, PacketType packetType)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(ApplicationId) {}

void Message::appendBits(uint8_t numBits, uint16_t data) {
	// TODO: Add assertion that data does not contain 1s outside of numBits bits
	ASSERT_INTERNAL(numBits <= 16, ErrorHandler::TooManyBitsAppend);

	while (numBits > 0) { // For every sequence of 8 bits...
		ASSERT_INTERNAL(dataSize < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);

		if ((currentBit + numBits) >= 8) {
			// Will have to shift the bits and insert the next ones later
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit);

			this->data[dataSize] |= static_cast<uint8_t>(data >> (numBits - bitsToAddNow));

			// Remove used bits
			data &= (1 << (numBits - bitsToAddNow)) - 1;
			numBits -= bitsToAddNow;

			currentBit = 0;
			dataSize++;
		} else {
			// Just add the remaining bits
			this->data[dataSize] |= static_cast<uint8_t>(data << (8 - currentBit - numBits));
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
	ASSERT_INTERNAL(dataSize < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits);

	data[dataSize] = value;
	dataSize++;
}

void Message::appendHalfword(uint16_t value) {
	ASSERT_INTERNAL((dataSize + 2) <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits);

	data[dataSize] = static_cast<uint8_t>((value >> 8) & 0xFF);
	data[dataSize + 1] = static_cast<uint8_t>(value & 0xFF);

	dataSize += 2;
}

void Message::appendWord(uint32_t value) {
	ASSERT_INTERNAL((dataSize + 4) <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	ASSERT_INTERNAL(currentBit == 0, ErrorHandler::ByteBetweenBits);

	data[dataSize] = static_cast<uint8_t>((value >> 24) & 0xFF);
	data[dataSize + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
	data[dataSize + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
	data[dataSize + 3] = static_cast<uint8_t>(value & 0xFF);

	dataSize += 4;
}

uint16_t Message::readBits(uint8_t numBits) {
	ASSERT_REQUEST(numBits <= 16, ErrorHandler::TooManyBitsRead);

	uint16_t value = 0x0;

	while (numBits > 0) {
		ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

		if ((currentBit + numBits) >= 8) {
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit);

			uint8_t mask = ((1U << bitsToAddNow) - 1U);
			uint8_t maskedData = data[readPosition] & mask;
			value |= maskedData << (numBits - bitsToAddNow);

			numBits -= bitsToAddNow;
			currentBit = 0;
			readPosition++;
		} else {
			value |= (data[readPosition] >> (8 - currentBit - numBits)) & ((1 << numBits) - 1);
			currentBit = currentBit + numBits;
			numBits = 0;
		}
	}

	return value;
}

uint8_t Message::readByte() {
	ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint8_t value = data[readPosition];
	readPosition++;

	return value;
}

uint16_t Message::readHalfword() {
	ASSERT_REQUEST((readPosition + 2) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint16_t value = (data[readPosition] << 8) | data[readPosition + 1];
	readPosition += 2;

	return value;
}

uint32_t Message::readWord() {
	ASSERT_REQUEST((readPosition + 4) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint32_t value = (data[readPosition] << 24) | (data[readPosition + 1] << 16) | (data[readPosition + 2] << 8) |
	                 data[readPosition + 3];
	readPosition += 4;

	return value;
}

void Message::readString(char* string, uint16_t size) {
	ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);
	ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort);
	std::copy(data + readPosition, data + readPosition + size, string);
	readPosition += size;
}

void Message::readString(uint8_t* string, uint16_t size) {
	ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);
	ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort);
	std::copy(data + readPosition, data + readPosition + size, string);
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
	ASSERT_INTERNAL(dataSize + string.size() <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	// TODO: Do we need to keep this check? How does etl::string handle it?
	ASSERT_INTERNAL(string.size() <= string.capacity(), ErrorHandler::StringTooLarge);
	std::copy(string.data(), string.data() + string.size(), data + dataSize);
	dataSize += string.size();
}

void Message::appendFixedString(const etl::istring& string) {
	ASSERT_INTERNAL((dataSize + string.max_size()) < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	std::copy(string.data(), string.data() + string.size(), data + dataSize);
	(void) memset(data + dataSize + string.size(), 0, string.max_size() - string.size());
	dataSize += string.max_size();
}

void Message::appendOctetString(const etl::istring& string) {
	// Make sure that the string is large enough to count
	ASSERT_INTERNAL(string.size() <= (std::numeric_limits<uint16_t>::max)(), ErrorHandler::StringTooLarge);
	// Redundant check to make sure we fail before appending string.size()
	ASSERT_INTERNAL(dataSize + 2 + string.size() < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);

	appendUint16(string.size());
	appendString(string);
}
