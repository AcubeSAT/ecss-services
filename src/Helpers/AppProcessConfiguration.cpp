#include <limits>
#include "Helpers/AppProcessConfiguration.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Message.hpp"
#include "etl/algorithm.h"

void ApplicationProcessConfiguration::addAllReportsOfApplication(ApplicationProcessId applicationID) {
	for (const auto& [serviceType, reportTypes]: AllReportTypes::MessagesOfService) {
		addAllReportsOfService(applicationID, serviceType);
	}
}

void ApplicationProcessConfiguration::addAllReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	auto serviceReportTypes = AllReportTypes::MessagesOfService.find(serviceType);
	if (serviceReportTypes == AllReportTypes::MessagesOfService.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
		return;
	}
	for (const auto& messageType: serviceReportTypes->second) {
		addReport(applicationID, serviceType, messageType);
	}
}

void ApplicationProcessConfiguration::checkAndAddReport(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType, MessageTypeNum messageType) {
	if (not checkMaxReportTypesReached(request, applicationID, serviceType)) {
		addReport(applicationID, serviceType, messageType);
	}
}

void ApplicationProcessConfiguration::addReport(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	if (not isReportTypeAdded(applicationID, serviceType, messageType)) {
		definitions[AppServiceKey{applicationID, serviceType}].push_back(messageType);
	}
}

uint8_t ApplicationProcessConfiguration::countServicesOfApplication(ApplicationProcessId applicationID) const {
	return etl::count_if(definitions.begin(), definitions.end(), [applicationID](const auto& definition) {
		return applicationID == definition.first.first;
	});
}

uint8_t ApplicationProcessConfiguration::countReportsOfService(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const {
	const auto definition = definitions.find(AppServiceKey{applicationID, serviceType});
	if (definition == definitions.end()) {
		return 0;
	}
	return definition->second.size();
}

bool ApplicationProcessConfiguration::checkApplicationOfAppProcessConfigValid(Message& request, ApplicationProcessId applicationID,
    uint8_t numOfServices, const etl::span<const ApplicationProcessId>& controlledApplications) {
	if ((not checkAppControlled(controlledApplications, request, applicationID)) or
	    ((numOfServices != 0U) and checkAllServiceTypesAllowed(request, applicationID))) {
		skipServiceBlocks(request, numOfServices);
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

bool ApplicationProcessConfiguration::checkAppControlled(const etl::span<const ApplicationProcessId>& controlledApplications,
    const Message& request, ApplicationProcessId applicationID) {
	if (etl::find(controlledApplications.begin(), controlledApplications.end(), applicationID) ==
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
	if (not AllReportTypes::MessagesOfService.contains(serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition);
		skipReportTypes(request, numOfMessages);
		return false;
	}
	if ((not isServiceAdded(applicationID, serviceType)) and checkMaxServiceTypesReached(request, applicationID)) {
		skipReportTypes(request, numOfMessages);
		return false;
	}

	return true;
}

bool ApplicationProcessConfiguration::checkMaxReportTypesReached(const Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType) const {
	auto serviceReportTypes = AllReportTypes::MessagesOfService.find(serviceType);
	if (serviceReportTypes == AllReportTypes::MessagesOfService.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
		return true;
	}
	if (countReportsOfService(applicationID, serviceType) >= serviceReportTypes->second.size()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypesReached);
		return true;
	}
	return false;
}

bool ApplicationProcessConfiguration::checkApplicationInConfiguration(Message& request, ApplicationProcessId applicationID,
    uint8_t numOfServices) const {
	if (not isApplicationEnabled(applicationID)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentApplicationProcess);
		skipServiceBlocks(request, numOfServices);
		return false;
	}
	return true;
}

bool ApplicationProcessConfiguration::checkServiceTypeInConfiguration(Message& request, ApplicationProcessId applicationID,
    ServiceTypeNum serviceType, uint8_t numOfMessages) const {
	if (not isServiceAdded(applicationID, serviceType)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistentServiceTypeDefinition);
		skipReportTypes(request, numOfMessages);
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
	bool deletedAnyDefinition = false;
	auto iter = definitions.begin();
	while (iter != definitions.end()) {
		if (iter->first.first == applicationID) {
			iter = definitions.erase(iter);
			deletedAnyDefinition = true;
		} else {
			++iter;
		}
	}
	if (not deletedAnyDefinition) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
	}
}

void ApplicationProcessConfiguration::deleteServiceType(ApplicationProcessId applicationID, ServiceTypeNum serviceType) {
	if (definitions.erase(AppServiceKey{applicationID, serviceType}) == 0U) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
	}
}

void ApplicationProcessConfiguration::deleteReportType(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) {
	const auto definition = definitions.find(AppServiceKey{applicationID, serviceType});
	if (definition == definitions.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
		return;
	}
	auto& reportTypes = definition->second;
	auto* const reportTypeIndex = etl::find(reportTypes.begin(), reportTypes.end(), messageType);
	if (reportTypeIndex == reportTypes.end()) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::ElementNotInArray);
		return;
	}
	reportTypes.erase(reportTypeIndex);
	if (reportTypes.empty()) {
		deleteServiceType(applicationID, serviceType);
	}
}

bool ApplicationProcessConfiguration::isApplicationEnabled(ApplicationProcessId targetAppID) const {
	return etl::any_of(definitions.begin(), definitions.end(), [targetAppID](const auto& definition) {
		return targetAppID == definition.first.first;
	});
}

bool ApplicationProcessConfiguration::isServiceAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType) const {
	return definitions.contains(AppServiceKey{applicationID, serviceType});
}

bool ApplicationProcessConfiguration::isReportTypeAdded(ApplicationProcessId applicationID, ServiceTypeNum serviceType,
    MessageTypeNum messageType) const {
	const auto definition = definitions.find(AppServiceKey{applicationID, serviceType});
	if (definition == definitions.end()) {
		return false;
	}
	return etl::find(definition->second.begin(), definition->second.end(), messageType) != definition->second.end();
}

void ApplicationProcessConfiguration::appendContentToReport(Message& report) const {
	uint8_t numberOfApplications = 0;
	ApplicationProcessId previousAppID = std::numeric_limits<ApplicationProcessId>::max();
	for (const auto& [appServiceKey, reportTypes]: definitions) {
		if (appServiceKey.first != previousAppID) {
			previousAppID = appServiceKey.first;
			numberOfApplications++;
		}
	}
	report.appendUint8(numberOfApplications);

	// The map is sorted by key, so all entries of the same application process are accessed consecutively.
	previousAppID = std::numeric_limits<ApplicationProcessId>::max();
	for (const auto& [appServiceKey, reportTypes]: definitions) {
		const ApplicationProcessId applicationID = appServiceKey.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
			report.append<ApplicationProcessId>(applicationID);
			report.appendUint8(countServicesOfApplication(applicationID));
		}

		report.append<ServiceTypeNum>(appServiceKey.second);
		report.appendUint8(reportTypes.size());
		for (const auto& messageType: reportTypes) {
			report.append<MessageTypeNum>(messageType);
		}
	}
}

void ApplicationProcessConfiguration::skipReportTypes(Message& request, uint8_t numOfMessages) {
	request.skipBytes(static_cast<uint16_t>(numOfMessages * sizeof(MessageTypeNum)));
}

void ApplicationProcessConfiguration::skipServiceBlocks(Message& request, uint8_t numOfServices) {
	for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
		request.skipBytes(sizeof(ServiceTypeNum));
		const uint8_t numOfMessages = request.readUint8();
		skipReportTypes(request, numOfMessages);
	}
}
