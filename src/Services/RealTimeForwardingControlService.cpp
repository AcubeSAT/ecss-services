#include "Services/RealTimeForwardingControlService.hpp"
#include <iostream>

void RealTimeForwardingControlService::addAllReportsOfApplication(uint8_t applicationID) {
	for (const auto& service: AllMessageTypes::MessagesOfService) {
		uint8_t serviceType = service.first;
		addAllReportsOfService(applicationID, serviceType);
	}
}

void RealTimeForwardingControlService::addAllReportsOfService(uint8_t applicationID, uint8_t serviceType) {
	for (const auto& messageType: AllMessageTypes::MessagesOfService.at(serviceType)) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		applicationProcessConfiguration.definitions[appServicePair].push_back(messageType);
	}
}

uint8_t RealTimeForwardingControlService::countServicesOfApplication(uint8_t applicationID) {
	uint8_t serviceCounter = 0;
	for (auto& definition: applicationProcessConfiguration.definitions) {
		const auto& pair = definition.first;
		if (pair.first == applicationID) {
			serviceCounter++;
		}
	}
	return serviceCounter;
}

uint8_t RealTimeForwardingControlService::countReportsOfService(uint8_t applicationID, uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return applicationProcessConfiguration.definitions[appServicePair].size();
}

bool RealTimeForwardingControlService::checkAppControlled(Message& request, uint8_t applicationId) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationId) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::checkApplicationOfAppProcessConfig(Message& request, uint8_t applicationID,
                                                                          uint8_t numOfServices) {
	if (not checkAppControlled(request, applicationID) or allServiceTypesAllowed(request, applicationID)) {
		for (uint8_t i = 0; i < numOfServices; i++) {
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
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::checkService(Message& request, uint8_t applicationID, uint8_t numOfMessages) {
	if (maxServiceTypesReached(request, applicationID)) {
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::maxReportTypesReached(Message& request, uint8_t applicationID,
                                                             uint8_t serviceType) {
	if (countReportsOfService(applicationID, serviceType) >= AllMessageTypes::MessagesOfService.at(serviceType).size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool RealTimeForwardingControlService::checkMessage(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                    uint8_t messageType) {
	return !maxReportTypesReached(request, applicationID, serviceType) and
	       !reportExistsInAppProcessConfiguration(applicationID, serviceType, messageType);
}

bool RealTimeForwardingControlService::reportExistsInAppProcessConfiguration(uint8_t applicationID, uint8_t serviceType,
                                                                             uint8_t messageType) {
	auto key = std::make_pair(applicationID, serviceType);
	auto& messages = applicationProcessConfiguration.definitions[key];
	return std::find(messages.begin(), messages.end(), messageType) != messages.end();
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
			addAllReportsOfApplication(applicationID);
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not checkService(request, applicationID, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				addAllReportsOfService(applicationID, serviceType);
				continue;
			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();

				if (not checkMessage(request, applicationID, serviceType, messageType)) {
					continue;
				}
				auto key = std::make_pair(applicationID, serviceType);
				applicationProcessConfiguration.definitions[key].push_back(
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
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
