#include "Helpers/PacketStore.hpp"

uint16_t PacketStore::calculateSizeInBytes() {
	uint16_t size = 0;
	for (auto& tmPacket : storedTelemetryPackets) {
		size += tmPacket.second.dataSize;
	}
	return size;
}
