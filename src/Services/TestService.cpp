#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TEST

#include "Services/TestService.hpp"
#include "ECSS_ST_Definitions.hpp"

void TestService::areYouAlive(Message& request) {
	request.assertTC(Test, AreYouAliveTest);
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(AreYouALiveTestReport);

	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	request.assertTC(Test, OnBoardConnectionTest);
	// TM[17,4] on-board connection test report
	Message report = createTM(OnBoardConnectionTestReport);

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
