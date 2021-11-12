#include "Services/PacketStore.hpp"

bool PacketStore::copyPacketsTo(PacketStore &target,
                                uint32_t startTime,
                                uint32_t endTime,
                                bool beforeTimeTag,
                                bool afterTimeTag,
                                bool fromTagToTag) {
	if ((beforeTimeTag and afterTimeTag) or (beforeTimeTag and fromTagToTag) or (afterTimeTag and fromTagToTag)) {
		return false;
	}
	if (fromTagToTag) {
		for (auto &packet : storedTmPackets) {
			if (packet.first < startTime) {
				continue;
			}
			if (packet.first > endTime) {
				break;
			}
			target.storedTmPackets.push_back(packet);
		}
	} else if (afterTimeTag) {
		for (auto &packet : storedTmPackets) {
			if (packet.first < startTime) {
				continue;
			}
			target.storedTmPackets.push_back(packet);
		}
	} else {
		for (auto &packet : storedTmPackets) {
			if (packet.first > endTime) {
				break;
			}
			target.storedTmPackets.push_back(packet);
		}
	}
	return true;
}