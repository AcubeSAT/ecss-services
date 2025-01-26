#include "Services/StorageAndRetrievalService.hpp"

void StorageAndRetrievalService::initializeStorageAndRetrievalServiceStructures() {
	PacketStore packetStore;
	packetStore.storageEnabled = true;
	packetStores.insert({"stats", packetStore});
	serviceToPacketStore.insert({1, "stats"});
	serviceToPacketStore.insert({3, "stats"});
	serviceToPacketStore.insert({4, "stats"});
	serviceToPacketStore.insert({5, "stats"});
	serviceToPacketStore.insert({6, "stats"});
	serviceToPacketStore.insert({11, "stats"});
	serviceToPacketStore.insert({12, "stats"});
	serviceToPacketStore.insert({13, "stats"});
	serviceToPacketStore.insert({14, "stats"});
	serviceToPacketStore.insert({15, "stats"});
	serviceToPacketStore.insert({17, "stats"});
	serviceToPacketStore.insert({19, "stats"});
	serviceToPacketStore.insert({20, "stats"});
	serviceToPacketStore.insert({23, "stats"});
}
