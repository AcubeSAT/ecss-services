#include "Helpers/AppProcessConfiguration.hpp"
#include "Message.hpp"
#include "Helpers/AllReportTypes.hpp"

void ApplicationProcessConfiguration::addAllReportsOfApplication(const Message& message, ApplicationProcessId
applicationID) {
	for (const auto& service: AllReportTypes::getMessagesOfService()) {
		uint8_t const serviceType = service.first;
		addAllReportsOfService(message, applicationID, serviceType);
	}
}

void ApplicationProcessConfiguration::addAllReportsOfService(const Message& message, ApplicationProcessId applicationID,
ServiceTypeNum
serviceType) {
	for (const auto& messageType: AllReportTypes::getMessagesOfService().find(serviceType)->second) {
		auto appServicePair = std::make_pair(applicationID, serviceType);
		if (canMessageBeAdded(message, applicationID, serviceType, messageType)) {
			definitions[appServicePair].push_back(messageType);
		}
	}
}

uint8_t ApplicationProcessConfiguration::countServicesOfApplication(ApplicationProcessId applicationID) {
	return std::count_if(std::begin(definitions), std::end(definitions), [applicationID](const auto& definition) {
		return applicationID == definition.first.first;
	});
}

uint8_t ApplicationProcessConfiguration::countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	auto appServicePair = std::make_pair(applicationID, serviceType);
	return definitions[appServicePair].size();
}

bool ApplicationProcessConfiguration::reportExistsInAppProcessConfiguration(ApplicationProcessId
	applicationID,
	ServiceTypeNum serviceType,
	MessageTypeNum messageType) {
	auto key = std::make_pair(applicationID, serviceType);
	if (definitions.find(key) != definitions.end()) {
		if (etl::any_of(definitions[key].begin(), definitions[key].end(), [messageType](const auto& message) {
			return message == messageType;
		})) {
			return true;
		}
	}
	return false;
}

bool ApplicationProcessConfiguration::isApplicationOfAppProcessConfigValid(Message& request, ApplicationProcessId applicationID,
	uint8_t numOfServices,
	const etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses>& controlledApplications) {
	if (not checkAppControlled(controlledApplications, request, applicationID) or areAllServiceTypesAllowed(request,
		    applicationID)) {
		for (uint8_t i = 0; i < numOfServices; i++) {
			request.skipBytes(sizeof(ServiceTypeNum));
			uint8_t const numOfMessages = request.readUint8();
			request.skipBytes(numOfMessages);
		}
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::areAllServiceTypesAllowed(const Message& request, const ApplicationProcessId applicationID) {
	if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AllServiceTypesAlreadyAllowed);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::checkAppControlled (
	etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications, const
	Message& request,
	ApplicationProcessId applicationID) {
	if (std::find(controlledApplications.begin(), controlledApplications.end(), applicationID) ==
	    controlledApplications.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NotControlledApplication);
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::isMaxServiceTypesReached(const Message& request, ApplicationProcessId applicationID) {
	if (countServicesOfApplication(applicationID) >= ECSSMaxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypesReached);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::canServiceBeAdded(Message& request, ApplicationProcessId applicationID,
	uint8_t numOfMessages, ServiceTypeNum serviceType) {
	if (isMaxServiceTypesReached(request, applicationID) and !isServiceExisting(applicationID, serviceType)) {
		request.skipBytes(numOfMessages);
		return false;
	}

	return true;
}

bool ApplicationProcessConfiguration::checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum
	serviceType) {
	if (countReportsOfService(applicationID, serviceType) >= AllReportTypes::getMessagesOfService().at(serviceType).size
	    ()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::canMessageBeAdded(const Message& request, ApplicationProcessId applicationID, ServiceTypeNum serviceType,
	MessageTypeNum messageType) {
	if (checkMaxReportTypesReached(request, applicationID, serviceType)) {
		return false;
	}

	if (reportExistsInAppProcessConfiguration(applicationID, serviceType, messageType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingReportType);
		return false;
	}
	return true;
}
