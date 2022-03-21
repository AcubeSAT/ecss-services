#include <iostream>
#include "Services/RealTimeForwardingControlService.hpp"

bool RealTimeForwardingControlService::appIsControlled(Message& request, uint8_t applicationId) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::ApplicationNotControlled);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::checkApplication1(Message& request, uint8_t applicationID,
                                                         uint8_t numOfServices) {
	if (not appIsControlled(request, applicationID) or allServiceTypesAllowed(request, applicationID) or
	    maxServiceTypesReached(request, applicationID)) {
		for (uint8_t l = 0; l < numOfServices; l++) {
			request.skipBytes(1);
			uint8_t numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::allServiceTypesAllowed(Message& request, uint8_t applicationID) {
	if (applicationProcessConfiguration.definitions[applicationID].empty() and
	    not applicationProcessConfiguration.serviceNotEmpty[applicationID].empty()) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::AdditionOfAllServiceTypesAlreadyEnabled);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::maxServiceTypesReached(Message& request, uint8_t applicationID) {
	if (applicationProcessConfiguration.definitions[applicationID].size() >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::checkService1(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                     uint8_t numOfMessages) {
	if (allReportTypesAllowed(request, applicationID, serviceType) or
	    maxReportTypesReached(request, applicationID, serviceType)) {
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::allReportTypesAllowed(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (applicationProcessConfiguration.definitions[applicationID][serviceType].empty() and
	    applicationProcessConfiguration.serviceNotEmpty[applicationID][serviceType]) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::AdditionOfAllReportTypesAlreadyEnabled);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::maxReportTypesReached(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (applicationProcessConfiguration.definitions[applicationID][serviceType].size() >=
	    ECSSMaxReportTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::reportExistsInAppProcessConfiguration(uint8_t target, uint8_t applicationID,
                                                                             uint8_t serviceType) {
	return std::find(applicationProcessConfiguration.definitions[applicationID][serviceType].begin(),
	                 applicationProcessConfiguration.definitions[applicationID][serviceType].end(),
	                 target) != applicationProcessConfiguration.definitions[applicationID][serviceType].end();
}

void RealTimeForwardingControlService::addReportTypesToAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration);
	uint8_t numOfApplications = request.readUint8();

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not checkApplication1(request, applicationID, numOfServices)) {
			continue;
		}

		//		if (numOfServices == 0) {
		//			// todo: add all report types of the application process to the configuration.
		//			continue;
		//		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			// todo: check if service type is valid.
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not checkService1(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}

			//			if (numOfMessages == 0) {
			//				// todo: add all report types of the service type to the configuration.
			//				continue;
			//			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();
				// todo: check if message type is valid.
				if (reportExistsInAppProcessConfiguration(messageType, applicationID, serviceType)) {
					continue;
				}
				applicationProcessConfiguration.definitions[applicationID][serviceType].push_back(messageType);
				applicationProcessConfiguration.serviceNotEmpty[applicationID][serviceType] = true;
			}
		}
	}
}

void RealTimeForwardingControlService::execute(Message& message) {
	switch (message.messageType) {
		case AddReportTypesToAppProcessConfiguration:
			addReportTypesToAppProcessConfiguration(message);
			break;
			//		case DeleteReportTypesFromAppProcessConfiguration:
			//			deleteReportTypesFromAppProcessConfiguration(message);
			//			break;
			//		case ReportAppProcessConfigurationContent:
			//			reportAppProcessConfigurationContent(message);
			//			break;
			//		case AddStructuresToHousekeepingConfiguration:
			//			addStructuresToHousekeepingConfiguration(message);
			//			break;
			//		case DeleteStructuresFromHousekeepingConfiguration:
			//			deleteStructuresFromHousekeepingConfiguration(message);
			//			break;
			//		case ReportHousekeepingConfigurationContent:
			//			reportHousekeepingConfigurationContent(message);
			//			break;
			//		case AddEventDefinitionsToEventReportConfiguration:
			//			addEventDefinitionsToEventReportConfiguration(message);
			//			break;
			//		case DeleteEventDefinitionsFromEventReportConfiguration:
			//			deleteEventDefinitionsFromEventReportConfiguration(message);
			//			break;
			//		case ReportEventReportConfigurationContent:
			//			reportEventReportConfigurationContent(message);
			//			break;
	}
}
