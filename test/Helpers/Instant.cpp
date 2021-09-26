#include "catch2/catch.hpp"
#include "Helpers/Instant.hpp"
#include "../Services/ServiceTests.hpp"
#include <typeinfo>
#include <iostream>

TEST_CASE("Instant class construction"){
  SECTION("Valid initialization at epoch"){
    Instant<1, 2> Epoch1;
    Instant<4, 4> Epoch2;
    REQUIRE(Epoch1.as_TAI_seconds() == 0);
    REQUIRE(Epoch2.as_TAI_seconds() == 0);
    const std::type_info& expected_type1 = typeid(uint8_t);
    const std::type_info& expected_type2 = typeid(uint16_t);
    CHECK(Epoch1.check_header_type() == expected_type1);
    CHECK(Epoch1.check_header_type() != expected_type2);
    CHECK(Epoch2.check_header_type() == expected_type2);
  }

  // SECTION("Initialize with excessive precision, breaks at compile time"){
  //   Instant<5, 10> Epoch3;
  //   Instant<4, 4> Epoch4;
  // }

  SECTION("Test CUC headers generation"){
    uint8_t cuc_header1 = build_CUC_header<uint8_t, 2, 2>();
    CHECK(cuc_header1 == 0b00100110);

    uint8_t cuc_header2 = build_CUC_header<uint8_t, 4, 1>();
    CHECK(cuc_header2 == 0b00101101);

    uint8_t cuc_header3 = build_CUC_header<uint8_t, 1, 1>();
    CHECK(cuc_header3 == 0b00100001);

    uint16_t cuc_header4 = build_CUC_header<uint16_t, 5, 1>();
    CHECK(cuc_header4 == 0b1010110110100000);

    uint16_t cuc_header5 = build_CUC_header<uint16_t, 1, 6>();
    CHECK(cuc_header5 == 0b1010001110011000);

    uint16_t cuc_header6 = build_CUC_header<uint16_t, 7, 1>();
    CHECK(cuc_header6 == 0b1010110111100000);

    //TODO ADD MORE TESTS HERE ONCE FULL PRECISION RANGE IS SUPPORTED IN INTERNAL REPRESENTATION

  }

  SECTION("Check TAI idempotence"){
    int input_time = 1000;
    Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    REQUIRE(Epoch.as_TAI_seconds() == input_time); //check initialization has intended effect
  }

  SECTION("Check CUC idempotence"){
    etl::array<uint8_t, 9> input_time = {10, 0, 1, 1, 3, 1, 2};
    Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    //REQUIRE(Epoch.as_TAI_seconds() == input_time); //check initialization has intended effect
  }

  SECTION("Check UTC idempotence"){
    // TODO
    //etl::array<uint8_t, 9> input_time = {10, 0, 1, 1, 3, 1, 2};
    //Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    //REQUIRE(Epoch.as_TAI_seconds() == input_time); //check initialization has intended effect
  }

  SECTION("Check CDS idempotence"){
    // TODO
    //etl::array<uint8_t, 9> input_time = {10, 0, 1, 1, 3, 1, 2};
    //Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    //REQUIRE(Epoch.as_TAI_seconds() == input_time); //check initialization has intended effect
  }

  // SECTION("Check different templates, should break at compile"){
  //   Instant<1, 2> Epoch1;
  //   Instant<4, 4> Epoch2;
  //   REQUIRE(Epoch1==Epoch2);
  // }

  SECTION("Check operators"){
    Instant<1, 2> Epoch1;
    Instant<1, 2> Epoch2;
    Instant<1, 2> Epoch3(10);//todo introduce variety in the initialization
    Instant<1, 2> Epoch4(15);
    Instant<1, 2> Epoch5(12);
    Instant<1, 2> Epoch6(10);
    Instant<2, 2> Epoch7;
    REQUIRE(Epoch1==Epoch2);
    REQUIRE(Epoch2==Epoch1); //check symmetry
    REQUIRE(Epoch3==Epoch6);
    REQUIRE(Epoch1!=Epoch3);
    REQUIRE(Epoch3!=Epoch5);
    REQUIRE(Epoch3<=Epoch5);
    REQUIRE(Epoch3<Epoch5);

    //REQUIRE(Epoch1==Epoch7); //should fail at compile, different templates
  }

  SECTION("Check runtime class size"){
    int input_time = 1000;
    Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    REQUIRE(sizeof(Epoch) < 32);
  }
}
