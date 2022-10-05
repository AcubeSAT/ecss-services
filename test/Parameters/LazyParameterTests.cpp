#include "../Services/ServiceTests.hpp"
#include "ECSSMessage.hpp"
#include "Helpers/LazyParameter.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Lazy Parameter: Getter") {
	LazyParameter<uint32_t> parameter;
	CHECK(parameter.getValue().has_value() == false);

	parameter.setGetter([]() -> auto{
		static uint32_t value = 99100;
		return value++;
	});

	CHECK(parameter.getValue().value() == 99100);
	CHECK(parameter.getValue().value() == 99101);
	CHECK(parameter.getValueAsDouble() == 99102);

	parameter.unsetGetter();

	CHECK(parameter.getValue().has_value() == false);
	CHECK(parameter.getValueAsDouble() == 0);
}

TEST_CASE("Lazy Parameter: Messages") {
	const int16_t fallback = -1;
	const int16_t value = 42;

	LazyParameter<int16_t> parameter(
	    []() -> auto{
		    return value;
	    },
	    fallback);

	SECTION("Getter works") {
		ECSSMessage message(0, 0, ECSSMessage::TC);
		parameter.appendValueToMessage(message);

		CHECK(message.dataSize == 2);
		CHECK(message.readSint16() == value);
		CHECK(ServiceTests::hasNoErrors());
	}

	SECTION("Getter broken") {
		parameter.unsetGetter();

		ECSSMessage message(0, 0, ECSSMessage::TC);
		parameter.appendValueToMessage(message);

		CHECK(message.dataSize == 2);
		CHECK(message.readSint16() == fallback);
		CHECK(ServiceTests::thrownError(ErrorHandler::ParameterValueMissing));
	}

	SECTION("Attempt to write") {
		ECSSMessage message(0, 0, ECSSMessage::TC);
		message.appendSint16(99);
		message.appendSint16(100);

		parameter.setValueFromMessage(message);
		CHECK(ServiceTests::thrownError(ErrorHandler::ParameterReadOnly));

		// Make sure that the appropriate amount of bytes has been skipped
		CHECK(message.readSint16() == 100);
	}
}

TEST_CASE("Lazy Parameter: Complex Types") {
	using namespace Time;

	LazyParameter<Time::DefaultCUC> parameter(Time::DefaultCUC(100_t));

	SECTION("Default value") {
		CHECK(parameter.getValue().has_value() == false);
	}

	SECTION("Set value") {
		parameter.setGetter([]() -> Time::DefaultCUC {
			return Time::DefaultCUC(200_t);
		});

		CHECK(parameter.getValue().value() == Time::DefaultCUC{200_t});
		CHECK(parameter.getValueAsDouble() == 0);
	}
}
