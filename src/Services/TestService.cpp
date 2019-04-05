#include "Services/TestService.hpp"

void TestService::areYouAlive(Message &request) {
	request.assertTC(17, 1);
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(2);

	storeMessage(report);
}

void TestService::onBoardConnection(Message &request) {
	request.assertTC(17, 3);
	// TM[17,4] on-board connection test report
	Message report = createTM(4);

	report.appendUint16(request.readUint16());
	// just print it on the screen
	storeMessage(report);
}

void TestService::execute(Message& message) {
	switch (message.messageType) {
		case 1:
			areYouAlive(message);
			break;
		case 3:
			onBoardConnection(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
