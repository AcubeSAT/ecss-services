#include "Services/RealTimeForwardingControlService.hpp"
#include "Helpers/AllReportTypes.hpp"

void RealTimeForwardingControlService::initializeRealTimeForwardingServiceStructures() {
	Message message;
	applicationProcessConfiguration.addAllReportsOfApplication(message, ApplicationId);
	controlledApplications.push_back(ApplicationId);
}
