#include "Helpers/Parameter.hpp"
#include <iostream>
#include "Message.hpp"
#include "Services/ParameterService.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Parameter Append") {
	SECTION("Check correct appending") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                          Message::TC, 1);
		auto parameter1 = Parameter<uint8_t>(1);
		auto parameter2 = Parameter<uint16_t>(500);
		auto parameter3 = Parameter<uint32_t>(70000);

		parameter1.appendValueToMessage(request);
		parameter2.appendValueToMessage(request);
		parameter3.appendValueToMessage(request);

		CHECK(request.readUint8() == 1);
		CHECK(request.readUint16() == 500);
		CHECK(request.readUint32() == 70000);
	}
}

TEST_CASE("Parameter Set") {
	SECTION("Check correct setting") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                          Message::TC, 1);
		auto parameter1 = Parameter<uint8_t>(1);
		auto parameter2 = Parameter<uint16_t>(500);
		auto parameter3 = Parameter<uint32_t>(70000);

		request.appendUint8(10);
		request.appendUint16(1000);
		request.appendUint32(70001);

		parameter1.setValueFromMessage(request);
		parameter2.setValueFromMessage(request);
		parameter3.setValueFromMessage(request);

		CHECK(parameter1.getValue() == 10);
		CHECK(parameter2.getValue() == 1000);
		CHECK(parameter3.getValue() == 70001);
	}
}

TEST_CASE("Get value as double") {
	SECTION("uint8 to double") {
		auto parameter1 = Parameter<uint8_t>(7);
		uint8_t value = 13;
		parameter1.setValue(value);
		CHECK(parameter1.getValueAsDouble() == Catch::Approx(13.0).epsilon(0.1));
	}
	SECTION("uint16 to double") {
		auto parameter2 = Parameter<uint32_t>(8);
		uint16_t value = 264;
		parameter2.setValue(value);
		CHECK(parameter2.getValueAsDouble() == Catch::Approx(264.0).epsilon(0.1));
	}
	SECTION("uint32 to double") {
		auto parameter3 = Parameter<uint32_t>(9);
		uint32_t value = 544;
		parameter3.setValue(value);
		CHECK(parameter3.getValueAsDouble() == Catch::Approx(544.0).epsilon(0.1));
	}
	SECTION("float to double") {
		auto parameter4 = Parameter<float>(10);
		float value = 14.237;
		parameter4.setValue(value);
		CHECK(parameter4.getValueAsDouble() == Catch::Approx(14.237).epsilon(0.001));
	}
}
