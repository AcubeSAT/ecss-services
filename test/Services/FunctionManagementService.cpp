#include "catch2/catch.hpp"
#include "Services/FunctionManagementService.hpp"
#include "ServicePool.hpp"
#include "ServiceTests.hpp"
#include <iostream>

FunctionManagementService& fms = Services.functionManagement;

uint8_t globalVariable = 10;

void test(String<MAX_ARG_LENGTH> a) {
	globalVariable = a[0];
}

TEST_CASE("ST[08] - Call Tests") {
	SECTION("Function call") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);

		msg.appendFixedString(String<FUNC_NAME_LENGTH>("test"));
		msg.appendByte(199);
		MessageParser::execute(msg);

		CHECK(ServiceTests::hasNoErrors());
		CHECK(globalVariable == 199);
	}

	SECTION("Malformed name") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendFixedString(String<FUNC_NAME_LENGTH>("t3st"));
		MessageParser::execute(msg);

		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(globalVariable == 10);
	}

	SECTION("Too long message") {
		ServiceTests::reset();
		globalVariable = 10;

		fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendFixedString(String<FUNC_NAME_LENGTH>("test"));
		msg.appendString(String<65>
		    ("eqrhjweghjhwqgthjkrghthjkdsfhgsdfhjsdjsfdhgkjdfsghfjdgkdfsgdfgsgd"));
		MessageParser::execute(msg);

		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(0).serviceType == 1);
		CHECK(ServiceTests::countErrors() == 2);
		CHECK(globalVariable == 10);
	}
}

TEST_CASE("ST[08] - Insert Tests") {
	SECTION("Insertion to full pointer map") {
		// make sure the pointer map is full to the brim
		ServiceTests::reset();
		std::string name = "test"; // FOR TESTING ONLY!

		for (int i = 0; i < FUNC_MAP_SIZE + 1; i++) {
			name += std::to_string(i); // different names to fill up the map
			fms.include(String<FUNC_NAME_LENGTH>(name.c_str()), &test);
		}
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::FunctionMapFull));
	}
}
