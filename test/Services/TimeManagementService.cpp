#include <catch2/catch.hpp>
#include <Services/TimeManagementService.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[9,2]", "[service][st09]") {
	TimeManagementService timeFormat;

	uint32_t seconds;
	seconds = time(nullptr);

	timeFormat.cucTimeReport();
	Message response = ServiceTests::get(0);
	CHECK(response.readByte() == 50);
	CHECK(response.readWord() == seconds);

}
