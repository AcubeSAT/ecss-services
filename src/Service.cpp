#include "Service.hpp"
#include "ServicePool.hpp"
#include "Helpers/TimeGetter.hpp"

void Service::handleMessage(Message& message) {
	message.finalize();
	#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	if (Services.realTimeForwarding.isReportTypeEnabled(message.messageType, message.applicationId, message.serviceType)) {
		releaseMessage(message);
	}

	#endif

	#ifdef SERVICE_STORAGEANDRETRIEVAL
	auto packetStoreId = Services.storageAndRetrieval.getPacketStoreFromServiceType(message.serviceType);
	Services.storageAndRetrieval.addTelemetryToPacketStore(packetStoreId, message, TimeGetter::getCurrentTimeDefaultCUC());
	#endif

	platformSpecificHandleMessage(message);
}
