#include <iostream>
#include "Services/RealTimeForwardingControlService.hpp"

uint8_t RealTimeForwardingControlService::countServicesOfApplication(uint8_t applicationID) {
	uint8_t serviceCounter = 0;
	for (auto& definition : applicationProcessConfiguration.definitions) {
		auto& pair = definition.first;
		if (pair.first == applicationID) {
			serviceCounter++;
		}
	}
	return serviceCounter;
}

uint8_t RealTimeForwardingControlService::countReportsOfService(uint8_t applicationID, uint8_t serviceType) {
	return applicationProcessConfiguration.definitions[std::make_pair(applicationID, serviceType)].size();
}

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
	if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::maxServiceTypesReached(Message& request, uint8_t applicationID) {
	if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
//		std::cout<<"err22\n";
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
//	std::cout<<"num mess= "<<static_cast<int>(countReportsOfService(applicationID, serviceType))<<"\n";
	if (countReportsOfService(applicationID, serviceType) >= allMessageTypes.messagesOfService[serviceType].size()) {
//		std::cout<<"err23\n";
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllReportTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::maxReportTypesReached(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (countReportsOfService(applicationID, serviceType) >= allMessageTypes.messagesOfService[serviceType].size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::checkMessage(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                    uint8_t messageType) {
	if (maxReportTypesReached(request, applicationID, serviceType) or
	    reportExistsInAppProcessConfiguration(applicationID, serviceType, messageType)) {
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::reportExistsInAppProcessConfiguration(uint8_t applicationID, uint8_t serviceType,
                                                                             uint8_t messageType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return std::find(applicationProcessConfiguration.definitions[appServicePair].begin(),
	                 applicationProcessConfiguration.definitions[appServicePair].end(),
	                 messageType) != applicationProcessConfiguration.definitions[appServicePair].end();
}

void RealTimeForwardingControlService::addReportTypesToAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration);
	uint8_t numOfApplications = request.readUint8();
//	std::cout << static_cast<int>(numOfApplications) << "\n";

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();
//		std::cout << "app= " << static_cast<int>(applicationID) << "\n";

		if (not checkApplicationOfAppProcessConfig(request, applicationID, numOfServices)) {
//			std::cout << "err1\n";
			continue;
		}

		//		if (numOfServices == 0) {
		//			applicationProcessConfiguration.definitions[applicationID].clear();
		//			continue;
		//		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			// todo: check if service type is valid.
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();
//			std::cout << "serv= " << static_cast<int>(serviceType) << "\n";
//			std::cout << "size= " << static_cast<int>(countServicesOfApplication(applicationID)) << "\n";

			if (not checkService(request, applicationID, serviceType, numOfMessages)) {
//				std::cout << "err2\n";
				continue;
			}

			//			if (numOfMessages == 0) {
			//				applicationProcessConfiguration.definitions[applicationID][serviceType].clear();
			//				continue;
			//			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();
//				std::cout << "mess= " << static_cast<int>(messageType) << "\n";

				if (not checkMessage(request, applicationID, serviceType, messageType)) {
//					std::cout << "err3\n";
					continue;
				}
				// todo: check if message type is valid.
				applicationProcessConfiguration.definitions[std::make_pair(applicationID, serviceType)].push_back(
				    messageType);
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
