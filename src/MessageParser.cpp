#include "MessageParser.hpp"

TestService TestService::instance;
RequestVerificationService RequestVerificationService::instance;

void MessageParser::execute(Message &message) {
	switch (message.serviceType) {
		case 1:
			RequestVerificationService::instance.execute(message);
			break;
		case 17:
			TestService::instance.execute(message);
			break;
		default:
			// cout is very bad for embedded systems
			std::cout << "This service hasn't been implemented yet or it doesn't exist";
			break;
	}
}
