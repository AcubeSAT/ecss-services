#include "Services/RealTimeForwardingControlService.hpp"
#include <iostream>

bool RealTimeForwardingControlService::reportExistsInAppProcessConfiguration(uint8_t target, uint8_t applicationID,
                                                                             uint8_t serviceType) {
	auto& serviceTypes = applicationProcessConfiguration.definitions[applicationID][serviceType];
	return std::find(serviceTypes.begin(), serviceTypes.end(), target) != serviceTypes.end();
}

bool RealTimeForwardingControlService::applicationExists(Message& request, uint8_t applicationID,
                                                         uint8_t numOfServices) {
	if (applicationProcessConfiguration.definitions.find(applicationID) ==
	    applicationProcessConfiguration.definitions.end()) {
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
	if (applicationProcessConfiguration.definitions[applicationID].find(serviceType) ==
	    applicationProcessConfiguration.definitions[applicationID].end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinition);
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::reportTypeExists(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                        uint8_t messageType) {
	if (not reportExistsInAppProcessConfiguration(messageType, applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinition);
		return false;
	}
	return true;
}

void RealTimeForwardingControlService::deleteServiceRecursive(uint8_t applicationID, uint8_t serviceType) {
	applicationProcessConfiguration.definitions[applicationID].erase(serviceType);
	applicationProcessConfiguration.notEmpty[applicationID].erase(serviceType);

	if (applicationProcessConfiguration.definitions[applicationID].empty()) {
		applicationProcessConfiguration.definitions.erase(applicationID);
		applicationProcessConfiguration.notEmpty.erase(applicationID);
	}
}

void RealTimeForwardingControlService::deleteReportRecursive(uint8_t applicationID, uint8_t serviceType,
                                                             uint8_t messageType) {
	auto& reportTypes = applicationProcessConfiguration.definitions[applicationID][serviceType];
	reportTypes.erase(std::remove(reportTypes.begin(), reportTypes.end(), messageType));

	if (applicationProcessConfiguration.definitions[applicationID][serviceType].empty()) {
		deleteServiceRecursive(applicationID, serviceType);
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

		if (not applicationExists(request, applicationID, numOfServices)) {
			continue;
		}
		if (numOfServices == 0) {
			applicationProcessConfiguration.definitions.erase(applicationID);
			applicationProcessConfiguration.notEmpty.erase(applicationID);
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
