#include "catch2/catch.hpp"
#include "Services/FunctionManagementService.hpp"

#define CATCH_CONFIG_MAIN

void test(String<MAXARGLENGTH> a) {
	std::cout << a.c_str() << std::endl;
}

TEST_CASE("FMS - Call Tests") {
	FunctionManagementService fms;

	SECTION("Malformed name") {
		fms.include(String<MAXFUNCNAMELENGTH>("test"), &test);
		Message msg(8, 1, Message::TC, 1);
		msg.appendString(String<MAXFUNCNAMELENGTH>("t3st"));
		CHECK(fms.call(msg) == 1);
	}

	/**
	 * @todo Add more tests
	 */
}
