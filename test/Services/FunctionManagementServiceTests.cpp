#include "Services/FunctionManagementService.hpp"
#include "ServicePool.hpp"
#include "ServiceTests.hpp"
#include "Services/RequestVerificationService.hpp"
#include "catch2/catch_all.hpp"
#include "Services/ParameterService.hpp"
#include "Parameters/PlatformParameters.hpp"

FunctionManagementService& fms = Services.functionManagement;
ParameterService& ps = Services.parameterManagement;

uint8_t globalVariable = 10;

void test(String<ECSSFunctionMaxArgLength> a) {
	globalVariable = a[0];
}
TEST_CASE("ST[08] - Call Tests") {
	SECTION("Function call") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<ECSSFunctionNameLength>("test"), &test);
		Message msg(FunctionManagementService::ServiceType, FunctionManagementService::MessageType::PerformFunction,
		            Message::TC, 1);

		msg.appendFixedString(String<ECSSFunctionNameLength>("test"));
		msg.appendByte(199);
		MessageParser::execute(msg);

		CHECK(ServiceTests::hasNoErrors());
		CHECK(globalVariable == 199);
	}

	SECTION("Malformed name") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<ECSSFunctionNameLength>("test"), &test);
		Message msg(FunctionManagementService::ServiceType, FunctionManagementService::MessageType::PerformFunction, Message::TC, 1);
		msg.appendFixedString(String<ECSSFunctionNameLength>("t3st"));
		MessageParser::execute(msg);

		CHECK(ServiceTests::get(0).messageType == RequestVerificationService::MessageType::FailedStartOfExecution);
		CHECK(ServiceTests::get(0).serviceType == RequestVerificationService::ServiceType);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(globalVariable == 10);
	}

	SECTION("Too long message") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<ECSSFunctionNameLength>("test"), &test);
		Message msg(FunctionManagementService::ServiceType, FunctionManagementService::MessageType::PerformFunction, Message::TC, 1);
		msg.appendFixedString(String<ECSSFunctionNameLength>("test"));
		msg.appendString(String<65>("eqrhjweghjhwqgthjkrghthjkdsfhgsdfhjsdjsfdhgkjdfsghfjdgkdfsgdfgsgd"));
		MessageParser::execute(msg);

		CHECK(ServiceTests::get(0).messageType == RequestVerificationService::MessageType::FailedStartOfExecution);
		CHECK(ServiceTests::get(0).serviceType == RequestVerificationService::ServiceType);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(globalVariable == 10);
	}
}

TEST_CASE("ST[08] - Insert Tests") {
	SECTION("Insertion to full pointer map") {
		// make sure the pointer map is full to the brim
		ServiceTests::reset();
		std::string name = "01"; // FOR TESTING ONLY!  "01" -> "test"

		for (int i = 0; i < ECSSFunctionMapSize + 1; i++) {
			name += std::to_string(i); // different names to fill up the map
			fms.include(String<ECSSFunctionNameLength>(name.c_str()), &test);
		}
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::MapFull));
	}
}

TEST_CASE("ST[08] - Check preinitialized function map") {
	SECTION("Check if the function map is preinitialized") {
		ServiceTests::reset();

		CHECK(fms.getMapSize() == 1);
	}

	SECTION("Check if the preinitialized functions in the function map can run") {
		ServiceTests::reset();

		Message message(FunctionManagementService::ServiceType, FunctionManagementService::MessageType::PerformFunction,
		            Message::TC, 1);

		message.appendFixedString(String<ECSSFunctionNameLength>("Test"));
		message.appendHalfword(400);
		message.appendByte(109);

		MessageParser::execute(message);
		CHECK(static_cast<Parameter<uint16_t>&>(ps.getParameter(7)->get()).getValue() == 400);

		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(0)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(1)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(2)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(3)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(4)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(5)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(6)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(7)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(8)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(9)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(10)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(11)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(12)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(13)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(14)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(15)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(16)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(17)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(18)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(19)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(20)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(21)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(22)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(23)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(24)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(25)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(26)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(27)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(28)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(29)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(30)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(31)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(32)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(33)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(34)->get()).getValue() == 8);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(35)->get()).getValue() == 8);

	}
}
