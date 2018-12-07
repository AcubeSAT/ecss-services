#include "catch2/catch.hpp"
#include "Helpers/TimeHelper.hpp"

TEST_CASE("Time format implementation", "[CUC]") {
	// a very simple test for the TimeHelper. Check the P-Field and the T-Field
	Message receive = Message(9, 2, Message::TC, 1); // random values
	TimeHelper::implementCUCTimeFormat(60, receive);

	// the P-Field as defined in the implementCUCTimeFormat()
	CHECK(receive.readBits(1) == 0);
	CHECK(receive.readBits(3) == 1);
	CHECK(receive.readBits(2) == 3);
	CHECK(receive.readBits(2) == 0);

	// the T-Field as defined in the implementCUCTimeFormat()
	CHECK(receive.readWord() == 60);

}
