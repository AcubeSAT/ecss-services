#include "Service.hpp"
#include "ServicePool.hpp"
#include "Helpers/TimeGetter.hpp"

void Service::handleMessage(Message& message) {
	message.finalize();

	platformSpecificHandleMessage(message);

#ifdef SERVICE_STORAGEANDRETRIEVAL
	// ST[15] decides on its own which packet stores (if any) shall store this message.
	Services.storageAndRetrieval.storeTelemetry(message, TimeGetter::getCurrentTimeDefaultCUC());
#endif

#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	// ST[14] is the gatekeeper of everything that leaves the subsystem: a TM is only released if the
	// real-time forwarding configuration contains its (application process, service type, message type).
	if (Services.realTimeForwarding.applicationProcessConfiguration.isReportTypeAdded(
	        message.applicationId, message.serviceType, message.messageType)) {
		releaseMessage(message);
	}
#else
	// Without ST[14], no forwarding control exists, so every message is released immediately.
	releaseMessage(message);
#endif
}
