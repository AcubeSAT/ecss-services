#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/EventActionService.hpp>
#include <catch2/catch.hpp>
#include <cstring>
#include <etl/String.hpp>
#include "ServiceTests.hpp"

EventActionService& eventActionService = Services.eventAction;

TEST_CASE("Add event-action definitions TC[19,1]", "[service][st19]") {
	SECTION("Add an event-action definition to check if the values are inserted correctly") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		message.appendUint8(1);
		message.appendEnum16(0);
		message.appendEnum16(2);
		message.appendEnum16(0);
		String<64> data = "010101";
		message.appendOctetString(data);
		MessageParser::execute(message);

		CHECK(eventActionService.eventActionDefinitionMap.find(2)->second.applicationId == 0);
		CHECK(eventActionService.eventActionDefinitionMap.find(2)->second.eventDefinitionID == 2);
		CHECK(eventActionService.eventActionDefinitionMap.find(2)->second.eventActionDefinitionID == 0);
		CHECK(!eventActionService.eventActionDefinitionMap.find(2)->second.enabled);
		CHECK(eventActionService.eventActionDefinitionMap.find(2)->second.request.compare(data) == 0);
	}

	SECTION("Adding multiple event-action definitions for different events") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {3, 5, 4};
		uint16_t eventActionDefinitionIDs[] = {1, 2, 3};
		String<64> dataArray[] = {"123", "456", "789"};
		message.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message.appendEnum16(applicationIDs[i]);
			message.appendEnum16(eventDefinitionIDs[i]);
			message.appendEnum16(eventActionDefinitionIDs[i]);
			message.appendOctetString(dataArray[i]);
		}
		MessageParser::execute(message);

		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			CHECK(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.applicationId == applicationIDs[i]);
			CHECK(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.eventDefinitionID == eventDefinitionIDs[i]);
			CHECK(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.eventActionDefinitionID == eventActionDefinitionIDs[i]);
			CHECK(!eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.enabled);
			CHECK(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.request.compare(dataArray[i]) == 0);
		}
	}

	SECTION("Adding multiple event-action definitions for the same event") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {2, 2, 2};
		uint16_t eventActionDefinitionIDs[] = {7, 4, 5};
		String<64> dataArray[] = {"321", "654", "987"};
		message.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message.appendEnum16(applicationIDs[i]);
			message.appendEnum16(eventDefinitionIDs[i]);
			message.appendEnum16(eventActionDefinitionIDs[i]);
			message.appendOctetString(dataArray[i]);
		}
		MessageParser::execute(message);

		uint16_t allEventActionDefinitionIDs[] = {0, 7, 4, 5};
		auto range = eventActionService.eventActionDefinitionMap.equal_range(2);
		int actionDefinitionCounter = 0;
		for (auto& element = range.first; element != range.second; ++element, ++actionDefinitionCounter) {
			CHECK(element->second.eventDefinitionID == 2);
			CHECK(element->second.eventActionDefinitionID == allEventActionDefinitionIDs[actionDefinitionCounter]);
			CHECK(!element->second.enabled);
		}
	}

	SECTION("Add a message that is too large to check for the corresponding error") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		message.appendUint8(1);
		message.appendEnum16(0);
		message.appendEnum16(1);
		message.appendEnum16(2074);
		String<128> data = "0123456789012345678901234567890123456789012345678901234567890123456789";
		message.appendOctetString(data);

		MessageParser::execute(message);
		CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooLarge));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Add an action definition ID that already exists") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		message.appendUint8(2);
		message.appendEnum16(1);
		message.appendEnum16(3);
		message.appendEnum16(1);
		String<64> data = "123";
		message.appendOctetString(data);
		message.appendEnum16(1);
		message.appendEnum16(5);
		message.appendEnum16(1);
		data = "123";
		message.appendOctetString(data);
		MessageParser::execute(message);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionIDExistsError));
		CHECK(ServiceTests::countErrors() == 3);
	}

	SECTION("Adding the same event-action definition twice in the same message to check for error") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		message.appendUint8(2);
		message.appendEnum16(1);
		message.appendEnum16(3);
		message.appendEnum16(98);
		String<64> data = "456";
		message.appendOctetString(data);
		message.appendEnum16(1);
		message.appendEnum16(3);
		message.appendEnum16(98);
		message.appendOctetString(data);
		MessageParser::execute(message);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionIDExistsError));
		CHECK(ServiceTests::countErrors() == 4);

		ServiceTests::reset();
		Services.reset();
		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st19]") {
	SECTION("") {
		// Add messages for the purpose of deleting them
		Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {1, 1, 1};
		uint16_t eventDefinitionIDs[] = {0, 1, 2};
		uint16_t eventActionDefinitionIDs[] = {1, 7, 9};
		String<64> data[] = {"0", "1", "2"};
		message0.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message0.appendEnum16(applicationIDs[i]);
			message0.appendEnum16(eventDefinitionIDs[i]);
			message0.appendEnum16(eventActionDefinitionIDs[i]);
			message0.appendOctetString(data[i]);
		}
		MessageParser::execute(message0);

		Message message1(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction,
		                 Message::TC, 0);
		message1.appendUint8(1);
		message1.appendEnum16(1);
		message1.appendEnum16(2);
		message1.appendEnum16(1);
		MessageParser::execute(message1);

		// Checking the values after deleting some definitions

		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.applicationId == 1);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.eventDefinitionID == 0);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.request.compare("0") == 0);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 0);

		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.applicationId == 1);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.eventDefinitionID == 1);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.request.compare("1") == 0);
		CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 0);

		CHECK(eventActionService.eventActionDefinitionMap.find(2) == eventActionService.eventActionDefinitionMap.end());
	}

	SECTION("") {
		// Enabling a definition to check for errors in the case of an attempt to delete it
		Message message2(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                 0);
		message2.appendUint8(1);
		message2.appendEnum16(1);
		message2.appendEnum16(1);
		message2.appendEnum16(1);
		MessageParser::execute(message2);

		Message message3(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC,
		                 0);
		message3.appendUint8(1);
		message3.appendEnum16(1);
		message3.appendEnum16(1);
		message3.appendEnum16(1);
		MessageParser::execute(message3);

		// Checking for errors in the case mentioned above
		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDeleteEnabledDefinitionError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("") {
		// Checking for errors in the case of an unknown definition
		Message message4(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC,
		                 0);
		message4.appendUint8(1);
		message4.appendEnum16(1);
		message4.appendEnum16(10);
		message4.appendEnum16(1);
		MessageParser::execute(message4);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 2);

		Message message5(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC,
		                 0);
		message5.appendUint8(1);
		message5.appendEnum16(1);
		message5.appendEnum16(1);
		message5.appendEnum16(10);
		MessageParser::execute(message5);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 3);
	}
}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st19]") {

	// Adding event action definitions to delete them later
	Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	uint8_t numberOfEventActionDefinitions = 5;
	uint16_t applicationIDs[] = {1, 1, 1, 1, 1};
	uint16_t eventDefinitionIDs[] = {0, 1, 2, 3, 4};
	uint16_t eventActionDefinitionIDs[] = {1, 1, 1, 1, 1};
	String<64> data[] = {"0", "1", "2", "3", "4"};
	message0.appendUint8(numberOfEventActionDefinitions);
	for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
		message0.appendEnum16(applicationIDs[i]);
		message0.appendEnum16(eventDefinitionIDs[i]);
		message0.appendEnum16(eventActionDefinitionIDs[i]);
		message0.appendOctetString(data[i]);
	}
	MessageParser::execute(message0);

	Message message(EventActionService::ServiceType, EventActionService::MessageType::DeleteAllEventAction, Message::TC, 0);
	MessageParser::execute(message);

	// Checking the content of the map
	for (int i = 0; i < 256; i++) {
		CHECK(eventActionService.eventActionDefinitionMap.find(i) == eventActionService.eventActionDefinitionMap.end());
	}
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {

	// Adding event action definitions to enable them
	Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	message0.appendUint8(2);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	message0.appendEnum16(1);
	String<64> data = "0";
	message0.appendOctetString(data);
	message0.appendEnum16(1);
	message0.appendEnum16(1);
	message0.appendEnum16(1);
	data = "00";
	message0.appendOctetString(data);
	MessageParser::execute(message0);

	// Checking their enabled status
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 0);

	// Creating a message to enable the previous messages
	Message message2(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	message2.appendUint8(2);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	MessageParser::execute(message2);

	// Checking if the messages are enabled
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 1);
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 1);

	// Checking for errors in the case of an attempt to enable an unknown definition
	Message message3(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	message3.appendUint8(1);
	message3.appendUint16(1);
	message3.appendEnum16(2);
	message3.appendEnum16(6);
	MessageParser::execute(message3);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
	CHECK(ServiceTests::countErrors() == 1);

	// Checking for errors in the case of an attempt to enable an unknown definition
	Message message4(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	message4.appendUint8(1);
	message4.appendEnum16(1);
	message4.appendEnum16(1);
	message4.appendEnum16(10);
	MessageParser::execute(message4);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
	CHECK(ServiceTests::countErrors() == 2);
}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st19]") {

	// Adding event action definitions to enable them
	Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	message0.appendUint8(2);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	message0.appendEnum16(1);
	String<64> data = "0";
	message0.appendOctetString(data);
	message0.appendEnum16(1);
	message0.appendEnum16(1);
	message0.appendEnum16(1);
	data = "00";
	message0.appendOctetString(data);
	MessageParser::execute(message0);

	// Checking their enabled status
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 0);

	// Creating a message to enable the previous messages
	Message message1(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	message1.appendUint16(2);
	message1.appendEnum16(1);
	message1.appendEnum16(0);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	message1.appendEnum16(1);
	MessageParser::execute(message1);

	// Checking if the messages are enabled
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 1);
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 1);

	// Creating a message to enable the previous messages
	Message message2(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction,
	                 Message::TC, 0);
	message2.appendUint16(2);
	message2.appendEnum16(1);
	message2.appendEnum16(0);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	message2.appendEnum16(1);
	MessageParser::execute(message2);

	// Checking if the messages are enabled
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(0)->second.enabled == 0);
	CHECK(eventActionService.eventActionDefinitionMap.lower_bound(1)->second.enabled == 0);

	// Checking for errors in the case of an attempt to disable an unknown definition
	Message message3(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction,
	                 Message::TC, 0);
	message3.appendUint8(1);
	message3.appendEnum16(1);
	message3.appendEnum16(2);
	message3.appendEnum16(6);
	MessageParser::execute(message3);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
	CHECK(ServiceTests::countErrors() == 1);

	// Checking for errors in the case of an attempt to enable an unknown definition
	Message message4(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction,
	                 Message::TC, 0);
	message4.appendUint8(1);
	message4.appendEnum16(1);
	message4.appendEnum16(1);
	message4.appendEnum16(10);
	MessageParser::execute(message4);

	CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
	CHECK(ServiceTests::countErrors() == 2);
}

