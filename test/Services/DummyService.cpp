#include "Services/DummyService.hpp"
#include <catch2/catch_all.hpp>
#include "ServicePool.hpp"
#include "ServiceTests.hpp"

DummyService& dummyService = Services.dummyService;

TEST_CASE("Log string as message TM[128, 128]", "[service][st128]") {
	etl::string<LOGGER_MAX_MESSAGE_SIZE> log = "An amazing log that is very informative";
	dummyService.logAsECSSMessage(log);
	Message report = ServiceTests::get(0);
	CHECK(report.serviceType == DummyService::ServiceType);
	CHECK(report.messageType == DummyService::MessageType::LogString);
	char logOutput[39];
	report.readString(logOutput, 39);
	CHECK(memcmp(logOutput, "An amazing log that is very informative", 39) == 0);
}
