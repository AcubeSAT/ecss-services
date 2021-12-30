#include "catch2/catch.hpp"
#include "Helpers/TimeStamp.hpp"
#include "../Services/ServiceTests.hpp"
#include <typeinfo>
#include <iostream>

using namespace Time;

TEST_CASE("TimeStamp class construction") {
	// SECTION("Initialize with excessive precision, breaks at compile time"){
	//   TimeStamp<5, 10> Epoch3;
	//   TimeStamp<4, 4> Epoch4;
	// }

	SECTION("Test CUC headers generation") {
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

	SECTION("Check TAI idempotence") {
		int input_time = 1000;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(input_time);
		REQUIRE(Epoch.asTAIseconds() == input_time); // check initialization has intended effect
	}

	SECTION("Check CUC idempotence") {
		etl::array<uint8_t, 9> input_time1 = {0b00100110, 0, 1, 1, 3, 0, 0, 0, 0};
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch1(input_time1);
		etl::array<uint8_t, 9> test_return1 = Epoch1.toCUCtimestamp();

		for (auto i = 0; i < 9; i++) {
			bool test = input_time1[i] == test_return1[i];
			REQUIRE(test);
		}

		etl::array<uint8_t, 9> input_time2 = {0b10101101, 0b10100000, 218, 103, 11, 0, 3, 23, 0};
		TimeStamp<5, 1> Epoch2(input_time2);
		etl::array<uint8_t, 9> test_return2 = Epoch2.toCUCtimestamp();

		for (auto i = 0; i < 9; i++) {
			bool test = input_time2[i] == test_return2[i];
			REQUIRE(test);
		}

		etl::array<uint8_t, 9> input_time3 = {0b10100011, 0b10001100, 218, 103, 11, 0, 3, 23, 2};
		TimeStamp<1, 6> Epoch3(input_time3);
		etl::array<uint8_t, 9> test_return3 = Epoch3.toCUCtimestamp();

		for (auto i = 0; i < 9; i++) {
			bool test = input_time3[i] == test_return3[i];
			REQUIRE(test);
		}
	}

	SECTION("Check UTC idempotence") {
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

	SECTION("UTC conversion to and from seconds timestamps") {
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

	SECTION("Check operators") {
		TimeStamp<1, 2> Epoch1;
		TimeStamp<1, 2> Epoch2;
		TimeStamp<1, 2> Epoch3(10);
		TimeStamp<1, 2> Epoch4(15);
		TimeStamp<1, 2> Epoch5(12);
		TimeStamp<1, 2> Epoch6(10);
		TimeStamp<2, 2> Epoch7;
		REQUIRE(Epoch1 == Epoch2);
		REQUIRE(Epoch2 == Epoch1);
		REQUIRE(Epoch3 == Epoch6);
		REQUIRE(Epoch1 != Epoch3);
		REQUIRE(Epoch3 != Epoch5);
		REQUIRE(Epoch3 <= Epoch5);
		REQUIRE(Epoch3 < Epoch5);

		// REQUIRE(Epoch1==Epoch7); //should fail at compile, different templates
	}

	SECTION("Check runtime class size") {
		int input_time = 1000;
		TimeStamp<CUCSecondsBytes, CUCFractionalBytes> Epoch(input_time);
		REQUIRE(sizeof(Epoch) < 32);
	}
}
