#include "Helpers/PacketStore.hpp"

uint16_t PacketStore::calculateSizeInBytes() {
	const uint16_t size = std::accumulate(storedTelemetryPackets.begin(), storedTelemetryPackets.end(), 0, [] // NOLINT (cppcoreguidelines-init-variables)
	                                      (uint16_t sum, const auto& tmPacket) {
		return sum + tmPacket.second.dataSize;
	});
	return size;
}
