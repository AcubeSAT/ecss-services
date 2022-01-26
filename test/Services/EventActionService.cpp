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

		REQUIRE(eventActionService.eventActionDefinitionMap.find(2)->second.applicationId == 0);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(2)->second.eventDefinitionID == 2);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(2)->second.eventActionDefinitionID == 0);
		REQUIRE(!eventActionService.eventActionDefinitionMap.find(2)->second.enabled);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(2)->second.request.compare(data) == 0);
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
			REQUIRE(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.applicationId == applicationIDs[i]);
			REQUIRE(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.eventDefinitionID == eventDefinitionIDs[i]);
			REQUIRE(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.eventActionDefinitionID == eventActionDefinitionIDs[i]);
			REQUIRE(!eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.enabled);
			REQUIRE(eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i])->second.request.compare(dataArray[i]) == 0);
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
			REQUIRE(element->second.eventDefinitionID == 2);
			REQUIRE(element->second.eventActionDefinitionID == allEventActionDefinitionIDs[actionDefinitionCounter]);
			REQUIRE(!element->second.enabled);
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

		REQUIRE(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionIDExistsError));
		REQUIRE(ServiceTests::countErrors() == 3);
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

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st19]") {
	SECTION("Delete an event-action definition") {
		// Add messages for the purpose of deleting them
		Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 7;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 0, 2};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 4, 5};
		uint16_t eventActionDefinitionIDs[] = {1, 7, 9, 65, 46, 27, 11};
		String<64> data[] = {"0", "1", "2", "3", "4", "5", "6"};
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
		message1.appendEnum16(9);
		MessageParser::execute(message1);

		Message message2(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction,
		                 Message::TC, 0);
		message2.appendUint8(2);
		message2.appendEnum16(0);
		message2.appendEnum16(4);
		message2.appendEnum16(27);
		message2.appendEnum16(2);
		message2.appendEnum16(5);
		message2.appendEnum16(11);
		MessageParser::execute(message2);

		// Checking the values after deleting some definitions

		REQUIRE(eventActionService.eventActionDefinitionMap.count(0) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(4) == 2);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(2) == 0);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(1) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(5) == 0);
	}

	SECTION("Trying to delete an enabled event-action definition") {
		// Enabling a definition to check for errors in the case of an attempt to delete it
		Message message2(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                 0);
		message2.appendUint8(1);
		message2.appendEnum16(0);
		message2.appendEnum16(1);
		message2.appendEnum16(46);
		MessageParser::execute(message2);

		Message message3(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC,
		                 0);
		message3.appendUint8(1);
		message3.appendEnum16(0);
		message3.appendEnum16(1);
		message3.appendEnum16(46);
		MessageParser::execute(message3);

		// Checking for errors in the case mentioned above
		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDeleteEnabledDefinitionError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Trying to delete an unknown definition") {
		Message message5(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC,
		                 0);
		message5.appendUint8(1);
		message5.appendEnum16(1);
		message5.appendEnum16(1);
		message5.appendEnum16(10);
		MessageParser::execute(message5);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 2);

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st19]") {

	// Adding event action definitions to delete them later
	Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	uint8_t numberOfEventActionDefinitions = 5;
	uint16_t applicationIDs[] = {1, 1, 1, 1, 1};
	uint16_t eventDefinitionIDs[] = {0, 1, 2, 3, 4};
	uint16_t eventActionDefinitionIDs[] = {0, 1, 2, 3, 4};
	String<64> data[] = {"0", "1", "2", "3", "4"};
	message0.appendUint8(numberOfEventActionDefinitions);
	for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
		message0.appendEnum16(applicationIDs[i]);
		message0.appendEnum16(eventDefinitionIDs[i]);
		message0.appendEnum16(eventActionDefinitionIDs[i]);
		message0.appendOctetString(data[i]);
	}
	MessageParser::execute(message0);

	CHECK(eventActionService.eventActionDefinitionMap.size() == 5);

	Message message(EventActionService::ServiceType, EventActionService::MessageType::DeleteAllEventAction, Message::TC, 0);
	MessageParser::execute(message);

	CHECK(eventActionService.eventActionDefinitionMap.size() == 0);
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {
	SECTION("Simple enable some event-action definitions") {
		Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 6;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729};
		String<64> data[] = {"0", "1", "2", "3", "4", "5"};
		message0.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message0.appendEnum16(applicationIDs[i]);
			message0.appendEnum16(eventDefinitionIDs[i]);
			message0.appendEnum16(eventActionDefinitionIDs[i]);
			message0.appendOctetString(data[i]);
		}
		MessageParser::execute(message0);

		// Checking their enabled status
		auto range = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(!range->second.enabled);
		REQUIRE(!std::next(range)->second.enabled);
		REQUIRE(!eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		// Creating a message to enable the previous messages
		Message message2(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                 0);
		message2.appendUint8(3);
		message2.appendEnum16(0);
		message2.appendEnum16(4);
		message2.appendEnum16(171);
		message2.appendEnum16(0);
		message2.appendEnum16(1);
		message2.appendEnum16(588);
		message2.appendEnum16(0);
		message2.appendEnum16(4);
		message2.appendEnum16(547);
		MessageParser::execute(message2);

		// Checking if the messages are enabled

		CHECK(range->second.enabled);
		CHECK(std::next(range)->second.enabled);
		CHECK(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);
	}

	SECTION("Trying to enable an unknown definition") {
		Message message3(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                 0);
		message3.appendUint8(1);
		message3.appendUint16(1);
		message3.appendEnum16(2);
		message3.appendEnum16(6);
		MessageParser::execute(message3);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 1);

