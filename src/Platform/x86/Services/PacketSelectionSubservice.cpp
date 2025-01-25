#include "Services/PacketSelectionSubservice.hpp"

void PacketSelectionSubservice::initializePacketSelectionSubServiceStructures() {
	ApplicationProcessConfiguration config;
	packetStoreAppProcessConfig.insert({"stats", config});
	controlledApplications.push_back(ApplicationId);
}
