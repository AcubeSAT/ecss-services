#include "../Services/ServiceTests.hpp"
#include "Time/TimeStamp.hpp"
#include "catch2/catch_all.hpp"

using namespace Time;
using Catch::Approx;

TEST_CASE("TimeStamp class construction") {
	// SECTION("Initialize with excessive precision, breaks at compile time"){
	//   TimeStamp<5, 10> Epoch3;
	//   TimeStamp<4, 4> Epoch4;
	// }
}

TEST_CASE("CUC headers generation") {
	auto cuc_header1 = buildCUCHeader<uint8_t, 2, 2>();
	CHECK(cuc_header1 == 0b00100110);

	auto cuc_header2 = buildCUCHeader<uint8_t, 4, 1>();
	CHECK(cuc_header2 == 0b00101101);

	auto cuc_header3 = buildCUCHeader<uint8_t, 1, 1>();
	CHECK(cuc_header3 == 0b00100001);

	auto cuc_header4 = buildCUCHeader<uint16_t, 5, 1>();
	CHECK(cuc_header4 == 0b1010110110100000);

	auto cuc_header5 = buildCUCHeader<uint16_t, 1, 6>();
	CHECK(cuc_header5 == 0b1010001110001100);

	auto cuc_header6 = buildCUCHeader<uint16_t, 7, 1>();
	CHECK(cuc_header6 == 0b1010110111100000);
}

TEST_CASE("TAI idempotence") {
	int input_time = 1000;
	TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(input_time);

	REQUIRE(time.asTAIseconds() == input_time);
}

TEST_CASE("CUC idempotence") {
	etl::array<uint8_t, 9> input1 = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
	TimeStamp<3, 2> time1(input1);
	etl::array<uint8_t, 9> cuc1 = time1.formatAsCUC();

	for (uint8_t i = 0; i < 9; i++) {
		CHECK(input1[i] == cuc1[i]);
	}

	etl::array<uint8_t, 9> input2 = {0b10101101, 0b10100000, 218, 103, 11, 0, 3, 23, 0};
	TimeStamp<5, 1> time2(input2);
	etl::array<uint8_t, 9> cuc2 = time2.formatAsCUC();

	for (auto i = 0; i < 9; i++) {
		CHECK(input2[i] == cuc2[i]);
	}

	etl::array<uint8_t, 9> input3 = {0b10100011, 0b10001100, 218, 103, 11, 0, 3, 23, 2};
	TimeStamp<1, 6> time3(input3);
	etl::array<uint8_t, 9> cuc3 = time3.formatAsCUC();

	for (auto i = 0; i < 9; i++) {
		CHECK(input3[i] == cuc3[i]);
	}
}

TEST_CASE("Conversion between CUC formats") {
	SECTION("Base unit conversion") {
		TimeStamp<2, 2> time1(20123);
		TimeStamp<5, 2> time2(time1.formatAsCUC());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}

	SECTION("Floating unit conversion") {
		etl::array<uint8_t, 9> timeInput = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
		TimeStamp<3, 2> time1(timeInput);

		TimeStamp<3, 5> time2(time1.formatAsCUC());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}

	SECTION("All units conversion") {
		etl::array<uint8_t, 9> timeInput = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
		TimeStamp<3, 2> time1(timeInput);

		TimeStamp<4, 4> time2(time1.formatAsCUC());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}
}

