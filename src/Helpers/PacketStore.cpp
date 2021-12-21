#include "Helpers/PacketStore.hpp"

uint16_t PacketStore::calculateSizeInBytes() {
	uint16_t sizeInBytes = 0;
	for (auto& tmPacket : storedTelemetryPackets) {
		sizeInBytes += tmPacket.second.dataSize;
	}
	return sizeInBytes;
}