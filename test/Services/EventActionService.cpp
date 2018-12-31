#include <catch2/catch.hpp>
#include <Services/EventActionService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <iostream>
TEST_CASE("Add event-action definitions TC[19,1]", "[service][st09]") {
	EventActionService eventActionService;
	char checkstring[256];
	Message message(19, 1, Message::TC, 0);
	message.appendUint16(2);
	message.appendEnum16(0);
	message.appendEnum16(2);
	String<64> data = "123";
	message.appendString(data);

	message.appendEnum16(1);
	message.appendEnum16(3);
	data = "456";
	message.appendString(data);
	// Should I check byte by byte or check the contents of the EventActionDefinitionArray
	eventActionService.addEventActionDefinitions(message);
	CHECK(message.readUint16() == 2);
	CHECK(eventActionService.eventActionDefinitionArray[0].empty == 0);
	message.readEnum16();
	CHECK(message.readEnum16() == 2);
	message.readString(checkstring, 3);
	CHECK(eventActionService.eventActionDefinitionArray[0].request.compare(checkstring) == 0);
	CHECK(message.readEnum16() == 1);
	CHECK(message.readEnum16() == 3);
	message.readString(checkstring, 3);
	std::cout << checkstring;
	CHECK(eventActionService.eventActionDefinitionArray[1].request.compare(checkstring) == 0);
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
