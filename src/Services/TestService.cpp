#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TEST

#include "ServicePool.hpp"
#include "Services/TestService.hpp"

void TestService::areYouAlive(Message& request) {
	request.assertTC(TestService::ServiceType, TestService::MessageType::AreYouAliveTest);
	areYouAliveReport();
}

void TestService::areYouAliveReport() {
	Message report = createTM(TestService::MessageType::AreYouAliveTestReport);
	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	request.assertTC(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest);
	uint16_t applicationProcessId = request.readUint16();
	if (applicationProcessId == ApplicationId) {
		onBoardConnectionReport();
	}
}

void TestService::onBoardConnectionReport() {
	Message report = createTM(TestService::MessageType::OnBoardConnectionTestReport);
	report.appendUint16(ApplicationId);
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
