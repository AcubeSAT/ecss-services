#include "Services/TestService.hpp"

void TestService::areYouAlive(const Message &message) {
	// TM[17,2] are-you-alive connection test report
	Message report = createTM(2);

	storeMessage(report);
}
