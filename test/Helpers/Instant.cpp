#include "catch2/catch.hpp"
#include "Helpers/Instant.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Instant class construction"){
  SECTION("Valid initialization"){
    Instant<CUC_seconds_counter_bytes, CUC_fractional_counter_bytes> Epoch;
    Instant<4, 4> Epoch2;
  }
  // SECTION("Initialize with excessive precision, breaks at compile time"){
  //   Instant<5, 10> Epoch3;
  //   Instant<4, 4> Epoch4;
  // }
}
