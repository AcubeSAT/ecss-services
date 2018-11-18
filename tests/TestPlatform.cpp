#define CATCH_CONFIG_EXTERNAL_INTERFACES

#include <catch2/catch.hpp>
#include <Message.hpp>
#include <Service.hpp>
#include "Services/ServiceTests.hpp"

std::vector<Message> ServiceTests::queuedMessages = std::vector<Message>();

void Service::storeMessage(const Message &message) {
	ServiceTests::queue(message);
}

struct ServiceTestsListener : Catch::TestEventListenerBase {
	using TestEventListenerBase::TestEventListenerBase; // inherit constructor

	void testCaseStarting(Catch::TestCaseInfo const &testInfo) override {
		// Perform some setup before a test case is run
	}

	void testCaseEnded(Catch::TestCaseStats const &testCaseStats) override {
		// Tear-down after a test case is run
		ServiceTests::reset();
	}
};
CATCH_REGISTER_LISTENER(ServiceTestsListener)
