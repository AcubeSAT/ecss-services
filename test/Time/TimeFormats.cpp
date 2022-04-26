#include "catch2/catch.hpp"
#include "Time/Time.hpp"
#include "Time/UTCTimestamp.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("UTC timestamps") {
	// invalid year
	UTCTimestamp Timestamp0{1950, 4, 10, 10, 15, 0};

	// invalid month
	UTCTimestamp Timestamp1{2020, 60, 10, 10, 15, 0};

	// invalid day
	UTCTimestamp Timestamp2{2020, 4, 35, 10, 15, 0};

	// invalid hour
	UTCTimestamp Timestamp3{2020, 4, 10, 100, 15, 0};

	// invalid minute
	UTCTimestamp Timestamp4{2020, 4, 10, 10, 200, 0};

	// invalid second
	UTCTimestamp Timestamp5{2020, 4, 10, 10, 15, 100};

	CHECK(ServiceTests::countErrors() == 6);
	CHECK(ServiceTests::thrownError(ErrorHandler::InvalidDate));
}
