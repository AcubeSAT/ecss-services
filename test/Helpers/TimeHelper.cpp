#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"

TEST_CASE("Time format implementation", "[CUC]") {
	// very simple tests for the TimeHelper

	CHECK(TimeHelper::implementCUCTimeFormat(60) == 0b11110000110010);
	CHECK(TimeHelper::implementCUCTimeFormat(1000) == 0x3E832);
	CHECK(TimeHelper::implementCUCTimeFormat(1200) == 0x4B032);
}
