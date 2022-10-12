#include "ECSSMessage.hpp"
#include <ErrorHandler.hpp>
#include <MessageParser.hpp>
#include <cstring>
#include "ServicePool.hpp"
#include "macros.hpp"


ECSSMessage::ECSSMessage(uint8_t serviceType, uint8_t messageType, PacketType packetType, uint16_t applicationId)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(applicationId) {}

ECSSMessage::ECSSMessage(uint8_t serviceType, uint8_t messageType, PacketType packetType)
    : serviceType(serviceType), messageType(messageType), packetType(packetType), applicationId(ApplicationId) {}

void ECSSMessage::appendBits(uint8_t numBits, uint16_t data) {
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

void ECSSMessage::finalize() {
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

void ECSSMessage::readString(char* string, uint16_t size) {
	ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);
	ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort);
	std::copy(data + readPosition, data + readPosition + size, string);
	readPosition += size;
}

void ECSSMessage::readString(uint8_t* string, uint16_t size) {
	ASSERT_REQUEST((readPosition + size) <= ECSSMaxMessageSize, ErrorHandler::MessageTooShort);
	ASSERT_REQUEST(size < ECSSMaxStringSize, ErrorHandler::StringTooShort);
	std::copy(data + readPosition, data + readPosition + size, string);
	readPosition += size;
}

void ECSSMessage::readCString(char* string, uint16_t size) {
	readString(string, size);
	string[size] = 0;
}

void ECSSMessage::appendMessage(const ECSSMessage& message, uint16_t size) {
	appendString(MessageParser::composeECSS(message, size));
}

void ECSSMessage::appendString(const etl::istring& string) {
	ASSERT_INTERNAL(dataSize + string.size() <= ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	// TODO: Do we need to keep this check? How does etl::string handle it?
	ASSERT_INTERNAL(string.size() <= string.capacity(), ErrorHandler::StringTooLarge);
	std::copy(string.data(), string.data() + string.size(), data + dataSize);
	dataSize += string.size();
}

void ECSSMessage::appendFixedString(const etl::istring& string) {
	ASSERT_INTERNAL((dataSize + string.max_size()) < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);
	std::copy(string.data(), string.data() + string.size(), data + dataSize);
	(void) memset(data + dataSize + string.size(), 0, string.max_size() - string.size());
	dataSize += string.max_size();
}

void ECSSMessage::appendOctetString(const etl::istring& string) {
	// Make sure that the string is large enough to count
	ASSERT_INTERNAL(string.size() <= (std::numeric_limits<uint16_t>::max)(), ErrorHandler::StringTooLarge);
	// Redundant check to make sure we fail before appending string.size()
	ASSERT_INTERNAL(dataSize + 2 + string.size() < ECSSMaxMessageSize, ErrorHandler::MessageTooLarge);

	appendUint16(string.size());
	appendString(string);
}
