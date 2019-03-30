#define CATCH_CONFIG_EXTERNAL_INTERFACES

#include <catch2/catch.hpp>
#include <Message.hpp>
#include <Service.hpp>
#include "Services/ServiceTests.hpp"

// Explicit template specializations for the logError() function
template void ErrorHandler::logError(const Message &, ErrorHandler::AcceptanceErrorType);
template void ErrorHandler::logError(const Message &, ErrorHandler::ExecutionStartErrorType);
template void ErrorHandler::logError(const Message &, ErrorHandler::ExecutionProgressErrorType);
template void ErrorHandler::logError(const Message &, ErrorHandler::ExecutionCompletionErrorType);
template void ErrorHandler::logError(const Message &, ErrorHandler::RoutingErrorType);
template void ErrorHandler::logError(ErrorHandler::InternalErrorType);

// Initialisation of ServiceTests properties
std::vector<Message> ServiceTests::queuedMessages = std::vector<Message>();
std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool> ServiceTests::thrownErrors =
	std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool>();
bool ServiceTests::expectingErrors = false;

void Service::storeMessage(Message &message) {
	// Just add the message to the queue
	ServiceTests::queue(message);
}

template<typename ErrorType>
void ErrorHandler::logError(const Message &message, ErrorType errorType) {
	logError(errorType);
}

template<typename ErrorType>
void ErrorHandler::logError(ErrorType errorType) {
	ServiceTests::addError(ErrorHandler::findErrorSource(errorType), errorType);
}

struct ServiceTestsListener : Catch::TestEventListenerBase {
	using TestEventListenerBase::TestEventListenerBase; // inherit constructor

	void sectionEnded(Catch::SectionStats const &sectionStats) override {
		// Make sure we don't have any errors
		if (not ServiceTests::isExpectingErrors()) {
			// An Error was thrown with this Message. If you expected this to happen, please call a
			// corresponding assertion function from ServiceTests to silence this message.

			//TODO: Uncomment the following line as soon as Issue #19 is closed
			// CHECK(ServiceTests::hasNoErrors());
		}
	}

	void testCaseEnded(Catch::TestCaseStats const &testCaseStats) override {
		// Tear-down after a test case is run
		ServiceTests::reset();
	}
};
CATCH_REGISTER_LISTENER(ServiceTestsListener)
