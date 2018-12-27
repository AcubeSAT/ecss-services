#include <catch2/catch.hpp>
#include <Services/EventActionService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <cstring>

TEST_CASE("Add event-action definitions TC[19,1]", "[service][st09]") {
	EventActionService eventActionService;
	Message message(19, 1, Message::TC, 0);

}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st09]") {

}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st09]") {

}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st09]") {

}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st09]") {

}

TEST_CASE("Request event-action definition status TC[19,6]", "[service][st09]") {

}

TEST_CASE("Event-action status report TM[19,7]", "[service][st09]") {

}

TEST_CASE("Enable event-action function TC[19,8]", "[service][st09]") {

}

TEST_CASE("Disable event-action function TC[19,1]", "[service][st09]") {

}
