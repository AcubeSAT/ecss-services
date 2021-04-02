#include "catch2/catch.hpp"
#include "Services/Parameter.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"

TEST_CASE("Parameter Append") {
	SECTION("Check correct appending") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues, Message::TC, 1);
		Parameter<uint8_t> parameter1 = Parameter<uint8_t>(1);
		Parameter<uint16_t> parameter2 = Parameter<uint16_t>(500);
		Parameter<uint32_t> parameter3 = Parameter<uint32_t>(70000);

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
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues, Message::TC, 1);
		Parameter<uint8_t> parameter1 = Parameter<uint8_t>(1);
		Parameter<uint16_t> parameter2 = Parameter<uint16_t>(500);
		Parameter<uint32_t> parameter3 = Parameter<uint32_t>(70000);

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
