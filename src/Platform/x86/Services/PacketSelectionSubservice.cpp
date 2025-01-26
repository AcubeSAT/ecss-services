#include "Services/PacketSelectionSubservice.hpp"

void PacketSelectionSubservice::initializePacketSelectionSubServiceStructures() {
	ApplicationProcessConfiguration config;
	Message message;
	config.addAllReportsOfApplication(message, ApplicationId);
	packetStoreAppProcessConfig.insert({"stats", config});
	controlledApplications.push_back(ApplicationId);
}
