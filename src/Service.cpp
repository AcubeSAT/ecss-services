#include "Service.hpp"
#include "ServicePool.hpp"

void Service::execute(Message &message) {
	switch (message.serviceType) {
		case 5:
			Services.eventReport.execute(message); // ST[05]
			break;
		case 6:
			Services.memoryManagement.rawDataMemorySubservice.execute(message); // ST[06]
			break;
		case 8:
			Services.functionManagement.execute(message); // ST[08]
			break;
		case 17:
			Services.testService.execute(message); // ST[17]
			break;
		case 19:
			Services.eventAction.execute(message); // ST[19]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}