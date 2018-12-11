#include <catch2/catch.hpp>
#include <Services/TimeManagementService.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[9,2]", "[service][st09]") {
	TimeManagementService timeFormat;

	uint32_t seconds = 60;
	timeFormat.cucTimeReport(seconds);
	Message response = ServiceTests::get(0);
	CHECK(response.readByte() == 50);
	CHECK(response.readWord() == seconds);

	seconds = 100;
	timeFormat.cucTimeReport(seconds);
	response = ServiceTests::get(1);
	CHECK(response.readByte() == 50);
	CHECK(response.readWord() == seconds);

	seconds = 1000;
	timeFormat.cucTimeReport(seconds);
	response = ServiceTests::get(2);
	CHECK(response.readByte() == 50);
	CHECK(response.readWord() == seconds);
}
