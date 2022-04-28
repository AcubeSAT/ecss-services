#include "Services/RealTimeForwardingControlService.hpp"
#include <iostream>

void RealTimeForwardingControlService::reportAppProcessConfigurationContent(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportAppProcessConfigurationContent);
	appProcessConfigurationContentReport();
}

void RealTimeForwardingControlService::appProcessConfigurationContentReport() {
	Message report(ServiceType, MessageType::AppProcessConfigurationContentReport, Message::TM, 1);

	auto& definitions = applicationProcessConfiguration.definitions;
	uint8_t numOfApplications = 0;
	uint8_t previousAppID = std::numeric_limits<uint8_t>::max();

	etl::vector<uint8_t, ECSSMaxControlledApplicationProcesses> numOfServicesPerApp(ECSSMaxControlledApplicationProcesses, 1);

	for (auto& definition: applicationProcessConfiguration.definitions) {
		auto& pair = definition.first;
		auto applicationID = pair.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
			numOfApplications++;
		} else {
			numOfServicesPerApp[numOfApplications - 1]++;
		}
	}

	report.appendUint8(numOfApplications);
	previousAppID = std::numeric_limits<uint8_t>::max();
	uint8_t index = 0;

	for (auto& definition: applicationProcessConfiguration.definitions) { // C++ sorts the maps by default, based on key. So keys with the same appID are accessed alltogether.
		auto& pair = definition.first;
		auto applicationID = pair.first;
		if (applicationID != previousAppID) {
			previousAppID = applicationID;
//			std::cout<<"app="<<static_cast<int>(applicationID)<<"\n";
			report.appendUint8(applicationID);
			report.appendUint8(numOfServicesPerApp[index]);
//			std::cout<<"size="<<static_cast<int>(numOfServicesPerApp[index])<<"\n";
			index++;
		}
		auto serviceType = pair.second;
		auto numOfMessages = definition.second.size();
		report.appendUint8(serviceType);
		report.appendUint8(numOfMessages);
		for (auto messageType : definition.second) {
			report.appendUint8(messageType);
		}
	}
	storeMessage(report);
}

void RealTimeForwardingControlService::execute(Message& message) {
	switch (message.messageType) {
		case ReportAppProcessConfigurationContent:
			reportAppProcessConfigurationContent(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
