#include "Services/TestService.hpp"

void TestService::areYouAlive(Message &request) {
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(2);

	storeMessage(report);
}

void TestService::onBoardConnection(Message &request) {
	// TM[17,4] on-board connection test report
	Message report = createTM(4);

	report.appendUint16(request.readUint16());
	//just print it on the screen
	storeMessage(report);
}