TEST_CASE("Use of custom Acubesat CUC format") {
	SECTION("Check forward conversion") {
		Time::CustomCUC_t customCUC1 = {1001};
		TimeStamp<3, 0> time1(customCUC1);
		CHECK(time1.asTAIseconds() == 100);
		CHECK(time1.asCustomCUCTimestamp().elapsed100msTicks == 1000);
		TimeStamp<3, 2> time2(customCUC1);
		CHECK(time2.asTAIseconds() == 100);
		CHECK(time2.asCustomCUCTimestamp().elapsed100msTicks == 1001);

		// check rounding errors
		Time::CustomCUC_t customCUC2 = {1004};
		TimeStamp<3, 0> time3(customCUC2);
		CHECK(time3.asTAIseconds() == 100);
		CHECK(time3.asCustomCUCTimestamp().elapsed100msTicks == 1000);
		TimeStamp<3, 2> time4(customCUC2);
		CHECK(time4.asTAIseconds() == 100);
		CHECK(time4.asCustomCUCTimestamp().elapsed100msTicks == 1004);

		// check rounding errors
		Time::CustomCUC_t customCUC3 = {1005};
		TimeStamp<3, 0> time5(customCUC3);
		CHECK(time5.asTAIseconds() == 101);
		CHECK(time5.asCustomCUCTimestamp().elapsed100msTicks == 1010);
		TimeStamp<3, 2> time6(customCUC3);
		CHECK(time6.asTAIseconds() == 100);
		CHECK(time6.asCustomCUCTimestamp().elapsed100msTicks == 1005);
	}

	SECTION("Check idempotence") {
		Time::CustomCUC_t customCUC1 = {1000};
		TimeStamp<3, 3> time1(customCUC1);
		Time::CustomCUC_t customCUC2 = time1.asCustomCUCTimestamp();
		CHECK(customCUC1.elapsed100msTicks == customCUC2.elapsed100msTicks);
	}
}

TEST_CASE("UTC idempotence") {
	{
		UTCTimestamp timestamp1(2020, 4, 10, 10, 15, 0); // 10 Apr 2020, 10:15:00;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(timestamp1);
		UTCTimestamp timestamp2 = time.toUTCtimestamp();
		bool cond = (timestamp2 == timestamp1);
		REQUIRE(cond);
	}
	{
		UTCTimestamp timestamp1(2035, 1, 1, 0, 0, 1); // 1 Jan 2035 midnight passed;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(timestamp1);
		UTCTimestamp timestamp2 = time.toUTCtimestamp();
		bool cond = (timestamp2 == timestamp1);
		REQUIRE(cond);
	}
}

TEST_CASE("UTC conversion to and from seconds timestamps") {
	{
		UTCTimestamp utc(2020, 12, 5, 0, 0, 0);
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(utc);
		REQUIRE(time.asTAIseconds() == 29289600);
	}
	{
		UTCTimestamp utc(2020, 2, 29, 0, 0, 0);
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(utc);
		REQUIRE(time.asTAIseconds() == 5097600);
	}
	{
		UTCTimestamp utc(2025, 3, 10, 0, 0, 0);
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> time(utc);
		REQUIRE(time.asTAIseconds() == 163728000);
	}
}

TEST_CASE("UTC overflow tests") {
	SECTION("Year too high") {
		UTCTimestamp utc(2999, 3, 11, 0, 0, 0);
		TimeStamp<2, 1> time(utc);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::TimeStampOutOfBounds));
		ServiceTests::reset();
	}
	SECTION("Seconds too high, small variable") {
		UTCTimestamp utc(Epoch.year, Epoch.month, Epoch.day, 0, 7, 0);
		TimeStamp<1, 1> time(utc);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::TimeStampOutOfBounds));
		ServiceTests::reset();
	}
	SECTION("Seconds too high, wide variable") {
		UTCTimestamp utc(Epoch.year, Epoch.month, Epoch.day, 0, 7, 0);
		TimeStamp<1, 4> time(utc);
		REQUIRE(ServiceTests::thrownError(ErrorHandler::TimeStampOutOfBounds));
		ServiceTests::reset();
	}
}

// SECTION("Check different templates, should break at compile"){
//   TimeStamp<1, 2> time1;
//   TimeStamp<4, 4> time2;
//   REQUIRE(time1==time2);
// }