TEST_CASE("Request event-action definition status TC[19,6]", "[service][st19]") {

	// Creating a request for a report on the event action definitions
	Message message(EventActionService::ServiceType, EventActionService::MessageType::ReportStatusOfEachEventAction, Message::TC, 0);
	MessageParser::execute(message);
	REQUIRE(ServiceTests::hasOneMessage());

	// Checking that the report was made
	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 7);
}

TEST_CASE("Event-action status report TM[19,7]", "[service][st19]") {

	// Adding event-action definitions to report them
	Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	message0.appendUint8(2);
	message0.appendEnum16(1);
	message0.appendEnum16(0);
	message0.appendEnum16(1);
	String<64> data = "0";
	message0.appendOctetString(data);
	message0.appendEnum16(1);
	message0.appendEnum16(2);
	message0.appendEnum16(1);
	data = "2";
	message0.appendOctetString(data);
	MessageParser::execute(message0);

	// Enablilng one of the two
	Message message1(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	message1.appendUint8(1);
	message1.appendEnum16(1);
	message1.appendEnum16(0);
	message1.appendEnum16(1);
	eventActionService.enableEventActionDefinitions(message1);

	eventActionService.eventActionStatusReport();
	REQUIRE(ServiceTests::hasOneMessage());

	// Checking the contents of the report
	Message report = ServiceTests::get(0);
	CHECK(report.readUint16() == 2);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 0);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readBoolean() == 1);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 2);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readBoolean() == 0);
}

TEST_CASE("Enable event-action function TC[19,8]", "[service][st19]") {

	// A message to enable event action function

	Message message(EventActionService::ServiceType, EventActionService::MessageType::EnableEventActionFunction, Message::TC, 0);
	eventActionService.enableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus());
}

TEST_CASE("Disable event-action function TC[19,9]", "[service][st19]") {

	// A message to disable event action function
	Message message(EventActionService::ServiceType, EventActionService::MessageType::DisableEventActionFunction, Message::TC, 0);
	eventActionService.disableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == false);
}

TEST_CASE("Execute a TC request", "[service][st19]") {
}
