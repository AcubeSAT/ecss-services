#include "catch2/catch.hpp"
#include "Helpers/Instant.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Instant class construction"){
  SECTION("Valid initialization"){
    Instant<1, 2> Epoch1;
    Instant<4, 4> Epoch2;
    REQUIRE(Epoch1.as_TAI_seconds() == 0);
    REQUIRE(Epoch2.as_TAI_seconds() == 0);
  }

  // SECTION("Initialize with excessive precision, breaks at compile time"){
  //   Instant<5, 10> Epoch3;
  //   Instant<4, 4> Epoch4;
  // }

  SECTION("Check TAI idempotence"){
    int input_time = 1000;
    Instant<CUC_seconds_counter_bytes, CUC_fractional_counter_bytes> Epoch;
    Epoch.update_from_TAI_seconds(input_time);
    REQUIRE(Epoch.as_TAI_seconds() == input_time);
  }
}
