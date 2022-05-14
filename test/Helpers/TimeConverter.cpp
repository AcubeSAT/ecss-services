#include "TimeConverter.hpp"
#include "catch2/catch.hpp"

TEST_CASE("Conversion of UTC time, to CUC") {
	SECTION("test 1") {
		// Convert UTC to custom CUC format.
		UTCTimestamp timeUTC(2020, 4, 10, 10, 15, 0);
		auto timeCUC = TimeConverter::currentTimeCustomCUC();

		REQUIRE(timeCUC == 86769000);

		// Given the custom CUC format, get the initial UTC timestamp.
		Time::CustomCUC_t customCUC1 = {timeCUC};
		TimeStamp<3, 2> timeStamp(customCUC1);
		auto timeUTC2 = timeStamp.toUTCtimestamp();

		REQUIRE(timeUTC == timeUTC2);
	}
}
