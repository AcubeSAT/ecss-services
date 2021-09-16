#include "catch2/catch.hpp"
#include "Helpers/Instant.hpp"
#include "../Services/ServiceTests.hpp"
#include <typeinfo>

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

  SECTION("Check runtime class size"){
    int input_time = 1000;
    Instant<Acubesat_CUC_seconds_counter_bytes, Acubesat_CUC_fractional_counter_bytes> Epoch(input_time);
    REQUIRE(sizeof(Epoch) < 32);
  }
}
