#include "Services/RealTimeForwardingControlService.hpp"

bool RealTimeForwardingControlService::appIsControlled(uint8_t applicationId) {
	return std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) !=
	       controlledApplications.end();
}

void RealTimeForwardingControlService::execute(Message& message) {
	switch (message.messageType) {
		case AddReportTypesToAppProcessConfiguration:
			addReportTypesToAppProcessConfiguration(message);
			break;
		case DeleteReportTypesFromAppProcessConfiguration:
			deleteReportTypesFromAppProcessConfiguration(message);
			break;
		case ReportAppProcessConfigurationContent:
			reportAppProcessConfigurationContent(message);
			break;
		case AddStructuresToHousekeepingConfiguration:
			addStructuresToHousekeepingConfiguration(message);
			break;
		case DeleteStructuresFromHousekeepingConfiguration:
			deleteStructuresFromHousekeepingConfiguration(message);
			break;
		case ReportHousekeepingConfigurationContent:
			reportHousekeepingConfigurationContent(message);
			break;
		case AddEventDefinitionsToEventReportConfiguration:
			addEventDefinitionsToEventReportConfiguration(message);
			break;
		case DeleteEventDefinitionsFromEventReportConfiguration:
			deleteEventDefinitionsFromEventReportConfiguration(message);
			break;
		case ReportEventReportConfigurationContent:
			reportEventReportConfigurationContent(message);
			break;
	}
}
