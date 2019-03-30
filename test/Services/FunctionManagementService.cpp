#include "catch2/catch.hpp"
#include "Services/FunctionManagementService.hpp"
#include "ServicePool.hpp"
#include "ServiceTests.hpp"
#include <iostream>

FunctionManagementService & fms = Services.functionManagement;

void test(String<MAX_ARG_LENGTH> a) {
	std::cout << a.c_str() << std::endl;
}

TEST_CASE("ST[08] - Call Tests") {

	SECTION("Malformed name") {
		ServiceTests::reset();
		fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendString(String<FUNC_NAME_LENGTH>("t3st"));
		fms.call(msg);
		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(0).serviceType == 1);
	}

	SECTION("Too long message") {
		ServiceTests::reset();
		fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendString(String<FUNC_NAME_LENGTH>("test"));
		msg.appendString(String<65>
		    ("eqrhjweghjhwqgthjkrghthjkdsfhgsdfhjsdjsfdhgkjdfsghfjdgkdfsgdfgsgd"));
		fms.call(msg);
		CHECK(ServiceTests::get(0).messageType == 4);
		CHECK(ServiceTests::get(0).serviceType == 1);
	}
}

// WARNING! include() is malfunctioning - do not merge!

//TEST_CASE("ST[08] - Insert Tests") {
//
//	SECTION("Insertion to full pointer map") {
//		// make sure the pointer map is full to the brim
//		ServiceTests::reset();
//		std::string name = "test";  // FOR TESTING ONLY!
//
//	}
//}
