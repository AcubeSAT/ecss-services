#include "catch2/catch.hpp"
#include "Helpers/Instant.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Instant class construction"){
  SECTION("Valid initialization"){
    Instant<1, 2> Epoch;
    Instant<4, 4> Epoch2;
    REQUIRE(Epoch.as_TAI_seconds() == 0);
    REQUIRE(Epoch2.as_TAI_seconds() == 0);
    REQUIRE(Epoch.as_TAI_seconds() == 10); //this test seems to do nothing
    REQUIRE(1 == 10); //this test seems to do nothing
  }
  // SECTION("Initialize with excessive precision, breaks at compile time"){
  //   Instant<5, 10> Epoch3;
  //   Instant<4, 4> Epoch4;
  // }
}
