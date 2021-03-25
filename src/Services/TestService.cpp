#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TEST

#include "Services/TestService.hpp"


void TestService::areYouAlive(Message& request) {
	request.assertTC(TEST, ARE_YOU_ALIVE_TEST);
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(ARE_YOU_ALIVE_TEST_REPORT);

	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	request.assertTC(TEST, ON_BOARD_CONNECTION_TEST);
	// TM[17,4] on-board connection test report
	Message report = createTM(ON_BOARD_CONNECTION_TEST_REPORT);

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

#endif
