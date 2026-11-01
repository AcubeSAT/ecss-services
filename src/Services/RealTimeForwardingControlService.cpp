#include "ECSS_Configuration.hpp"
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL

#include "Services/RealTimeForwardingControlService.hpp"

void RealTimeForwardingControlService::addReportTypesToAppProcessConfiguration(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::AddReportTypesToAppProcessConfiguration)) {
		return;
	}
	uint8_t const numOfApplications = request.readUint8();

	for (uint8_t currentApplicationNumber = 0; currentApplicationNumber < numOfApplications; currentApplicationNumber++) {
		const ApplicationProcessId applicationID = request.read<ApplicationProcessId>();
		uint8_t const numOfServices = request.readUint8();

		if (not applicationProcessConfiguration.checkApplicationOfAppProcessConfigValid(request, applicationID,
		numOfServices, controlledApplications)) {
			continue;
		}

		if (numOfServices == 0) {
			applicationProcessConfiguration.addAllReportsOfApplication(applicationID);
			continue;
		}

		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			const ServiceTypeNum serviceType = request.read<ServiceTypeNum>();
			uint8_t const numOfMessages = request.readUint8();

			if (not applicationProcessConfiguration.checkServiceCanBeAdded(request, applicationID, numOfMessages, serviceType)) {
				continue;
			}

			if (numOfMessages == 0) {
				applicationProcessConfiguration.addAllReportsOfService(applicationID, serviceType);
				continue;
			}

			for (uint8_t currentMessageNumber = 0; currentMessageNumber < numOfMessages; currentMessageNumber++) {
				MessageTypeNum const messageType = request.read<MessageTypeNum>();

				applicationProcessConfiguration.checkAndAddReport(request, applicationID, serviceType, messageType);
			}
		}
	}
}

void RealTimeForwardingControlService::deleteReportTypesFromAppProcessConfiguration(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeleteReportTypesFromAppProcessConfiguration)) {
		return;
	}

	uint8_t const numOfApplications = request.readUint8();
	if (numOfApplications == 0) {
		applicationProcessConfiguration.definitions.clear();
		return;
	}

	for (uint8_t currentApplicationNumber = 0; currentApplicationNumber < numOfApplications; currentApplicationNumber++) {
		const ApplicationProcessId applicationID = request.read<ApplicationProcessId>();
		uint8_t const numOfServices = request.readUint8();

		if (not applicationProcessConfiguration.checkApplicationInConfiguration(request, applicationID, numOfServices)) {
			continue;
		}
		if (numOfServices == 0) {
			applicationProcessConfiguration.deleteApplicationProcess(applicationID);
			continue;
		}

		for (uint8_t currentServiceNumber = 0; currentServiceNumber < numOfServices; currentServiceNumber++) {
			const ServiceTypeNum serviceType = request.read<ServiceTypeNum>();
			uint8_t const numOfMessages = request.readUint8();

			if (not applicationProcessConfiguration.checkServiceTypeInConfiguration(request, applicationID, serviceType, numOfMessages)) {
				continue;
			}
			if (numOfMessages == 0) {
				applicationProcessConfiguration.deleteServiceType(applicationID, serviceType);
				continue;
			}

			for (uint8_t currentMessageNumber = 0; currentMessageNumber < numOfMessages; currentMessageNumber++) {
				const MessageTypeNum messageType = request.read<MessageTypeNum>();

				if (not applicationProcessConfiguration.checkReportTypeInConfiguration(request, applicationID, serviceType, messageType)) {
					continue;
				}
				applicationProcessConfiguration.deleteReportType(applicationID, serviceType, messageType);
			}
		}
	}
}

void RealTimeForwardingControlService::reportAppProcessConfigurationContent(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportAppProcessConfigurationContent)) {
		return;
	}
	appProcessConfigurationContentReport();
}

void RealTimeForwardingControlService::appProcessConfigurationContentReport() {
	Message report(ServiceType, MessageType::AppProcessConfigurationContentReport, Message::TM, ApplicationId);
	applicationProcessConfiguration.appendContentToReport(report);
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