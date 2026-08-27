#include "Helpers/AppProcessConfiguration.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Message.hpp"

void ApplicationProcessConfiguration::addAllReportsOfApplication(const Message& message, ApplicationProcessId applicationID) {
	for (const auto& [serviceType, reportTypes]: AllReportTypes::MessagesOfService) {
		addAllReportsOfService(message, applicationID, serviceType);
	}
}

void ApplicationProcessConfiguration::addAllReportsOfService(const Message& message, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType) {
	auto serviceReportTypes = AllReportTypes::MessagesOfService.find(serviceType);
	if (serviceReportTypes == AllReportTypes::MessagesOfService.end()) {
		return;
	}
	for (const auto& messageType: serviceReportTypes->second) {
		if (checkMessageCanBeAdded(message, applicationID, serviceType, messageType)) {
			addReport(applicationID, serviceType, messageType);
		}
	}
}

void ApplicationProcessConfiguration::addReport(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	definitions[std::make_pair(applicationID, serviceType)].push_back(messageType);
}

uint8_t ApplicationProcessConfiguration::countServicesOfApplication(ApplicationProcessId applicationID) const {
	return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](const auto& definition) {
		return applicationID == definition.first.first;
	});
}

uint8_t ApplicationProcessConfiguration::countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const {
	const auto definition = definitions.find(std::make_pair(applicationID, serviceType));
	if (definition == definitions.end()) {
		return 0;
	}
	return definition->second.size();
}

bool ApplicationProcessConfiguration::checkAlreadyExistingReport(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	if (not isReportTypeAdded(applicationID, serviceType, messageType)) {
		return false;
	}
	ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingReportType);
	return true;
}

bool ApplicationProcessConfiguration::checkApplicationOfAppProcessConfigValid(Message& request, ApplicationProcessId applicationID,
    uint8_t numOfServices,
    const etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications) {
	if (not checkAppControlled(controlledApplications, request, applicationID) or
	    checkAllServiceTypesAllowed(request, applicationID)) {
		for (uint8_t i = 0; i < numOfServices; i++) {
			request.skipBytes(sizeof(ServiceTypeNum));
			uint8_t const numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::checkAllServiceTypesAllowed(const Message& request, const ApplicationProcessId applicationID) {
	if (isMaxServiceTypesReached(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::checkAppControlled(
    const etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications,
    const Message& request,
    ApplicationProcessId applicationID) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationID) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::isMaxServiceTypesReached(const ApplicationProcessId applicationID) const {
	return countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions;
}

bool ApplicationProcessConfiguration::checkMaxServiceTypesReached(const Message& request, ApplicationProcessId applicationID) {
	if (isMaxServiceTypesReached(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::checkServiceCanBeAdded(Message& request, ApplicationProcessId applicationID,
    uint8_t numOfMessages, ServiceTypeNum serviceType) {
	if (AllReportTypes::MessagesOfService.find(serviceType) == AllReportTypes::MessagesOfService.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition);
		request.skipBytes(numOfMessages);
		return false;
	}
	if (checkMaxServiceTypesReached(request, applicationID) and !isServiceAdded(applicationID, serviceType)) {
		request.skipBytes(numOfMessages);
		return false;
	}

	return true;
}

bool ApplicationProcessConfiguration::checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType) {
	auto serviceReportTypes = AllReportTypes::MessagesOfService.find(serviceType);
	if (serviceReportTypes == AllReportTypes::MessagesOfService.end()) {
		return true;
	}
	if (countReportsOfService(applicationID, serviceType) >= serviceReportTypes->second.size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::checkMessageCanBeAdded(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	return !checkMaxReportTypesReached(request, applicationID, serviceType) and
	       !checkAlreadyExistingReport(request, applicationID, serviceType, messageType);
}

bool ApplicationProcessConfiguration::checkApplicationInConfiguration(Message& request, ApplicationProcessId applicationID,
    uint8_t numOfServices) {
	if (not isApplicationEnabled(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess);
		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			request.skipBytes(sizeof(ServiceTypeNum));
			uint8_t const numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::checkServiceTypeInConfiguration(Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType, uint8_t numOfMessages) {
	if (not isServiceAdded(applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition);
		request.skipBytes(numOfMessages);
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::checkReportTypeInConfiguration(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType, MessageTypeNum messageType) const {
	if (not isReportTypeAdded(applicationID, serviceType, messageType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentReportTypeDefinition);
		return false;
	}
	return true;
}

void ApplicationProcessConfiguration::deleteApplicationProcess(ApplicationProcessId applicationID) {
	auto iter = definitions.begin();
	while (iter != definitions.end()) {
		if (iter->first.first == applicationID) {
			iter = definitions.erase(iter);
		} else {
			++iter;
		}
	}
}

void ApplicationProcessConfiguration::deleteServiceType(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	definitions.erase(std::make_pair(applicationID, serviceType));
}

void ApplicationProcessConfiguration::deleteReportType(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	const auto definition = definitions.find(std::make_pair(applicationID, serviceType));
	if (definition == definitions.end()) {
		return;
	}
	auto& reportTypes = definition->second;
	auto* const reportTypeIndex = etl::find(reportTypes.begin(), reportTypes.end(), messageType);
	if (reportTypeIndex != reportTypes.end()) {
		reportTypes.erase(reportTypeIndex);
	}
	if (reportTypes.empty()) {
		deleteServiceType(applicationID, serviceType);
	}
}

bool ApplicationProcessConfiguration::isApplicationEnabled(ApplicationProcessId targetAppID) const {
	return std::any_of(std::begin(definitions), std::end(definitions), [targetAppID](const auto& definition) {
		return targetAppID == definition.first.first;
	});
}

bool ApplicationProcessConfiguration::isServiceAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const {
	return definitions.find(std::make_pair(applicationID, serviceType)) != definitions.end();
}

bool ApplicationProcessConfiguration::isReportTypeAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) const {
	const auto definition = definitions.find(std::make_pair(applicationID, serviceType));
	if (definition == definitions.end()) {
		return false;
	}
	return etl::find(definition->second.begin(), definition->second.end(), messageType) != definition->second.end();
}