TEST_CASE("Time operators") {
	SECTION("Same type") {
		TimeStamp<1, 2> time1;
		TimeStamp<1, 2> time2;
		TimeStamp<1, 2> time3(10);
		TimeStamp<1, 2> time4(12);
		TimeStamp<1, 2> time5(10);
		CHECK(time1 == time2);
		CHECK(time2 == time1);
		CHECK(time3 == time5);
		CHECK(time1 != time3);
		CHECK(time3 != time4);
		CHECK(time3 <= time4);
		CHECK(time3 < time4);
	}

	SECTION("Different size") {
		TimeStamp<1, 2> time1(10);
		TimeStamp<2, 1> time2(10);
		TimeStamp<3, 2> time3(15);
		TimeStamp<2, 2> time4(5);
		CHECK(time1 == time2);
		CHECK(time3 != time2);
		CHECK(time4 < time2);
		CHECK(time3 > time4);
		CHECK(time2 >= time1);
		CHECK(time1 <= time3);
	}

	SECTION("Different units") {
		TimeStamp<1, 2, 10> time1(10);
		TimeStamp<1, 2, 1, 10> time2(10);
		TimeStamp<3, 2, 3, 2> time3(15);
		TimeStamp<2, 2, 57, 89> time4(5);
		CHECK(time1 == time2);
		CHECK(time3 != time2);
		CHECK(time4 < time2);
		CHECK(time3 > time4);
		CHECK(time2 >= time1);
		CHECK(time1 <= time3);
	}

	SECTION("Overflow") {
		TimeStamp<1, 0> time1(1);
		TimeStamp<4, 4> time2(std::numeric_limits<uint32_t>::max());

		CHECK(time1 != time2);
		CHECK_FALSE(time1 == time2);
		CHECK(time1 < time2);
		CHECK(time1 <= time2);
		CHECK(time2 > time1);
		CHECK(time2 >= time1);
	}
}

TEST_CASE("Time runtime class size") {
	REQUIRE(sizeof(TimeStamp<CUCSecondsBytes, CUCFractionalBytes>) <= 8);
}

TEST_CASE("CUC conversions") {
	SECTION("Base unit, without fractions") {
		TimeStamp<2, 0, 10, 1> time1(100);
		CHECK(time1.asTAIseconds() == 100);

		TimeStamp<2, 0, 1, 10> time2(time1);
		CHECK(time2.asTAIseconds() == 100);
	}

	SECTION("Base unit, with fractions") {
		TimeStamp<2, 2, 10, 1> time1(100);
		CHECK(time1.asTAIseconds() == 100);

		TimeStamp<2, 2, 1, 10> time2(time1);
		CHECK(time2.asTAIseconds() == 100);
	}

	SECTION("Addition of fraction") {
		TimeStamp<2, 0, 1, 1> time1(100);
		CHECK(time1.asTAIseconds() == 100);

		TimeStamp<2, 2, 1, 1> time2(time1);
		CHECK(time2.asTAIseconds() == 100);
	}

	SECTION("Removal of fraction") {
		TimeStamp<2, 2, 1, 1> time1(100);
		CHECK(time1.asTAIseconds() == 100);

		TimeStamp<2, 0, 1, 1> time2(time1);
		CHECK(time2.asTAIseconds() == 100);
	}

	SECTION("Many changes") {
		TimeStamp<2, 2, 3, 2> time1(1000);
		CHECK(time1.asTAIseconds() == Approx(1000).epsilon(1));

		TimeStamp<3, 4, 100, 29> time2(time1);
		CHECK(time2.asTAIseconds() == Approx(1000).epsilon(1));

		TimeStamp<2, 1, 1, 1> time3(time1);
		CHECK(time3.asTAIseconds() == Approx(1000).epsilon(1));
	}

	SECTION("Large numbers") {
		TimeStamp<4, 0, 1, 1> time1(10000);
		CHECK(time1.asTAIseconds() == 10000);

		TimeStamp<2, 1, 7907, 7559> time2(time1);
		CHECK(time2.asTAIseconds() == 9999);
	}
}

TEST_CASE("Duration conversions") {
	using namespace std::chrono_literals;

	SECTION("Conversion to duration") {
		TimeStamp<2, 2, 1, 1> time(3600);
		auto duration = time.asDuration<std::chrono::hours>();
		CHECK(duration == 1h);
	}

	SECTION("Conversion from duration") {
		auto duration = 90min;
		TimeStamp<2, 2, 1, 1> time(duration);
		CHECK(time.asTAIseconds() == 5400);
	}

	SECTION("Duration idempotence") {
		auto duration = 13532s;
		TimeStamp<2, 2, 1, 1> time(duration);

		CHECK(time.asDuration() == duration);
	}

	SECTION("Overflow") {
		auto duration = 24h;
		TimeStamp<2, 2, 1, 1> time(duration);

		CHECK(time.asTAIseconds() == 20864);
	}
}
