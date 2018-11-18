#include "Message.hpp"
#include <cstring>
#include <Message.hpp>


Message::Message(uint8_t serviceType, uint8_t messageType, Message::PacketType packetType,
                 uint16_t applicationId) : serviceType(serviceType), messageType(messageType),
                                           packetType(packetType), applicationId(applicationId) {}

void Message::appendBits(uint8_t numBits, uint16_t data) {
	// TODO: Add assertion that data does not contain 1s outside of numBits bits
	assert(numBits <= 16);

	while (numBits > 0) { // For every sequence of 8 bits...
		assert(dataSize < ECSS_MAX_MESSAGE_SIZE);

		if (currentBit + numBits >= 8) {
			// Will have to shift the bits and insert the next ones later
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit);

			this->data[dataSize] |= static_cast<uint8_t>(data >> (numBits - bitsToAddNow));

			// Remove used bits
			data &= (1 << bitsToAddNow) - 1;
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

void Message::appendByte(uint8_t value) {
	assert(dataSize < ECSS_MAX_MESSAGE_SIZE);
	assert(currentBit == 0);

	data[dataSize] = value;
	dataSize++;
}

void Message::appendHalfword(uint16_t value) {
	assert(dataSize + 2 <= ECSS_MAX_MESSAGE_SIZE);
	assert(currentBit == 0);

	data[dataSize] = static_cast<uint8_t>((value >> 8) & 0xFF);
	data[dataSize + 1] = static_cast<uint8_t>(value & 0xFF);

	dataSize += 2;
}

void Message::appendWord(uint32_t value) {
	assert(dataSize + 4 <= ECSS_MAX_MESSAGE_SIZE);
	assert(currentBit == 0);

	data[dataSize] = static_cast<uint8_t>((value >> 24) & 0xFF);
	data[dataSize + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
	data[dataSize + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
	data[dataSize + 3] = static_cast<uint8_t>(value & 0xFF);

	dataSize += 4;
}

void Message::appendString(uint8_t size, const char *value) {
	assert(dataSize + size <= ECSS_MAX_MESSAGE_SIZE);
	assert(size < ECSS_MAX_STRING_SIZE);

	memcpy(data + dataSize, value, size);

	dataSize += size;
}

uint16_t Message::readBits(uint8_t numBits) {
	assert(numBits <= 16);
	// TODO: Add assert

	uint16_t value = 0x0;

	while (numBits > 0) {
		assert(readPosition < ECSS_MAX_MESSAGE_SIZE);

		if (currentBit + numBits >= 8) {
			auto bitsToAddNow = static_cast<uint8_t>(8 - currentBit);

			auto maskedData = static_cast<uint8_t>(data[readPosition] & ((1 << bitsToAddNow) - 1));
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
	assert(readPosition < ECSS_MAX_MESSAGE_SIZE);

	uint8_t value = data[readPosition];
	readPosition++;

	return value;
}

uint16_t Message::readHalfword() {
	assert(readPosition + 2 < ECSS_MAX_MESSAGE_SIZE);

	uint16_t value = (data[readPosition] << 8) | data[readPosition + 1];
	readPosition += 2;

	return value;
}

uint32_t Message::readWord() {
	assert(readPosition + 4 < ECSS_MAX_MESSAGE_SIZE);

	uint32_t value = (data[readPosition] << 24) | (data[readPosition + 1] << 16) |
	                 (data[readPosition + 2] << 8) | data[readPosition + 3];
	readPosition += 4;

	return value;
}

void Message::readString(char *string, uint8_t size) {
	assert(readPosition + size <= ECSS_MAX_MESSAGE_SIZE);
	assert(size < ECSS_MAX_STRING_SIZE);

	memcpy(string, data + readPosition, size);
	string[size] = '\0';

	readPosition += size;
}

void Message::resetRead() {
	readPosition = 0;
	currentBit = 0;
}
