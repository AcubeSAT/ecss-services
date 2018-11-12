#include "Message.hpp"
#include <cstring>

void Message::appendBits(uint8_t numBits, uint16_t data) {
    assert(dataSize < ECSS_MAX_MESSAGE_SIZE);
    assert(numBits < 16);

    while (numBits > 0) { // For every sequence of 8 bits...
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

    memcpy(data + dataSize, value, size);

    dataSize += size;
}

void Message::appendEnumerated(uint8_t bits, uint32_t value) {
    // TODO: Implement 32-bit enums, if needed
    assert((value & 0xffff0000) != 0);

    return appendBits(bits, value);
}

Message::Message(uint8_t serviceType, uint8_t messageType, Message::PacketType packetType,
                 uint16_t applicationId) : serviceType(serviceType), messageType(messageType),
                                           packetType(packetType), applicationId(applicationId) {}
