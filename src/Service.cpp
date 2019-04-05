#include "Service.hpp"
#include "ServicePool.hpp"

void Service::execute(Message& message) {
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
		case 9:
			Services.timeManagement.execute(message); // ST[09]
			break;
		case 11:
			Services.timeBasedScheduling.execute(message); // ST[11]
			break;
		case 17:
			Services.testService.execute(message); // ST[17]
			break;
		case 19:
			Services.eventAction.execute(message); // ST[19]
			break;
		case 20:
			Services.parameterManagement.execute(message); // ST[20]
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
