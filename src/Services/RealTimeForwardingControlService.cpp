#include "Services/RealTimeForwardingControlService.hpp"
#include <iostream>

void RealTimeForwardingControlService::reportAppProcessConfigurationContent(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportAppProcessConfigurationContent);
	appProcessConfigurationContentReport();
}

void RealTimeForwardingControlService::appProcessConfigurationContentReport() {
	Message report(ServiceType, MessageType::AppProcessConfigurationContentReport, Message::TM, 1);

	uint8_t numOfApplications = applicationProcessConfiguration.definitions.size();
	report.appendUint8(numOfApplications);
	for (auto& definition : applicationProcessConfiguration.definitions) {
		auto& pair = definition.first;
		auto applicationID = pair.first;
		auto serviceType = pair.second;
		std::cout<<static_cast<int>(applicationID)<<"\n";
	}
//	storeMessage(report);
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
