#include <catch2/catch.hpp>
#include <Services/EventActionService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <etl/map.h>
#include <cstring>
#include <iostream>
#include <ServicePool.hpp>

EventActionService & eventActionService = Services.eventAction;

TEST_CASE("Add event-action definitions TC[19,1]", "[service][st19]") {

	// Add a message that is too large to check for the corresponding error
	Message message(19, 1, Message::TC, 0);
	message.appendEnum16(0);
	message.appendEnum16(5);
	String<128> data = "0123456789012345678901234567890123456789012345678901234567890123456789";
	message.appendString(data);

	eventActionService.addEventActionDefinitions(message);
	CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooLarge));
	CHECK(ServiceTests::countErrors() == 1);

	// Add an event-action definition to check if the values are inserted correctly
	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(0);
	message1.appendEnum16(2);
	data = "01234";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);
	CHECK(eventActionService.eventActionDefinitionMap[2].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionMap[2].eventDefinitionID == 2);
	CHECK(eventActionService.eventActionDefinitionMap[2].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap[2].request.compare(data) == 0);

	// Add a second event-action definition
	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(3);
	data = "456";
	message2.appendString(data);
	eventActionService.addEventActionDefinitions(message2);
	CHECK(eventActionService.eventActionDefinitionMap[3].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionMap[3].eventDefinitionID == 3);
	CHECK(eventActionService.eventActionDefinitionMap[3].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap[3].request.compare(data) == 0);

	// Enable an event-action definition for the purposes of adding an event-action definition
	// that exists already and is enabled to check for the corresponding error.
	Message message4(19, 4, Message::TC, 0);
	message4.appendUint16(1);
	message4.appendEnum16(1);
	message4.appendEnum16(3);
	eventActionService.enableEventActionDefinitions(message4);

	// Add an already existing event-action definition
	Message message3(19, 1, Message::TC, 0);
	message3.appendEnum16(1);
	message3.appendEnum16(3);
	data = "789";
	message3.appendString(data);
	eventActionService.addEventActionDefinitions(message3);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionAddEnabledDefinitionError));
	CHECK(ServiceTests::countErrors() == 2);

	// Disable an event-action definition for the purposes of adding an event-action definition
	// that exists already and is disabled to check for any errors that shoudln't be thrown.
	Message message5(19, 5, Message::TC, 0);
	message5.appendUint16(1);
	message5.appendEnum16(1);
	message5.appendEnum16(3);
	eventActionService.disableEventActionDefinitions(message5);

	Message message6(19, 1, Message::TC, 0);
	message6.appendEnum16(1);
	message6.appendEnum16(3);
	data = "789";
	message6.appendString(data);
	eventActionService.addEventActionDefinitions(message6);

	CHECK(eventActionService.eventActionDefinitionMap[3].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionMap[3].eventDefinitionID == 3);
	CHECK(eventActionService.eventActionDefinitionMap[3].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap[3].request.compare(data) == 0);
}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	eventActionService.addEventActionDefinitions(message0);

	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "1";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);

	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(2);
	data = "2";
	message2.appendString(data);
	eventActionService.addEventActionDefinitions(message2);

	Message message3(19, 1, Message::TC, 0);
	message3.appendEnum16(1);
	message3.appendEnum16(3);
	data = "3";
	message3.appendString(data);
	eventActionService.addEventActionDefinitions(message3);

	Message message4(19, 1, Message::TC, 0);
	message4.appendEnum16(1);
	message4.appendEnum16(4);
	data = "4";
	message4.appendString(data);
	eventActionService.addEventActionDefinitions(message4);

	Message message(19, 5, Message::TC, 0);
	message.appendUint16(2);
	message.appendEnum16(1);
	message.appendEnum16(4);
	message.appendEnum16(1);
	message.appendEnum16(2);
	eventActionService.disableEventActionDefinitions(message);

	Message message5(19, 2, Message::TC, 0);
	message5.appendUint16(2);
	message5.appendEnum16(1);
	message5.appendEnum16(4);
	message5.appendEnum16(1);
	message5.appendEnum16(2);
	eventActionService.deleteEventActionDefinitions(message5);

	CHECK(eventActionService.eventActionDefinitionMap[0].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionMap[0].eventDefinitionID == 0);
	CHECK(eventActionService.eventActionDefinitionMap[0].request.compare("0") == 0);
	CHECK(eventActionService.eventActionDefinitionMap[0].enabled == 0);

	CHECK(eventActionService.eventActionDefinitionMap[1].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionMap[1].eventDefinitionID == 1);
	CHECK(eventActionService.eventActionDefinitionMap[1].request.compare("1") == 0);
	CHECK(eventActionService.eventActionDefinitionMap[1].enabled == 0);

	CHECK(eventActionService.eventActionDefinitionMap[2].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionMap[2].eventDefinitionID == 65535);
	CHECK(eventActionService.eventActionDefinitionMap[2].request.compare("") == 0);
	CHECK(eventActionService.eventActionDefinitionMap[2].enabled == 0);

	CHECK(eventActionService.eventActionDefinitionMap[3].applicationId == 1);
	CHECK(eventActionService.eventActionDefinitionMap[3].eventDefinitionID == 3);
	CHECK(eventActionService.eventActionDefinitionMap[3].request.compare("3") == 0);
	CHECK(eventActionService.eventActionDefinitionMap[3].enabled == 0);

	CHECK(eventActionService.eventActionDefinitionMap[4].applicationId == 0);
	CHECK(eventActionService.eventActionDefinitionMap[4].eventDefinitionID == 65535);
	CHECK(eventActionService.eventActionDefinitionMap[4].request.compare("") == 0);
	CHECK(eventActionService.eventActionDefinitionMap[4].enabled == 0);

	Message message8(19, 4, Message::TC, 0);
	message8.appendUint16(1);
	message8.appendEnum16(1);
	message8.appendEnum16(3);
	eventActionService.enableEventActionDefinitions(message8);

	Message message6(19, 2, Message::TC, 0);
	message6.appendUint16(1);
	message6.appendEnum16(1);
	message6.appendEnum16(3);
	eventActionService.deleteEventActionDefinitions(message6);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDeleteEnabledDefinitionError));
	CHECK(ServiceTests::countErrors() == 1);

	Message message7(19, 2, Message::TC, 0);
	message7.appendUint16(1);
	message7.appendEnum16(1);
	message7.appendEnum16(10);
	eventActionService.deleteEventActionDefinitions(message7);
	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownDefinitionError));
	CHECK(ServiceTests::countErrors() == 2);
}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	eventActionService.addEventActionDefinitions(message0);

	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "1";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);

	Message message2(19, 1, Message::TC, 0);
	message2.appendEnum16(1);
	message2.appendEnum16(2);
	data = "2";
	message2.appendString(data);
	eventActionService.addEventActionDefinitions(message2);

	Message message3(19, 1, Message::TC, 0);
	message3.appendEnum16(1);
	message3.appendEnum16(3);
	data = "3";
	message3.appendString(data);
	eventActionService.addEventActionDefinitions(message3);

	Message message4(19, 1, Message::TC, 0);
	message4.appendEnum16(1);
	message4.appendEnum16(4);
	data = "4";
	message4.appendString(data);
	eventActionService.addEventActionDefinitions(message4);

	Message message(19, 3, Message::TC, 0);
	eventActionService.deleteAllEventActionDefinitions(message);

	for (int i = 0; i < 256; i++){
		CHECK(eventActionService.eventActionDefinitionMap[i].applicationId == 0);
		CHECK(eventActionService.eventActionDefinitionMap[i].eventDefinitionID == 65535);
		CHECK(eventActionService.eventActionDefinitionMap[i].request.compare("") == 0);
	}
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {

	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	eventActionService.addEventActionDefinitions(message0);

	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	data = "00";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);

	Message message2(19, 5, Message::TC, 0);
	message2.appendUint16(2);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	eventActionService.disableEventActionDefinitions(message2);

	CHECK(eventActionService.eventActionDefinitionMap[0].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap[1].enabled == 0);
	Message message3(19, 4, Message::TC, 0);
	message3.appendUint16(2);
	message3.appendEnum16(1);
	message3.appendEnum16(0);
	message3.appendEnum16(1);
	message3.appendEnum16(1);
	eventActionService.enableEventActionDefinitions(message3);

	CHECK(eventActionService.eventActionDefinitionMap[0].enabled == 1);
	CHECK(eventActionService.eventActionDefinitionMap[1].enabled == 1);

	Message message7(19, 2, Message::TC, 0);
	message7.appendUint16(1);
	message7.appendEnum16(1);
	message7.appendEnum16(10);
	eventActionService.enableEventActionDefinitions(message7);
	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownDefinitionError));

	CHECK(ServiceTests::countErrors() == 1);
}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st19]") {
	Message message0(19, 1, Message::TC, 0);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	String<64> data = "0";
	message0.appendString(data);
	eventActionService.addEventActionDefinitions(message0);

	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(0);
	data = "00";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);

	Message message2(19, 5, Message::TC, 0);
	message2.appendUint16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	eventActionService.disableEventActionDefinitions(message2);

	CHECK(eventActionService.eventActionDefinitionMap[0].enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap[1].enabled == 0);

	Message message7(19, 2, Message::TC, 0);
	message7.appendUint16(1);
	message7.appendEnum16(1);
	message7.appendEnum16(10);
	eventActionService.enableEventActionDefinitions(message7);
	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownDefinitionError));
	CHECK(ServiceTests::countErrors() == 1);
}

TEST_CASE("Request event-action definition status TC[19,6]", "[service][st19]") {
	Message message(19, 6, Message::TC, 0);
	eventActionService.requestEventActionDefinitionStatus(message);
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
	eventActionService.addEventActionDefinitions(message0);

	Message message1(19, 1, Message::TC, 0);
	message1.appendEnum16(1);
	message1.appendEnum16(2);
	data = "2";
	message1.appendString(data);
	eventActionService.addEventActionDefinitions(message1);

	Message message2(19, 5, Message::TC, 0);
	message2.appendUint16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	eventActionService.enableEventActionDefinitions(message2);

	eventActionService.eventActionStatusReport();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	CHECK(report.readUint8() == 2);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 0);
	CHECK(report.readUint8() == 1);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 2);
	CHECK(report.readUint8() == 0);
}

TEST_CASE("Enable event-action function TC[19,8]", "[service][st19]") {
	Message message(19, 8, Message::TC, 0);
	eventActionService.enableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == true);
}

TEST_CASE("Disable event-action function TC[19,9]", "[service][st19]") {
	Message message(19, 9, Message::TC, 0);
	eventActionService.disableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == false);
}

TEST_CASE("Execute a TC request", "[service][st19]"){

}
