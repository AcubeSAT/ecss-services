#include "Services/StorageAndRetrievalService.hpp"

void StorageAndRetrievalService::initializeStorageAndRetrievalServiceStructures() {
	packetStores.insert({"stats", PacketStore()});
	serviceToPacketStore.insert({4, "stats"});
}
