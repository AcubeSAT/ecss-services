#include "ForwardingAndPacketHelper.hpp"
namespace ForwardingAndPacketHelper {
	void validReportTypes(Message& request) {
		uint8_t numOfApplications = 1;
		uint8_t numOfServicesPerApp = 2;
		uint8_t numOfMessagesPerService = 2;

		request.appendUint8(numOfApplications);

		for (auto appID: applications) {
			request.append <ApplicationProcessId>(appID);
			request.appendUint8(numOfServicesPerApp);

			for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
				ServiceTypeNum serviceType = services[serviceIndex];
				request.append <ServiceTypeNum>(serviceType);
				request.appendUint8(numOfMessagesPerService);
				auto& messages = (serviceIndex == 0) ? messages1 : messages2;

				for (uint8_t messageIndex = 0; messageIndex < numOfMessagesPerService; messageIndex++) {
					request.append <MessageTypeNum>(messages[messageIndex]);
				}
			}
		}
	}

	void duplicateReportTypes(Message& request) {
		uint8_t numOfApplications = 1;
		uint8_t numOfServicesPerApp = 2;
		uint8_t numOfMessagesPerService = 2;

		request.appendUint8(numOfApplications);

		for (auto appID: applications) {
			request.append <ApplicationProcessId>(appID);
			request.appendUint8(numOfServicesPerApp);

			for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
				ServiceTypeNum serviceType = services[serviceIndex];
				request.append <ServiceTypeNum>(serviceType);
				request.appendUint8(numOfMessagesPerService);

				for (uint8_t messageIndex = 0; messageIndex < numOfMessagesPerService; messageIndex++) {
					request.append <MessageTypeNum>(messages1[0]);
				}
			}
		}
	}

	void validInvalidReportTypes(Message& request) {
		uint8_t numOfApplications = 3;
		uint8_t numOfMessagesPerService = 2;

		ApplicationProcessId applications2[] = {1, 2, 3};
		request.appendUint8(numOfApplications);

		for (uint8_t appIndex = 0; appIndex < numOfApplications; appIndex++) {
			request.append <ApplicationProcessId>(applications2[appIndex]);
			uint8_t numOfServicesPerApp = (appIndex == 0) ? 12 : 2;
			uint8_t* servicesToPick = (appIndex == 0) ? redundantServices : services;
			request.appendUint8(numOfServicesPerApp);

			for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
				ServiceTypeNum serviceType = servicesToPick[serviceIndex];
				request.append <ServiceTypeNum>(serviceType);
				request.appendUint8(numOfMessagesPerService);
				auto& messages = (serviceIndex == 0) ? messages1 : messages2;

				for (uint8_t messageIndex = 0; messageIndex < numOfMessagesPerService; messageIndex++) {
					request.append <MessageTypeNum>(messages[messageIndex]);
				}
			}
		}
	}

	void validAllReportsOfService(Message& request) {
		uint8_t numOfApplications = 1;
		uint8_t numOfServicesPerApp = 2;
		uint8_t numOfMessagesPerService = 0;

		request.appendUint8(numOfApplications);

		for (auto appID: applications) {
			request.append <ApplicationProcessId>(appID);
			request.appendUint8(numOfServicesPerApp);

			for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
				ServiceTypeNum serviceType = services[serviceIndex];
				request.append <ServiceTypeNum>(serviceType);
				request.appendUint8(numOfMessagesPerService);
			}
		}
	}

	void validInvalidAllReportsOfService(Message& request) {
		uint8_t numOfApplications = 3;
		uint8_t numOfMessagesPerService = 2;

		ApplicationProcessId applications2[] = {1, 2, 3};
		request.appendUint8(numOfApplications);

		for (uint8_t appIndex = 0; appIndex < numOfApplications; appIndex++) {
			request.append <ApplicationProcessId>(applications2[appIndex]);
			uint8_t numOfServicesPerApp = (appIndex == 0) ? 12 : 2;
			uint8_t* servicesToPick = (appIndex == 0) ? redundantServices : services;
			request.appendUint8(numOfServicesPerApp);

			for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
				ServiceTypeNum serviceType = servicesToPick[serviceIndex];
				request.append <ServiceTypeNum>(serviceType);
				uint8_t numOfMessages = (appIndex < 2) ? 0 : numOfMessagesPerService;
				request.appendUint8(numOfMessages);
				if (appIndex >= 2) {
					uint8_t* messages = (serviceIndex == 0) ? messages1 : messages2;

					for (uint8_t messageIndex = 0; messageIndex < numOfMessagesPerService; messageIndex++) {
						request.append <MessageTypeNum>(messages[messageIndex]);
					}
				}
			}
		}
	}

	void validAllReportsOfApp(Message& request) {
		uint8_t numOfApplications = 1;
		uint8_t numOfServicesPerApp = 0;

		request.appendUint8(numOfApplications);

		for (auto appID: applications) {
			request.append <ApplicationProcessId>(appID);
			request.appendUint8(numOfServicesPerApp);
		}
	}

	void validInvalidAllReportsOfApp(Message& request) {
		uint8_t numOfApplications = 3;
		uint8_t numOfMessagesPerService = 2;

		ApplicationProcessId applications2[] = {1, 2, 3};
		request.appendUint8(numOfApplications);

		for (uint8_t appIndex = 0; appIndex < numOfApplications; appIndex++) {
			request.append <ApplicationProcessId>(applications2[appIndex]);
			uint8_t numOfServicesPerApp = (appIndex == 0 or appIndex == 1) ? 0 : 2;
			uint8_t* servicesToPick = (appIndex == 0) ? redundantServices : services;
			request.appendUint8(numOfServicesPerApp);

			if (appIndex >= 2) {
				for (uint8_t serviceIndex = 0; serviceIndex < numOfServicesPerApp; serviceIndex++) {
					ServiceTypeNum serviceType = servicesToPick[serviceIndex];
					request.append <ServiceTypeNum>(serviceType);
					uint8_t numOfMessages = (appIndex == 0 or appIndex == 1) ? 0 : numOfMessagesPerService;
					request.appendUint8(numOfMessages);

					uint8_t* messages = (serviceIndex == 0) ? messages1 : messages2;

					for (uint8_t messageIndex = 0; messageIndex < numOfMessagesPerService; messageIndex++) {
						request.append <MessageTypeNum>(messages[messageIndex]);
					}
				}
			}
		}
	}
}
