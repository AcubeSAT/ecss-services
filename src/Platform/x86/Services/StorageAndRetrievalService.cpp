#include "Services/StorageAndRetrievalService.hpp"

void StorageAndRetrievalService::initializeStorageAndRetrievalServiceStructures() {
	PacketStore packetStore;
	packetStore.storageEnabled = true;
	packetStores.insert({"stats", packetStore});
}