//		// Checking for errors in the case of an attempt to enable an unknown definition
//		Message message4(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
//		                 0);
//		message4.appendUint8(1);
//		message4.appendEnum16(1);
//		message4.appendEnum16(1);
//		message4.appendEnum16(10);
//		MessageParser::execute(message4);
//
//		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
//		CHECK(ServiceTests::countErrors() == 2);
		eventActionService.eventActionDefinitionMap.clear();
	}

    SECTION("Enable all event action definitions") {
		Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 8;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2, 0, 1};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5, 1, 23};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729, 45, 555};
		String<64> data[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
		message0.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message0.appendEnum16(applicationIDs[i]);
			message0.appendEnum16(eventDefinitionIDs[i]);
			message0.appendEnum16(eventActionDefinitionIDs[i]);
			message0.appendOctetString(data[i]);
		}
		MessageParser::execute(message0);

		Message message3(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		message3.appendUint8(0);
		MessageParser::execute(message3);

		for(const auto& iterator : eventActionService.eventActionDefinitionMap) {
			CHECK(iterator.second.enabled);
		}

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st19]") {
	SECTION("Simple disable some event-action definitions") {
		Message message0(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 9;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2, 0, 1, 0};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5, 1, 23, 1};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729, 45, 555, 234};
		String<64> data[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
		message0.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			message0.appendEnum16(applicationIDs[i]);
			message0.appendEnum16(eventDefinitionIDs[i]);
			message0.appendEnum16(eventActionDefinitionIDs[i]);
			message0.appendOctetString(data[i]);
		}
		MessageParser::execute(message0);

		// Creating a message to enable the previous messages
		Message message2(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		message2.appendUint8(4);
		message2.appendEnum16(0);
		message2.appendEnum16(4);
		message2.appendEnum16(171);
		message2.appendEnum16(0);
		message2.appendEnum16(1);
		message2.appendEnum16(588);
		message2.appendEnum16(0);
		message2.appendEnum16(4);
		message2.appendEnum16(547);
		message2.appendEnum16(1);
		message2.appendEnum16(23);
		message2.appendEnum16(555);
		MessageParser::execute(message2);

		auto range = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(range->second.enabled);
		REQUIRE(std::next(range)->second.enabled);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		Message message3(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		message3.appendUint8(4);
		message3.appendEnum16(0);
		message3.appendEnum16(4);
		message3.appendEnum16(171);
		message3.appendEnum16(2);
		message3.appendEnum16(5);
		message3.appendEnum16(729);
		message3.appendEnum16(0);
		message3.appendEnum16(4);
		message3.appendEnum16(547);
		message3.appendEnum16(0);
		message3.appendEnum16(1);
		message3.appendEnum16(588);
		MessageParser::execute(message3);

		CHECK(!range->second.enabled);
		CHECK(!std::next(range)->second.enabled);
		CHECK(!eventActionService.eventActionDefinitionMap.find(5)->second.enabled);
		range = eventActionService.eventActionDefinitionMap.find(1);
		CHECK(!range->second.enabled);
	}

    SECTION("Disable all event action definitions") {
		Message message1(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		message1.appendUint8(0);
		MessageParser::execute(message1);

		Message message2(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		message2.appendUint8(0);
		MessageParser::execute(message2);

		for(const auto& iterator : eventActionService.eventActionDefinitionMap) {
			CHECK(!iterator.second.enabled);
		}
	}
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
	message0.appendEnum16(3);
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
	MessageParser::execute(message1);

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
	CHECK(report.readEnum16() == 3);
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
