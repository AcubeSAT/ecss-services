#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TEST

#include "ServicePool.hpp"
#include "Services/TestService.hpp"

void TestService::areYouAlive(const Message& request) {
	if (!request.assertTC(TestService::ServiceType, TestService::MessageType::AreYouAliveTest)) {
		return;
	}
	areYouAliveReport();
}

void TestService::areYouAliveReport() {
	Message report = createTM(TestService::MessageType::AreYouAliveTestReport);
	storeMessage(report);
}

void TestService::onBoardConnection(Message& request) {
	if (!request.assertTC(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest)) {
		return;
	}
	const ApplicationProcessId applicationProcessId = request.read<ApplicationProcessId>();
	onBoardConnectionReport(applicationProcessId);
}

void TestService::onBoardConnectionReport(ApplicationProcessId applicationProcessId) {
	Message report = createTM(TestService::MessageType::OnBoardConnectionTestReport);
	report.append<ApplicationProcessId>(applicationProcessId);
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
