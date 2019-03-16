#include <catch2/catch.hpp>
#include "ServiceTests.hpp"

// #define private public
#include <Services/TimeBasedSchedulingService.hpp>

namespace unit_test {
	struct Tester {
		TimeBasedSchedulingService gen;
		static bool executionFunctionStatus(TimeBasedSchedulingService tmService) {
			return tmService.executionFunctionStatus;
		}
		static uint8_t currentNumberOfActivities(TimeBasedSchedulingService tmService) {
			return tmService.currentNumberOfActivities;
		}
		static auto scheduledActivities(TimeBasedSchedulingService tmService) {
			return tmService.scheduledActivities;
		}
	};
}


TEST_CASE("TC[11,1]", "[service][st11]") {
	Message receivedMessage;
	receivedMessage.serviceType = 11;
	receivedMessage.messageType = 1;

	TimeBasedSchedulingService timeSchedulingService;
	timeSchedulingService.enableScheduleExecution(receivedMessage);
	CHECK(unit_test::Tester::executionFunctionStatus(timeSchedulingService));
}

TEST_CASE("TC[11,2]", "[service][st11]") {
	Message receivedMessage;
	receivedMessage.serviceType = 11;
	receivedMessage.messageType = 2;

	TimeBasedSchedulingService timeSchedulingService;
	timeSchedulingService.disableScheduleExecution(receivedMessage);
	CHECK(!unit_test::Tester::executionFunctionStatus(timeSchedulingService));
}
