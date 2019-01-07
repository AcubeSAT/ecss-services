#include "catch2/catch.hpp"
#include "Services/FunctionManagementService.hpp"

#define CATCH_CONFIG_MAIN

void test(String<MAXARGLENGTH> a) {
	std::cout << a.c_str() << std::endl;
}

TEST_CASE("FMS - Call Tests") {
	FunctionManagementService fms;

	SECTION("Malformed name") {
		fms.include(String<FUNCNAMELENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendString(String<FUNCNAMELENGTH>("t3st"));
		CHECK(fms.call(msg) == 1);
	}

	SECTION("Too long message") {
		fms.include(String<FUNCNAMELENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendString(String<FUNCNAMELENGTH>("test"));
		msg.appendString(String<65>
		    ("eqrhjweghjhwqgthjkrghthjkdsfhgsdfhjsdjsfdhgkjdfsghfjdgkdfsgdfgsgd"));
		CHECK(fms.call(msg) == 4);
	}
}

TEST_CASE("FMS - Insert Tests") {

	SECTION("Insertion to full pointer map") {
		FunctionManagementService fms;
		std::string name;  // FOR TESTING ONLY!

		// make sure the pointer map is full to the brim
		for (int i = 0; i < 15000; i++) {
			name = "test" + i;
			String<FUNCNAMELENGTH> funcName(name.c_str());

			if (~fms.funcPtrIndex.full()) {  // not ! because vera whines about "using negation
				// in its short form"
				fms.include(funcName, &test);
			}
			else {
				break;
			}
		}

		CHECK(fms.include(String<FUNCNAMELENGTH>("testall"), &test) == 2);
	}
}

/**
 * @todo Add more tests
 */
