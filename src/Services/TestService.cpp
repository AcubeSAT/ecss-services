#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TEST

#include "Services/TestService.hpp"

void TestService::areYouAlive(Message& request) {
	request.assertTC(TestService::ServiceType, TestService::MessageType::AreYouAliveTest);
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(TestService::MessageType::AreYouAliveTestReport);

	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	request.assertTC(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest);
	// TM[17,4] on-board connection test report
	Message report = createTM(TestService::MessageType::OnBoardConnectionTestReport);

	report.appendUint16(request.readUint16());
	// just print it on the screen
	storeMessage(report);
}

void TestService::execute(Message& message) {
	switch (message.messageType) {
		case AreYouAliveTest:
			areYouAlive(message);
			break;
		case OnBoardConnectionTest:
			onBoardConnection(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
