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
		std::string name = ""; // FOR TESTING ONLY!

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

		message.appendFixedString(String<ECSSFunctionNameLength>("st08FunctionTest"));
		message.appendHalfword(400);
		message.appendByte(8);

		MessageParser::execute(message);
		CHECK(static_cast<Parameter<uint16_t>&>(ps.getParameter(34)->get()).getValue() == 400);
		CHECK(static_cast<Parameter<uint8_t>&>(ps.getParameter(35)->get()).getValue() == 8);
	}
}
