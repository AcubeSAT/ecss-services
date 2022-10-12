#include "Message.hpp"
#include "ErrorHandler.hpp"
#include "macros.hpp"


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
	//TODO Fix error handling and use asserts
//	ASSERT_REQUEST(numBits <= 16, ErrorHandler::TooManyBitsRead);

	uint16_t value = 0x0;

	while (numBits > 0) {
//		ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

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
//	ASSERT_REQUEST(readPosition < ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint8_t value = data[readPosition];
	readPosition++;

	return value;
}

uint16_t Message::readHalfword() {
//	ASSERT_REQUEST((readPosition + 2) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint16_t value = (data[readPosition] << 8) | data[readPosition + 1];
	readPosition += 2;

	return value;
}

uint32_t Message::readWord() {
//	ASSERT_REQUEST((readPosition + 4) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);

	uint32_t value = (data[readPosition] << 24) | (data[readPosition + 1] << 16) | (data[readPosition + 2] << 8) |
	                 data[readPosition + 3];
	readPosition += 4;

	return value;
}

void Message::resetRead() {
	readPosition = 0;
	currentBit = 0;
}
