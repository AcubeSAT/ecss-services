#include "../Services/ServiceTests.hpp"
#include "Time/Time.hpp"
#include "Time/UTCTimestamp.hpp"
#include "Message.hpp"
#include "catch2/catch_all.hpp"

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

TEST_CASE("UTC timestamp addition") {
	using namespace std::chrono_literals;

	UTCTimestamp time1 = UTCTimestamp{2020, 1, 1, 0, 0, 0};
	UTCTimestamp time2 = UTCTimestamp{2035, 11, 19, 23, 57, 24};

	SECTION("Valid ranges") {
		auto time = time1;
		time += -1s;
		CHECK(ServiceTests::thrownError(ErrorHandler::InvalidTimeStampInput));
	}

	SECTION("Simple addition") {
		auto time = time1;
		time += 10s;
		CHECK(time == UTCTimestamp{2020, 1, 1, 0, 0, 10});

		time += 25h;
		CHECK(time == UTCTimestamp{2020, 1, 2, 1, 0, 10});
	}

	SECTION("Overflow within range") {
		auto time = time2;
		time += 1209780s;
		CHECK(time == UTCTimestamp{2035, 12, 4, 0, 0, 24});

		time += 60 * 24h;
		CHECK(time == UTCTimestamp{2036, 2, 2, 0, 0, 24});
	}

	SECTION("Future dates") {
		auto time = time2;
		time += 999999h;
		CHECK(time == UTCTimestamp{2149, 12, 18, 14, 57, 24});
	}
}

TEST_CASE("CUC Custom Timestamp as Parameter") {
	using namespace Time;
	DefaultCUC time(999_t);

	auto parameter = Parameter<DefaultCUC>(time);

	auto message = Message(0, 0, Message::TC);
	parameter.appendValueToMessage(message);
	CHECK(message.dataSize == 4);

	parameter.setValueFromMessage(message);
	CHECK(time == parameter.getValue());
}
