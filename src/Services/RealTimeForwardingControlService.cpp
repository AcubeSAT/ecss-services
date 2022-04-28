#include "Services/RealTimeForwardingControlService.hpp"
#include <iostream>

bool RealTimeForwardingControlService::findApplication(uint8_t targetAppID) {
	auto& definitions = applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [targetAppID](auto& definition) { return targetAppID == definition.first.first; });
}

bool RealTimeForwardingControlService::findServiceType(uint8_t applicationID, uint8_t targetService) {
	auto& definitions = applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [applicationID, targetService](auto& definition) { return applicationID == definition.first.first and targetService == definition.first.second; });
}

bool RealTimeForwardingControlService::findReportType(uint8_t target, uint8_t applicationID,
                                                      uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	auto& serviceTypes = applicationProcessConfiguration.definitions[appServicePair];
	return std::find(serviceTypes.begin(), serviceTypes.end(), target) != serviceTypes.end();
}

void RealTimeForwardingControlService::deleteApplicationProcess(uint8_t applicationID) {
	auto& definitions = applicationProcessConfiguration.definitions;
	auto iter = std::find_if(
	    std::begin(definitions), std::end(definitions), [applicationID](auto& definition) { return applicationID == definition.first.first; });
	while (iter != definitions.end()) {
		definitions.erase(iter);
		iter = std::find_if(
		    std::begin(definitions), std::end(definitions), [applicationID](auto& definition) { return applicationID == definition.first.first; });
	}
}

bool RealTimeForwardingControlService::applicationExists(Message& request, uint8_t applicationID,
                                                         uint8_t numOfServices) {
	if (not findApplication(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingApplication);
		for (uint8_t z = 0; z < numOfServices; z++) {
			request.skipBytes(1);
			uint8_t numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::serviceTypeExists(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                         uint8_t numOfMessages) {
	if (not findServiceType(applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition);
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::reportTypeExists(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                        uint8_t messageType) {
	if (not findReportType(messageType, applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition);
		return false;
	}
	return true;
}

void RealTimeForwardingControlService::deleteServiceRecursive(uint8_t applicationID, uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	applicationProcessConfiguration.definitions.erase(appServicePair);
}

void RealTimeForwardingControlService::deleteReportRecursive(uint8_t applicationID, uint8_t serviceType,
                                                             uint8_t messageType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	auto& reportTypes = applicationProcessConfiguration.definitions[appServicePair];
	reportTypes.erase(std::remove(reportTypes.begin(), reportTypes.end(), messageType));

	if (applicationProcessConfiguration.definitions[appServicePair].empty()) {
		deleteServiceRecursive(applicationID, serviceType);
	}
}

void RealTimeForwardingControlService::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteReportTypesFromAppProcessConfiguration);

	uint8_t numOfApplications = request.readUint8();
	if (numOfApplications == 0) {
		applicationProcessConfiguration.definitions.clear();
		return;
	}

	for (uint8_t i = 0; i < numOfApplications; i++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not applicationExists(request, applicationID, numOfServices)) {
			continue;
		}
		if (numOfServices == 0) {
			deleteApplicationProcess(applicationID);
			continue;
		}

		for (uint8_t j = 0; j < numOfServices; j++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not serviceTypeExists(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}
			if (numOfMessages == 0) {
				deleteServiceRecursive(applicationID, serviceType);
				continue;
			}

			for (uint8_t k = 0; k < numOfMessages; k++) {
				uint8_t messageType = request.readUint8();

				if (not reportTypeExists(request, applicationID, serviceType, messageType)) {
					continue;
				}
				deleteReportRecursive(applicationID, serviceType, messageType);
			}
		}
	}
}

void RealTimeForwardingControlService::execute(Message& message) {
	switch (message.messageType) {
		case DeleteReportTypesFromAppProcessConfiguration:
			deleteReportTypesFromAppProcessConfiguration(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}
