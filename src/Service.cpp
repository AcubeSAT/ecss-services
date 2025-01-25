#include "Service.hpp"
#include "ServicePool.hpp"



void Service::handleMessage(Message& message) {
	message.finalize();
	#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	if (Services.realTimeForwarding.isReportTypeEnabled(message.messageType, message.applicationId, message.serviceType)) {
		releaseMessage(message);
	}

	#endif

	#ifdef SERVICE_STORAGEANDRETRIEVAL
	// if (Services.storageAndRetrieval.
	// Services.storageAndRetrieval.addTelemetryToPacketStore();
	#endif

	platformSpecificHandleMessage(message);
}
