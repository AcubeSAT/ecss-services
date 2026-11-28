#include "Services/RealTimeForwardingControlService.hpp"
#include "Helpers/AllReportTypes.hpp"

void RealTimeForwardingControlService::initializeRealTimeForwardingServiceStructures() {
	applicationProcessConfiguration.addAllReportsOfApplication(ApplicationId);
}
