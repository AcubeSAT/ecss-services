#include "Helpers/PacketStore.hpp"

uint16_t PacketStore::calculateSizeInBytes() {
uint16_t sizeInBytes = std::reduce(std::execution::seq, storedTelemetryPackets.cbegin(), storedTelemetryPackets.cend(), 0, [](uint16_t init, const std::pair<uint32_t, Message>& message) { return init + message.second.dataSize;	});
}