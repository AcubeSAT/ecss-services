#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cstdint>
#include <cmath>
#include "Helpers/Result.hpp"

using namespace Catch::literals;

TEST_CASE("Result class size") {
	STATIC_CHECK(sizeof(Result<uint16_t, int>) == 8);
	STATIC_CHECK(sizeof(Result<uint64_t, uint8_t>) == 16);
}

TEST_CASE("Boolean operators") {
	SECTION("Successful result") {
		Result<uint64_t, uint8_t> success = Ok(15UL);

		if (!success) {
			FAIL("Success result must be true");
		}

		CHECK(success.has_value());
		CHECK_FALSE(success.has_error());
	}

	SECTION("Error result") {
		Result<uint64_t, int> failure(Err(15));

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

enum class Operation {
	Constructor,
	Destructor,
	CopyConstructor,
	CopyAssignment,
	MoveConstructor,
	MoveAssignment,
};

template<int Tag>
struct OperatorTester {
	uint8_t padding[4 * (Tag + 1)];

	Operation lastOperation;

	OperatorTester() { lastOperation = Operation::Constructor; } // NOLINT(cppcoreguidelines-pro-type-member-init)

	~OperatorTester() { lastOperation = Operation::Destructor; }

	OperatorTester(const OperatorTester&) { lastOperation = Operation::CopyConstructor; }

	OperatorTester& operator=(const OperatorTester&) { lastOperation = Operation::CopyAssignment; return *this; }
};

#pragma GCC push_options
#pragma GCC optimize ("O0")
TEST_CASE("Copy and move operators") {
	SECTION("Copy constructor from other similar Result") {
		Result<double, int> success = Ok(16.0);
		Result<double, int> error = Err(1);

		auto copy1 = success;

		REQUIRE(copy1);
		CHECK(*copy1 == 16_a);

		auto copy2 = error;
		REQUIRE_FALSE(copy2);
		CHECK(copy2.error() == 1);
	}

	SECTION("Copy constructor from other different Result") {
		Result<float, int> success = Ok(16.0f);
		Result<float, int> error = Err(1);

		Result<double, long int> copy1 = success;

		REQUIRE(copy1);
		CHECK(*copy1 == 16_a);

		Result<double, long int> copy2 = error;
		REQUIRE_FALSE(copy2);
		CHECK(copy2.error() == 1);
	}

	SECTION("Copy assignment from other similar Result") {
		using TestResult = Result<OperatorTester<0>, OperatorTester<1>>;

		TestResult success = Ok(OperatorTester<0>());
		TestResult error = Err(OperatorTester<1>());

		CHECK(success.unwrap().lastOperation == Operation::CopyConstructor);

		CHECK(error.error().lastOperation == Operation::CopyConstructor);


		success = error;
		REQUIRE(success.has_error());
		CHECK((*success).lastOperation == Operation::Destructor);
		CHECK(success.error().lastOperation == Operation::CopyConstructor);
	}
}
#pragma GCC pop_options


TEST_CASE("Map") {
	Result<double, int> success = Ok(16.0);
	Result<double, int> error = Err(100);

	auto functionThatMayFail([](double input) {
		return Result<double, uint64_t>(Err(50UL));
	});

	SECTION("Mapping success with a simple function") {
		auto mapped = success.map(sqrt);

		STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, double>);
		STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, int>);
		REQUIRE(mapped);
		CHECK(*mapped == 4.0_a);
	}

	SECTION("Mapping success with a Result function") {
		auto mapped = success.map(functionThatMayFail);

		STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, double>);
		STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, uint64_t>);
		REQUIRE_FALSE(mapped);
		CHECK(mapped.error() == 50);
	}

	SECTION("Mapping errors with a simple function") {
		auto mapped = error.map(sqrt);

		STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, double>);
		STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, int>);
		REQUIRE_FALSE(mapped);
		CHECK(mapped.error() == error.error());
	}

	SECTION("Mapping errors with a Result function") {
		auto mapped = error.map(functionThatMayFail);

		STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, double>);
		STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, uint64_t>);
		REQUIRE_FALSE(mapped);
		CHECK(mapped.error() == 100);
	}

	SECTION("Mapping errors with incompatible functions") {
		SECTION("Identical error") {
			auto mapped = error.map([](double in) -> Result<int, int> { return 0; });

			STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, int>);
			STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, decltype(error)::ErrorType>);
			REQUIRE_FALSE(mapped);
			CHECK(mapped.error() == 100);
		}

		SECTION("Integer error") {
			auto mapped = error.map([](double in) -> Result<int, uint64_t> { return 0; });

			STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, int>);
			STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, uint64_t>);
			REQUIRE_FALSE(mapped);
			CHECK(mapped.error() == 100);
		}

		SECTION("String error") {
			auto mapped = error.map([](double in) -> Result<int, std::string> { return {"Error!"}; });

			STATIC_CHECK(std::is_same_v<decltype(mapped)::ValueType, int>);
			STATIC_CHECK(std::is_same_v<decltype(mapped)::ErrorType, std::string>);
			REQUIRE_FALSE(mapped);
			CHECK(mapped.error().empty());
		}
	}
}
