#include "ECSS_Configuration.hpp"
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL

#include "Services/RealTimeForwardingControlService.hpp"

void RealTimeForwardingControlService::addAllReportsOfApplication(uint8_t applicationID) {
	for (const auto& service: AllReportTypes::MessagesOfService) {
		uint8_t serviceType = service.first;
		addAllReportsOfService(applicationID, serviceType);
	}
}

void RealTimeForwardingControlService::addAllReportsOfService(uint8_t applicationID, uint8_t serviceType) {
	for (const auto& messageType: AllReportTypes::MessagesOfService.at(serviceType)) {
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
	if (countReportsOfService(applicationID, serviceType) >= AllReportTypes::MessagesOfService.at(serviceType).size()) {
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
	if (!request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration)) {
		return;
	}
	uint8_t numOfApplications = request.readUint8();

	for (uint8_t currentApplicationNumber = 0; currentApplicationNumber < numOfApplications; currentApplicationNumber++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not checkApplicationOfAppProcessConfig(request, applicationID, numOfServices)) {
			continue;
		}

		if (numOfServices == 0) {
			addAllReportsOfApplication(applicationID);
			continue;
		}

		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not checkService(request, applicationID, numOfMessages)) {
				continue;
			}

			if (numOfMessages == 0) {
				addAllReportsOfService(applicationID, serviceType);
				continue;
			}

			for (uint8_t currentMessageNumber = 0; currentMessageNumber < numOfMessages; currentMessageNumber++) {
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

bool RealTimeForwardingControlService::isApplicationEnabled(uint8_t targetAppID) {
	auto& definitions = applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [targetAppID](auto& definition) { return targetAppID == definition.first.first; });
}

bool RealTimeForwardingControlService::isServiceTypeEnabled(uint8_t applicationID, uint8_t targetService) {
	auto& definitions = applicationProcessConfiguration.definitions;
	return std::any_of(std::begin(definitions), std::end(definitions), [applicationID, targetService](auto& definition) { return applicationID == definition.first.first and targetService == definition.first.second; });
}

bool RealTimeForwardingControlService::isReportTypeEnabled(uint8_t target, uint8_t applicationID,
                                                      uint8_t serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	auto serviceTypes = applicationProcessConfiguration.definitions.find(appServicePair);
	if (serviceTypes == applicationProcessConfiguration.definitions.end()) {
		return false;
	}
	return std::find(serviceTypes->second.begin(), serviceTypes->second.end(), target) != serviceTypes->second.end();
}

void RealTimeForwardingControlService::deleteApplicationProcess(uint8_t applicationID) {
	auto& definitions = applicationProcessConfiguration.definitions;
	auto iter = std::begin(definitions);
	while (iter != definitions.end()) {
		iter = std::find_if(
		    std::begin(definitions), std::end(definitions), [applicationID](auto& definition) { return applicationID == definition.first.first; });
		definitions.erase(iter);
	}
}

bool RealTimeForwardingControlService::isApplicationInConfiguration(Message& request, uint8_t applicationID,
                                                         uint8_t numOfServices) {
	if (not isApplicationEnabled(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess);
		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			request.skipBytes(1);
			uint8_t numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::isServiceTypeInConfiguration(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                         uint8_t numOfMessages) {
	if (not isServiceTypeEnabled(applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition);
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool RealTimeForwardingControlService::isReportTypeInConfiguration(Message& request, uint8_t applicationID, uint8_t serviceType,
                                                        uint8_t messageType) {
	if (not isReportTypeEnabled(messageType, applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentReportTypeDefinition);
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
	auto reportTypes = applicationProcessConfiguration.definitions.find(appServicePair);
	if (reportTypes == applicationProcessConfiguration.definitions.end()) {
		return;
	}
	reportTypes->second.erase(std::remove(reportTypes->second.begin(), reportTypes->second.end(), messageType));

	if (applicationProcessConfiguration.definitions[appServicePair].empty()) {
		deleteServiceRecursive(applicationID, serviceType);
	}
}

void RealTimeForwardingControlService::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeleteReportTypesFromAppProcessConfiguration)) {
		return;
	}

	uint8_t numOfApplications = request.readUint8();
	if (numOfApplications == 0) {
		applicationProcessConfiguration.definitions.clear();
		return;
	}

	for (uint8_t currentApplicationNumber = 0; currentApplicationNumber < numOfApplications; currentApplicationNumber++) {
		uint8_t applicationID = request.readUint8();
		uint8_t numOfServices = request.readUint8();

		if (not isApplicationInConfiguration(request, applicationID, numOfServices)) {
			continue;
		}
		if (numOfServices == 0) {
			deleteApplicationProcess(applicationID);
			continue;
		}

		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			uint8_t serviceType = request.readUint8();
			uint8_t numOfMessages = request.readUint8();

			if (not isServiceTypeInConfiguration(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}
			if (numOfMessages == 0) {
				deleteServiceRecursive(applicationID, serviceType);
				continue;
			}

			for (uint8_t currentMessageNumber = 0; currentMessageNumber < numOfMessages; currentMessageNumber++) {
				uint8_t messageType = request.readUint8();

				if (not isReportTypeInConfiguration(request, applicationID, serviceType, messageType)) {
					continue;
				}
				deleteReportRecursive(applicationID, serviceType, messageType);
			}
		}
	}
}

void RealTimeForwardingControlService::reportAppProcessConfigurationContent(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportAppProcessConfigurationContent)) {
		return;
	}
	appProcessConfigurationContentReport();
}

void RealTimeForwardingControlService::appProcessConfigurationContentReport() {
	Message report(ServiceType, MessageType::AppProcessConfigurationContentReport, Message::TM, ApplicationId);

	const auto& definitions = applicationProcessConfiguration.definitions;
	uint8_t numOfApplications = 0;
	uint8_t previousAppID = std::numeric_limits<uint8_t>::max();

	etl::vector<uint8_t, ECSSMaxControlledApplicationProcesses> numOfServicesPerApp(ECSSMaxControlledApplicationProcesses, 0);

	for (const auto& definition: definitions) {
		const auto& appAndServiceIdPair = definition.first;
		auto applicationID = appAndServiceIdPair.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
			numOfApplications++;
		}
		numOfServicesPerApp[numOfApplications - 1]++;
	}

	report.appendUint8(numOfApplications);
	previousAppID = std::numeric_limits<uint8_t>::max();
	uint8_t appIdIndex = 0;

	// C++ sorts the maps based on key by default. So keys with the same appID are accessed all-together.
	for (const auto& definition: definitions) {
		const auto& appAndServiceIdPair = definition.first;
		auto applicationID = appAndServiceIdPair.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
			report.appendUint8(applicationID);
			report.appendUint8(numOfServicesPerApp[appIdIndex]);
			appIdIndex++;
		}
		auto serviceType = appAndServiceIdPair.second;
		auto numOfMessages = definition.second.size();
		report.appendUint8(serviceType);
		report.appendUint8(numOfMessages);
		for (auto messageType: definition.second) {
			report.appendUint8(messageType);
		}
	}
	storeMessage(report);
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
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif