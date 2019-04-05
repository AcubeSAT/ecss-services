#include <catch2/catch.hpp>
#include <Services/EventActionService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <iostream>
#include <ServicePool.hpp>

EventActionService & eventActionService = Services.eventAction;

TEST_CASE("Add event-action definitions TC[19,1]", "[service][st19]") {
	EventActionService & eventActionService = Services.eventAction;

	char checkstring[256];
	Message message(19, 1, Message::TC, 0);
	message.appendEnum16(0);
	message.appendEnum16(2);
	String<64> data = "123";
	message.appendString(data);

	Service::execute(message);
	CHECK(eventActionService.eventActionDefinitionArray[0].empty == 0);
	CHECK(eventActionService.eventActionDefinitionArray[0].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionArray[0].eventDefinitionID == 2);
	CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);
	CHECK(message.readEnum16() == 0);
	CHECK(message.readEnum16() == 2);
	message.readString(checkstring, 3);
	CHECK(eventActionService.eventActionDefinitionArray[0].request.compare(data) == 0);

	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(3);
	data = "456";
	message2.appendString(data);

	Service::execute(message2);
	CHECK(eventActionService.eventActionDefinitionArray[1].empty == 0);
	CHECK(eventActionService.eventActionDefinitionArray[1].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionArray[1].eventDefinitionID == 3);
	CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

	CHECK(message2.readEnum16() == 1);
	CHECK(message2.readEnum16() == 3);
	CHECK(eventActionService.eventActionDefinitionArray[1].request.compare(data) == 0);
}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	Service::execute(message0);
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "1";
	message1.appendString(data);
	Service::execute(message1);
	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(2);
	data = "2";
	message2.appendString(data);
	Service::execute(message2);
	Message message3(19, 1, Message::TC, 0);
	message3.appendEnum16(1);
	message3.appendEnum16(3);
	data = "3";
	message3.appendString(data);
	Service::execute(message3);
	Message message4(19, 1, Message::TC, 0);
	message4.appendEnum16(1);
	message4.appendEnum16(4);
	data = "4";
	message4.appendString(data);
	Service::execute(message4);

	Message message(19, 2, Message::TC, 0);
	message.appendUint16(2);
	message.appendEnum16(1);
	message.appendEnum16(4);
	message.appendEnum16(1);
	message.appendEnum16(2);
	Service::execute(message);

	CHECK(eventActionService.eventActionDefinitionArray[0].empty == 0);
	CHECK(eventActionService.eventActionDefinitionArray[0].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionArray[0].eventDefinitionID == 0);
	CHECK(eventActionService.eventActionDefinitionArray[0].request.compare("0") == 0);
	CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);

	CHECK(eventActionService.eventActionDefinitionArray[1].empty == 0);
	CHECK(eventActionService.eventActionDefinitionArray[1].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionArray[1].eventDefinitionID == 1);
	CHECK(eventActionService.eventActionDefinitionArray[1].request.compare("1") == 0);
	CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

	CHECK(eventActionService.eventActionDefinitionArray[2].empty == 1);
	CHECK(eventActionService.eventActionDefinitionArray[2].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionArray[2].eventDefinitionID == 65535);
	CHECK(eventActionService.eventActionDefinitionArray[2].request.compare("") == 0);
	CHECK(eventActionService.eventActionDefinitionArray[2].enabled == 0);

	CHECK(eventActionService.eventActionDefinitionArray[3].empty == 0);
	CHECK(eventActionService.eventActionDefinitionArray[3].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionArray[3].eventDefinitionID == 3);
	CHECK(eventActionService.eventActionDefinitionArray[3].request.compare("3") == 0);
	CHECK(eventActionService.eventActionDefinitionArray[3].enabled == 1);

	CHECK(eventActionService.eventActionDefinitionArray[4].empty == 1);
	CHECK(eventActionService.eventActionDefinitionArray[4].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionArray[4].eventDefinitionID == 65535);
	CHECK(eventActionService.eventActionDefinitionArray[4].request.compare("") == 0);
	CHECK(eventActionService.eventActionDefinitionArray[4].enabled == 0);

}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	Service::execute(message0);
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "1";
	message1.appendString(data);
	Service::execute(message1);
	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(2);
	data = "2";
	message2.appendString(data);
	Service::execute(message2);
	Message message3(19, 1, Message::TC, 0);
	message3.appendEnum16(1);
	message3.appendEnum16(3);
	data = "3";
	message3.appendString(data);
	Service::execute(message3);
	Message message4(19, 1, Message::TC, 0);
	message4.appendEnum16(1);
	message4.appendEnum16(4);
	data = "4";
	message4.appendString(data);
	Service::execute(message4);

	Message message(19, 3, Message::TC, 0);
	eventActionService.deleteAllEventActionDefinitions(message);

	for (int i = 0; i < 256; i++){
		CHECK(eventActionService.eventActionDefinitionArray[i].empty == 1);
		CHECK(eventActionService.eventActionDefinitionArray[i].applicationId == 0);
		CHECK(eventActionService.eventActionDefinitionArray[i].eventDefinitionID == 65535);
		CHECK(eventActionService.eventActionDefinitionArray[i].request.compare("") == 0);
	}
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	Service::execute(message0);
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "00";
	message1.appendString(data);
	Service::execute(message1);
	Message message2(19, 4, Message::TC, 0);
	message2.appendUint16(2);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	Service::execute(message2);
	CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);
	CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	Service::execute(message0);
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(0);
	data = "00";
	message1.appendString(data);
	Service::execute(message1);
	Message message2(19, 5, Message::TC, 0);
	message2.appendUint16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	Service::execute(message2);
	CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 0);
}

TEST_CASE("Request event-action definition status TC[19,6]", "[service][st19]") {
	Message message(19, 6, Message::TC, 0);
	Service::execute(message);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 7);
}

TEST_CASE("Event-action status report TM[19,7]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	Service::execute(message0);
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(2);
	data = "2";
	message1.appendString(data);
	Service::execute(message1);
	Message message2(19, 5, Message::TC, 0);
	message2.appendUint16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	Service::execute(message2);
	eventActionService.eventActionStatusReport();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	CHECK(report.readUint8() == 2);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 0);
	CHECK(report.readUint8() == 0);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 2);
	CHECK(report.readUint8() == 1);
}

TEST_CASE("Enable event-action function TC[19,8]", "[service][st19]") {
	Message message(19, 8, Message::TC, 0);
	Service::execute(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == true);
}

TEST_CASE("Disable event-action function TC[19,9]", "[service][st19]") {
	Message message(19, 9, Message::TC, 0);
	Service::execute(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == false);
}

TEST_CASE("Execute a TC request", "[service][st19]"){

}
