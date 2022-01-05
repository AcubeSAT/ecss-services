#include "catch2/catch.hpp"
#include "Time/TimeStamp.hpp"
#include "../Services/ServiceTests.hpp"
#include <typeinfo>
#include <iostream>

using namespace Time;

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
	TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(input_time);

	REQUIRE(Epoch.asTAIseconds() == input_time);
}

TEST_CASE("CUC idempotence") {
	etl::array<uint8_t, 9> input_time1 = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
	TimeStamp<3, 2> Epoch1(input_time1);
	etl::array<uint8_t, 9> test_return1 = Epoch1.toCUCtimestamp();

	for (uint8_t i = 0; i < 9; i++) {
		CHECK(input_time1[i] == test_return1[i]);
	}

	etl::array<uint8_t, 9> input_time2 = {0b10101101, 0b10100000, 218, 103, 11, 0, 3, 23, 0};
	TimeStamp<5, 1> Epoch2(input_time2);
	etl::array<uint8_t, 9> test_return2 = Epoch2.toCUCtimestamp();

	for (auto i = 0; i < 9; i++) {
		CHECK(input_time2[i] == test_return2[i]);
	}

	etl::array<uint8_t, 9> input_time3 = {0b10100011, 0b10001100, 218, 103, 11, 0, 3, 23, 2};
	TimeStamp<1, 6> Epoch3(input_time3);
	etl::array<uint8_t, 9> test_return3 = Epoch3.toCUCtimestamp();

	for (auto i = 0; i < 9; i++) {
		CHECK(input_time3[i] == test_return3[i]);
	}
}

TEST_CASE("Conversion between CUC formats") {
	SECTION("Base unit conversion") {
		TimeStamp<2, 2> time1(20123);
		TimeStamp<5, 2> time2(time1.toCUCtimestamp());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}

	SECTION("Floating unit conversion") {
		etl::array<uint8_t, 9> timeInput = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
		TimeStamp<3, 2> time1(timeInput);

		TimeStamp<3, 5> time2(time1.toCUCtimestamp());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}

	SECTION("All units conversion") {
		etl::array<uint8_t, 9> timeInput = {0b00101010, 0, 1, 1, 3, 0, 0, 0, 0};
		TimeStamp<3, 2> time1(timeInput);

		TimeStamp<4, 4> time2(time1.toCUCtimestamp());
		CHECK(time1.asTAIseconds() == time2.asTAIseconds());
	}
}

TEST_CASE("UTC idempotence") {
	{
		UTCTimestamp timestamp1(2020, 4, 10, 10, 15, 0); // 10 Apr 2020, 10:15:00;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(timestamp1);
		UTCTimestamp timestamp2 = Epoch.toUTCtimestamp();
		bool cond = (timestamp2 == timestamp1);
		REQUIRE(cond);
	}
	{
		UTCTimestamp timestamp1(2035, 1, 1, 0, 0, 1); // 1 Jan 2035 midnight passed;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(timestamp1);
		UTCTimestamp timestamp2 = Epoch.toUTCtimestamp();
		bool cond = (timestamp2 == timestamp1);
		REQUIRE(cond);
	}
}

TEST_CASE("UTC conversion to and from seconds timestamps") {
	{
		UTCTimestamp timestamp1(2020, 12, 5, 0, 0, 0); // 5 Dec 2020, midnight;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(timestamp1);
		REQUIRE(Epoch.asTAIseconds() == 1607126400);
	}
	{
		UTCTimestamp timestamp1(2020, 2, 29, 0, 0, 0); // 5 Dec 2020, midnight;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(timestamp1);
		REQUIRE(Epoch.asTAIseconds() == 1582934400);
	}
}

// SECTION("Check different templates, should break at compile"){
//   TimeStamp<1, 2> Epoch1;
//   TimeStamp<4, 4> Epoch2;
//   REQUIRE(Epoch1==Epoch2);
// }

TEST_CASE("Time operators") {
	TimeStamp<1, 2> Epoch1;
	TimeStamp<1, 2> Epoch2;
	TimeStamp<1, 2> Epoch3(10);
	TimeStamp<1, 2> Epoch4(12);
	TimeStamp<1, 2> Epoch5(10);
	TimeStamp<2, 2> Epoch6;
	REQUIRE(Epoch1 == Epoch2);
	REQUIRE(Epoch2 == Epoch1);
	REQUIRE(Epoch3 == Epoch5);
	REQUIRE(Epoch1 != Epoch3);
	REQUIRE(Epoch3 != Epoch4);
	REQUIRE(Epoch3 <= Epoch4);
	REQUIRE(Epoch3 < Epoch4);

	// REQUIRE(Epoch1==Epoch6); //should fail at compile, different templates
}

TEST_CASE("Time runtime class size") {
	int input_time = 1000;
	TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(input_time);
	REQUIRE(sizeof(Epoch) < 32);
}
