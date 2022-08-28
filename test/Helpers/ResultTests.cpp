#include <catch2/catch_test_macros.hpp>
#include "Helpers/Result.hpp"

TEST_CASE("Result class size") {
	STATIC_CHECK(sizeof(Result<uint16_t, uint32_t>) == 8);
	STATIC_CHECK(sizeof(Result<uint64_t, uint8_t>) == 16);
}

TEST_CASE("Boolean operators") {
	SECTION("Successful result") {
		Result<uint64_t, uint8_t> success(uint64_t{15});

		if (!success) {
			FAIL("Success result must be true");
		}

		CHECK(success.has_value());
		CHECK_FALSE(success.has_error());
	}

	SECTION("Error result") {
		Result<uint64_t, uint8_t> failure(uint8_t{15});

		if (failure) {
			FAIL("Failure result must be false");
		}

		CHECK_FALSE(failure.has_value());
		CHECK(failure.has_error());
	}
}

TEST_CASE("Value or") {
	Result<uint64_t, uint8_t> success(uint64_t{15});
	Result<uint64_t, uint8_t> error(uint8_t{15});

	CHECK(error.valueOr() == 0);
}

TEST_CASE("Error access") {
	Result<uint64_t, uint8_t> error(uint8_t{15});

	CHECK(error.error() == 15);
}
