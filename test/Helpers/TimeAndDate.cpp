#include "catch2/catch.hpp"
#include "Helpers/TimeAndDate.hpp"
#include "../Services/ServiceTests.hpp"

TEST_CASE("Date comparison", "[operands]") {
	SECTION("Invalid date") {
		TimeAndDate InvalidDate0(1900, 2, 2, 4, 5, 6); // error in year
		TimeAndDate InvalidDate1(2030, 70, 2, 4, 5, 6); // error in month
		TimeAndDate InvalidDate2(2030, 2, 73, 4, 5, 6); // error in day
		TimeAndDate InvalidDate3(2030, 2, 2, 74, 5, 6); // error in hour
		TimeAndDate InvalidDate4(2030, 2, 2, 4, 75, 6); // error in minute
		TimeAndDate InvalidDate5(2030, 2, 2, 4, 5, 76); // error in seconds

		CHECK(ServiceTests::countErrors() == 6);
		CHECK(ServiceTests::thrownError(ErrorHandler::InvalidDate));
	}

	SECTION("Different year") {
		TimeAndDate Now;
		// 10/04/2021 10:15:00
		Now.year = 2021;
		Now.month = 4;
		Now.day = 10;
		Now.hour = 10;
		Now.minute = 15;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Different month") {
		TimeAndDate Now;
		// 10/05/2020 10:15:00
		Now.year = 2020;
		Now.month = 5;
		Now.day = 10;
		Now.hour = 10;
		Now.minute = 15;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Different day") {
		TimeAndDate Now;
		// 11/04/2020 10:15:00
		Now.year = 2020;
		Now.month = 5;
		Now.day = 11;
		Now.hour = 10;
		Now.minute = 15;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Different hour") {
		TimeAndDate Now;
		// 10/04/2020 11:15:00
		Now.year = 2020;
		Now.month = 4;
		Now.day = 10;
		Now.hour = 11;
		Now.minute = 15;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Different minute") {
		TimeAndDate Now;
		// 10/04/2020 10:16:00
		Now.year = 2020;
		Now.month = 4;
		Now.day = 10;
		Now.hour = 10;
		Now.minute = 16;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Different second") {
		TimeAndDate Now;
		// 10/04/2020 10:15:01
		Now.year = 2020;
		Now.month = 4;
		Now.day = 10;
		Now.hour = 10;
		Now.minute = 15;
		Now.second = 1;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now < Date) == false);
		CHECK((Now > Date) == true);
		CHECK((Now > Date) == true);
		CHECK((Now < Date) == false);

		CHECK((Now <= Date) == false);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == false);

		CHECK((Now == Date) == false);
	}

	SECTION("Same date") {
		TimeAndDate Now;
		// 10/04/2020 10:15:01
		Now.year = 2020;
		Now.month = 4;
		Now.day = 10;
		Now.hour = 10;
		Now.minute = 15;
		Now.second = 0;

		TimeAndDate Date;
		// 10/04/2020 10:15:00
		Date.year = 2020;
		Date.month = 4;
		Date.day = 10;
		Date.hour = 10;
		Date.minute = 15;
		Date.second = 0;

		CHECK((Now == Date) == true);
		CHECK((Now <= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now >= Date) == true);
		CHECK((Now <= Date) == true);
	}
}
