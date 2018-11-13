#include "Services/TestService.hpp"

void TestService::areYouAlive(const Message &request) {
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(2);

	storeMessage(report);
}

void TestService::onBoardConnection(const Message &request) {
	// TM[17,4] on-board connection test report
	Message report = createTM(4);

	// TODO: This is not the correct way to do this! Fetching functions will be added later
	report.appendUint8(request.data[1]);

	storeMessage(report);
}
