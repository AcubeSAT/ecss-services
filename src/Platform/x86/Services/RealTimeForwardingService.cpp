#include "Services/RealTimeForwardingControlService.hpp"

void RealTimeForwardingControlService::initializeRealTimeForwardingServiceStructures() {
	Message message;
	applicationProcessConfiguration.addAllReportsOfApplication(message, ApplicationId);
}
