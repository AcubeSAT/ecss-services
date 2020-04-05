#include "catch2/catch.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"

ParameterService& pserv = Services.parameterManagement;

TEST_CASE("Parameter Report Subservice") {
	SECTION("All requested parameters invalid") {
		Message request = Message(20, 1, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(54432);
		request.appendUint16(60000);
		request.appendUint16(65535);

		MessageParser::execute(request);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(1).serviceType == 1);
		CHECK(ServiceTests::get(1).messageType == 4);
		CHECK(ServiceTests::get(2).serviceType == 1);
		CHECK(ServiceTests::get(2).messageType == 4);

		Message report = ServiceTests::get(3);
		CHECK(report.serviceType == 20);
		CHECK(report.messageType == 2);
		CHECK(report.readUint16() == 0);  // the message shall be empty

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Faulty instruction handling") {
		auto param0 = Parameter<uint16_t>(1);
		auto param1 = Parameter<uint16_t>(12);
		auto param2 = Parameter<uint16_t>(3);
		systemParameters.parametersArray.push_back(param0);
		systemParameters.parametersArray.push_back(param1);
		systemParameters.parametersArray.push_back(param2);

		Message request(20, 1, Message::TC, 1);
		request.appendUint16(2); // number of requested IDs
		request.appendUint16(65535); // invalid ID in this context
		request.appendUint16(1); // valid

		MessageParser::execute(request);
		Message report = ServiceTests::get(1);

		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(0).serviceType == 1);
		// check for an ST[1,4] message caused by the faulty ID
		CHECK((ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError)));
		// check for the thrown UnknownExecutionStartError

		CHECK(report.messageType == 2);
		CHECK(report.serviceType == 20);
		// check for an ST[20,2] message (the one that contains the settings)

		CHECK(report.readUint16() == 1);  // only one parameter shall be contained

		CHECK(report.readUint16() == 1);  // check for parameter ID
		uint8_t data[ECSS_ST_20_MAX_STRING_LENGTH];
		report.readString(data, ECSS_ST_20_MAX_STRING_LENGTH);
		String<ECSS_ST_20_MAX_STRING_LENGTH> str = String<ECSS_ST_20_MAX_STRING_LENGTH>(data);
		CHECK(str.compare("12")); // check for value as string (defined when adding parameters)

		ServiceTests::reset();  // clear all errors
		Services.reset();  // reset the services
	}

	SECTION("Wrong Message Type Handling Test") {
		Message falseRequest(62, 3, Message::TM, 1); // a totally wrong message

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Parameter Setting Subservice") {
	SECTION("Faulty Instruction Handling Test") {
		auto param0 = Parameter<uint16_t>(1);
		auto param1 = Parameter<uint16_t>(12);
		auto param2 = Parameter<uint16_t>(3);
		systemParameters.parametersArray.push_back(param0);
		systemParameters.parametersArray.push_back(param1);
		systemParameters.parametersArray.push_back(param2);

		Message setRequest(20, 3, Message::TC, 1);
		setRequest.appendUint16(2); // total number of IDs
		setRequest.appendUint16(1); // correct ID in this context
		setRequest.appendUint32(3735928559); // 0xDEADBEEF in hex (new setting)
		setRequest.appendUint16(65535); // faulty ID
		setRequest.appendUint32(3131746989); // 0xBAAAAAAD (this shouldn't be found in the report)

		MessageParser::execute(setRequest);

		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::get(0).messageType == 4);

		Message reportRequest(20, 1, Message::TC, 1);
		reportRequest.appendUint16(1);
		reportRequest.appendUint16(1);  // the changed parameter has ID 1

		MessageParser::execute(reportRequest);
		Message report = ServiceTests::get(1);
		CHECK(report.serviceType == 20);
		CHECK(report.messageType == 2);
		CHECK(report.readUint16() == 1);  // only 1 ID contained
		CHECK(report.readUint16() == 1);  // contained ID should be ID 1

		char data[ECSS_ST_20_MAX_STRING_LENGTH];
		report.readString(data, ECSS_ST_20_MAX_STRING_LENGTH);
		String<ECSS_ST_20_MAX_STRING_LENGTH> str = String<ECSS_ST_20_MAX_STRING_LENGTH>(data);
		CHECK(str.compare("3735928559")); // whose value is the string 0xDEADBEEF

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Attempt to set parameter with no manual update availability") {
		auto param1 = Parameter<uint16_t>(12);
		systemParameters.parametersArray.push_back(param1);

		Message setRequest = Message(20, 3, Message::TC, 1);
		setRequest.appendUint16(1);
		setRequest.appendUint16(1);
		setRequest.appendUint32(0xBAAAAAAD);

		MessageParser::execute(setRequest);

		Message infoRequest = Message(20, 1, Message::TC, 1);
		infoRequest.appendUint16(1);
		infoRequest.appendUint16(1);

		MessageParser::execute(infoRequest);

		Message report = ServiceTests::get(0);

		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 1);
		CHECK_FALSE(report.readUint32() == 0xBAAAAAAD);

		ServiceTests::reset();
		Services.reset();
	}
}
