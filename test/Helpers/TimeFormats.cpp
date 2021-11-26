#include "catch2/catch.hpp"
#include "Helpers/TimeFormats.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("UTC timestamps") {
	// invalid year
	UTC_Timestamp Timestamp0{2018, 4, 10, 10, 15, 0};

	// invalid month
	UTC_Timestamp Timestamp1{2020, 60, 10, 10, 15, 0};

	// invalid day
	UTC_Timestamp Timestamp2{2020, 4, 35, 10, 15, 0};

	// invalid hour
	UTC_Timestamp Timestamp3{2020, 4, 10, 100, 15, 0};

	// invalid minute
	UTC_Timestamp Timestamp4{2020, 4, 10, 10, 200, 0};

	// invalid second
	UTC_Timestamp Timestamp5{2020, 4, 10, 10, 15, 100};

	CHECK(ServiceTests::countErrors() == 6);
	CHECK(ServiceTests::thrownError(ErrorHandler::InvalidDate));
}
