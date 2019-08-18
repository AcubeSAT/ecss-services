#include "catch2/catch.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"

ParameterService& pserv = Services.parameterManagement;

void foo(ValueType* bar) {  // sample function
	*bar = 0xDEADBEEF;
}

/* test ideas:
* parameter setting while flag is active
* requesting only invalid parameter IDs
*
*/

TEST_CASE("Parameter Service - General") {
	SECTION("Addition to full map") {

		Parameter param0 = Parameter(3, 14);
		Parameter param1 = Parameter(1, 7, 12);
		Parameter param2 = Parameter(4, 12, 3, nullptr);
		Parameter param3 = Parameter(12, 3, 6, &foo);
		Parameter param4 = Parameter(15, 7, 3, &foo);

		Parameter param5 = Parameter(15, 5, 4);

		pserv.addNewParameter(0, param0);
		pserv.addNewParameter(1, param1);
		pserv.addNewParameter(2, param2);
		pserv.addNewParameter(3, param3);
		pserv.addNewParameter(4, param4);

		REQUIRE_FALSE(pserv.addNewParameter(5, param5));  // addNewParameter should return false
		Services.reset();  // reset all services
	}

//	SECTION("Addition of already existing parameter") {
//
//	}

	//SECTION("Passing of null-pointer as update function on construction")

}

TEST_CASE("Parameter Report Subservice") {
//	SECTION("All requested parameters invalid") {
//
//	}

	SECTION("Faulty instruction handling") {
		Parameter param0 = Parameter(3, 14);
		Parameter param1 = Parameter(1, 7, 12);
		Parameter param2 = Parameter(4, 12, 3, nullptr);
		pserv.addNewParameter(0, param0);
		pserv.addNewParameter(1, param1);
		pserv.addNewParameter(2, param2);

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
		CHECK(report.readUint32() == 12); // check for value (defined when adding parameters)

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
		Parameter param0 = Parameter(3, 14);
		Parameter param1 = Parameter(1, 7, 12);
		Parameter param2 = Parameter(4, 12, 3, nullptr);
		pserv.addNewParameter(0, param0);
		pserv.addNewParameter(1, param1);
		pserv.addNewParameter(2, param2);

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
		CHECK(report.readUint32() == 3735928559); // whose value is 0xDEADBEEF

		ServiceTests::reset();
		Services.reset();
	}
}
