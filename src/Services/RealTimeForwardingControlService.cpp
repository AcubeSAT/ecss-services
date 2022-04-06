#include <iostream>
#include "Services/RealTimeForwardingControlService.hpp"

bool RealTimeForwardingControlService::appIsControlled(Message& request, uint8_t applicationId) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::checkApplicationOfAppProcessConfig(Message& request, uint8_t applicationID,
                                                                          uint8_t numOfServices) {
	if (not appIsControlled(request, applicationID) or allServiceTypesAllowed(request, applicationID)) {
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
	    not applicationProcessConfiguration.notEmpty[applicationID].empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
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

bool RealTimeForwardingControlService::checkService(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                    uint8_t numOfMessages) {
	if (maxServiceTypesReached(request, applicationID) or allReportTypesAllowed(request, applicationID, serviceType)) {
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::allReportTypesAllowed(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (applicationProcessConfiguration.definitions[applicationID][serviceType].empty() and
	    applicationProcessConfiguration.notEmpty[applicationID][serviceType]) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllReportTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::maxReportTypesReached(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (applicationProcessConfiguration.definitions[applicationID][serviceType].size() >=
	    ECSSMaxReportTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::checkMessage(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                    uint8_t messageType) {
	if (maxReportTypesReached(request, applicationID, serviceType) or
	    reportExistsInAppProcessConfiguration(messageType, applicationID, serviceType)) {
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::reportExistsInAppProcessConfiguration(uint8_t target, uint8_t applicationID,
                                                                             uint8_t serviceType) {
	auto& serviceTypes = applicationProcessConfiguration.definitions[applicationID][serviceType];
	return std::find(serviceTypes.begin(), serviceTypes.end(), target) != serviceTypes.end();
}

void RealTimeForwardingControlService::addReportTypesToAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration);
	uint8_t numOfApplications = request.readUint8();

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not checkApplicationOfAppProcessConfig(request, applicationID, numOfServices)) {
			continue;
		}

		if (numOfServices == 0) {
			applicationProcessConfiguration.definitions[applicationID].clear();
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			// todo: check if service type is valid.
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not checkService(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				applicationProcessConfiguration.definitions[applicationID][serviceType].clear();
				continue;
			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();

				if (not checkMessage(request, applicationID, serviceType, messageType)) {
					continue;
				}

				// todo: check if message type is valid.
				applicationProcessConfiguration.definitions[applicationID][serviceType].push_back(messageType);
				applicationProcessConfiguration.notEmpty[applicationID][serviceType] = true;
			}
		}
	}
}

void RealTimeForwardingControlService::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteReportTypesFromAppProcessConfiguration);

	uint8_t numOfApplications = request.readUint8();
	if (numOfApplications == 0) {
		applicationProcessConfiguration.definitions.clear();
		applicationProcessConfiguration.notEmpty.clear();
		return;
	}

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		// application not present in the application process configuration
		if (applicationProcessConfiguration.definitions.find(applicationID) ==
		    applicationProcessConfiguration.definitions.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingApplication);
			for (uint8_t z = 0; z < numOfServices; z++) {
				request.skipBytes(1);
				uint8_t numOfMessages = request.readUint8();
				request.skipBytes(numOfMessages);
			}
			continue;
		}

		// delete the application process
		if (numOfServices == 0) {
			applicationProcessConfiguration.definitions.erase(applicationID);
			applicationProcessConfiguration.notEmpty.erase(applicationID);
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			// Service type not present in the specified application process definition
			if (applicationProcessConfiguration.definitions[applicationID].find(serviceType) ==
			    applicationProcessConfiguration.definitions[applicationID].end()) {
				ErrorHandler::reportError(request,
				                          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition);
				request.skipBytes(numOfMessages);
				continue;
			}

			// delete service type
			if (numOfMessages == 0) {
				applicationProcessConfiguration.definitions[applicationID].erase(serviceType);
				applicationProcessConfiguration.notEmpty[applicationID].erase(serviceType);
				// if empty application
				if (applicationProcessConfiguration.definitions[applicationID].empty()) {
					applicationProcessConfiguration.definitions.erase(applicationID);
					applicationProcessConfiguration.notEmpty.erase(applicationID);
				}
				continue;
			}

			// if not check service

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();

				if (not reportExistsInAppProcessConfiguration(messageType, applicationID, serviceType)) {
					ErrorHandler::reportError(request,
					                          ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition);
					continue;
				}
				// delete
				auto& reportTypes = applicationProcessConfiguration.definitions[applicationID][serviceType];
				reportTypes.erase(std::remove(reportTypes.begin(), reportTypes.end(), messageType));
				// if service is empty, delete service type.
				if (applicationProcessConfiguration.definitions[applicationID][serviceType].empty()) {
					applicationProcessConfiguration.definitions[applicationID].erase(serviceType);
					applicationProcessConfiguration.notEmpty[applicationID].erase(serviceType);

					// if application is empty, delete application
					if (applicationProcessConfiguration.definitions[applicationID].empty()) {
						applicationProcessConfiguration.definitions.erase(applicationID);
						applicationProcessConfiguration.notEmpty.erase(applicationID);
					}
				}
			}
		}
	}
}

void RealTimeForwardingControlService::execute(Message& message) {
	switch (message.messageType) {
		case AddReportTypesToAppProcessConfiguration:
			addReportTypesToAppProcessConfiguration(message);
			break;
		case DeleteReportTypesFromAppProcessConfiguration:
			deleteReportTypesFromAppProcessConfiguration(message);
			break;
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
