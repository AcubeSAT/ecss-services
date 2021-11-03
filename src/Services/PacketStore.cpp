#include "Services/PacketStore.hpp"

void PacketStore::copyPacketsTo(PacketStore &target, uint32_t startTime, uint32_t endTime) {
	for (auto &packet : storedTmPackets) {
		if (packet.first >= startTime and packet.first <= endTime) {
			target.storedTmPackets.push_back(packet);
		} else {
			break;
		}
	}
}
