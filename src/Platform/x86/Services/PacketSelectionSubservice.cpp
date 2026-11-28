#include "Services/PacketSelectionSubservice.hpp"

void PacketSelectionSubservice::initializePacketSelectionSubServiceStructures() {
	ApplicationProcessConfiguration config;
	config.addAllReportsOfApplication(ApplicationId);
	packetStoreAppProcessConfig.insert({"stats", config});
}
