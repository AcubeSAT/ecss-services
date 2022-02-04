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
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(2);
		addDefinition.appendEnum16(0);
		String<CCSDSMaxMessageSize> data = "12345";
		addDefinition.appendOctetString(data);
		MessageParser::execute(addDefinition);

		auto element = eventActionService.eventActionDefinitionMap.find(2);
		CHECK(element->second.applicationId == 0);
		CHECK(element->second.eventDefinitionID == 2);
		CHECK(element->second.eventActionDefinitionID == 0);
		CHECK(!element->second.enabled);
		CHECK(element->second.request.compare(data) == 0);
	}

	SECTION("Adding multiple event-action definitions for different events") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {3, 5, 4};
		uint16_t eventActionDefinitionIDs[] = {1, 2, 3};
		String<CCSDSMaxMessageSize> dataArray[] = {"123", "456", "789"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(dataArray[i]);
		}
		MessageParser::execute(addDefinitions);

		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			auto element = eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i]);
			REQUIRE(element->second.applicationId == applicationIDs[i]);
			REQUIRE(element->second.eventDefinitionID == eventDefinitionIDs[i]);
			REQUIRE(element->second.eventActionDefinitionID == eventActionDefinitionIDs[i]);
			REQUIRE(!element->second.enabled);
			REQUIRE(element->second.request.compare(dataArray[i]) == 0);
		}
	}

	SECTION("Adding multiple event-action definitions for the same event") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {2, 2, 2};
		uint16_t eventActionDefinitionIDs[] = {7, 4, 5};
		String<CCSDSMaxMessageSize> dataArray[] = {"321", "654", "987"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(dataArray[i]);
		}
		MessageParser::execute(addDefinitions);

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
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(1);
		addDefinition.appendEnum16(2074);
		String<CCSDSMaxMessageSize> data = "";
		data.append(120, 'a');
		addDefinition.appendOctetString(data);

		MessageParser::execute(addDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooLarge));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Add an action definition ID that already exists") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(2);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(3);
		addDefinitions.appendEnum16(1);
		String<CCSDSMaxMessageSize> data = "123";
		addDefinitions.appendOctetString(data);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(5);
		addDefinitions.appendEnum16(1);
		data = "123";
		addDefinitions.appendOctetString(data);
		MessageParser::execute(addDefinitions);

		REQUIRE(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionIDExistsError));
		REQUIRE(ServiceTests::countErrors() == 3);

		ServiceTests::reset();
	}

	SECTION("Adding the same event-action definition twice in the same message to check for error") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(2);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(3);
		addDefinitions.appendEnum16(98);
		String<CCSDSMaxMessageSize> data = "456";
		addDefinitions.appendOctetString(data);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(3);
		addDefinitions.appendEnum16(98);
		addDefinitions.appendOctetString(data);
		MessageParser::execute(addDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionIDExistsError));
		CHECK(ServiceTests::countErrors() == 1);

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {
	SECTION("Simple enable some event-action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 6;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729};
		String<CCSDSMaxMessageSize> data[] = {"0", "1", "2", "3", "4", "5"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		auto range = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(!range->second.enabled);
		REQUIRE(!std::next(range)->second.enabled);
		REQUIRE(!eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                          0);
		enableDefinitions.appendUint8(3);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(171);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(588);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(547);
		MessageParser::execute(enableDefinitions);

		CHECK(range->second.enabled);
		CHECK(std::next(range)->second.enabled);
		CHECK(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);
	}

	SECTION("Trying to enable an unknown definition") {
		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
		                         0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendUint16(1);
		enableDefinition.appendEnum16(2);
		enableDefinition.appendEnum16(6);
		MessageParser::execute(enableDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Trying to enable an existing event-action definition with the wrong event definition ID") {
		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(2);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(724);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(3);
		enableDefinitions.appendEnum16(447);
		MessageParser::execute(enableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 2);

		eventActionService.eventActionDefinitionMap.clear();
	}

	SECTION("Enable all event action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 8;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2, 0, 1};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5, 1, 23};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729, 45, 555};
		String<CCSDSMaxMessageSize> data[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message enableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableAllDefinitions.appendUint8(0);
		MessageParser::execute(enableAllDefinitions);

		for(const auto& iterator : eventActionService.eventActionDefinitionMap) {
			CHECK(iterator.second.enabled);
		}

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Delete event-action definitions TC[19,2]", "[service][st19]") {
	SECTION("Delete an event-action definition") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 7;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 0, 2};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 4, 5};
		uint16_t eventActionDefinitionIDs[] = {1, 7, 9, 65, 46, 27, 11};
		String<CCSDSMaxMessageSize> data[] = {"0", "1", "2", "3", "4", "5", "6"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(1);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(2);
		deleteDefinition.appendEnum16(9);
		MessageParser::execute(deleteDefinition);

		Message deleteMultipleDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteMultipleDefinitions.appendUint8(2);
		deleteMultipleDefinitions.appendEnum16(0);
		deleteMultipleDefinitions.appendEnum16(4);
		deleteMultipleDefinitions.appendEnum16(27);
		deleteMultipleDefinitions.appendEnum16(2);
		deleteMultipleDefinitions.appendEnum16(5);
		deleteMultipleDefinitions.appendEnum16(11);
		MessageParser::execute(deleteMultipleDefinitions);

		REQUIRE(eventActionService.eventActionDefinitionMap.count(0) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(4) == 2);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(2) == 0);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(1) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(5) == 0);
	}

	SECTION("Trying to delete an enabled event-action definition") {
		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(1);
		enableDefinition.appendEnum16(46);
		MessageParser::execute(enableDefinition);

		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(1);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(46);
		MessageParser::execute(deleteDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDeleteEnabledDefinitionError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Trying to delete an unknown event-action definition") {
		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(1);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(10);
		MessageParser::execute(deleteDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 2);
	}

    SECTION("Trying to delete an existing event-action definition with the wrong event definition ID") {
		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(2);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(5);
		deleteDefinition.appendEnum16(7);
		MessageParser::execute(deleteDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 3);

		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Delete all event-action definitions TC[19,3]", "[service][st19]") {
	Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	uint8_t numberOfEventActionDefinitions = 5;
	uint16_t applicationIDs[] = {1, 1, 1, 1, 1};
	uint16_t eventDefinitionIDs[] = {0, 1, 2, 3, 4};
	uint16_t eventActionDefinitionIDs[] = {0, 1, 2, 3, 4};
	String<CCSDSMaxMessageSize> data[] = {"0", "1", "2", "3", "4"};
	addDefinition.appendUint8(numberOfEventActionDefinitions);
	for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
		addDefinition.appendEnum16(applicationIDs[i]);
		addDefinition.appendEnum16(eventDefinitionIDs[i]);
		addDefinition.appendEnum16(eventActionDefinitionIDs[i]);
		addDefinition.appendOctetString(data[i]);
	}
	MessageParser::execute(addDefinition);

	REQUIRE(eventActionService.eventActionDefinitionMap.size() == 5);

	Message deleteAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DeleteAllEventAction, Message::TC, 0);
	MessageParser::execute(deleteAllDefinitions);

	CHECK(eventActionService.eventActionDefinitionMap.empty());
}

TEST_CASE("Disable event-action definitions TC[19,5]", "[service][st19]") {
	SECTION("Simple disable some event-action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 9;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2, 0, 1, 0};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 4, 1, 5, 1, 23, 1};
		uint16_t eventActionDefinitionIDs[] = {724, 171, 447, 547, 588, 729, 45, 555, 234};
		String<CCSDSMaxMessageSize> data[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendEnum16(eventActionDefinitionIDs[i]);
			addDefinitions.appendOctetString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(4);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(171);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(588);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(547);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(23);
		enableDefinitions.appendEnum16(555);
		MessageParser::execute(enableDefinitions);

		auto range = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(range->second.enabled);
		REQUIRE(std::next(range)->second.enabled);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		Message disableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableDefinitions.appendUint8(4);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(4);
		disableDefinitions.appendEnum16(171);
		disableDefinitions.appendEnum16(2);
		disableDefinitions.appendEnum16(5);
		disableDefinitions.appendEnum16(729);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(4);
		disableDefinitions.appendEnum16(547);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(1);
		disableDefinitions.appendEnum16(588);
		MessageParser::execute(disableDefinitions);

		CHECK(!range->second.enabled);
		CHECK(!std::next(range)->second.enabled);
		CHECK(!eventActionService.eventActionDefinitionMap.find(5)->second.enabled);
		CHECK(!eventActionService.eventActionDefinitionMap.find(1)->second.enabled);
	}

	SECTION("Trying to disable unknown definitions") {
		Message disableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableDefinitions.appendUint8(2);
		disableDefinitions.appendUint16(2);
		disableDefinitions.appendEnum16(5);
		disableDefinitions.appendEnum16(6);
		disableDefinitions.appendUint16(2);
		disableDefinitions.appendEnum16(5);
		disableDefinitions.appendEnum16(13);
		MessageParser::execute(disableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 2);
	}

	SECTION("Trying to disable an existing event-action definition with the wrong event definition ID") {
		Message deleteDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		deleteDefinitions.appendUint8(2);
		deleteDefinitions.appendEnum16(1);
		deleteDefinitions.appendEnum16(2);
		deleteDefinitions.appendEnum16(447);
		deleteDefinitions.appendEnum16(0);
		deleteDefinitions.appendEnum16(3);
		deleteDefinitions.appendEnum16(45);
		MessageParser::execute(deleteDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 3);
	}

    SECTION("Disable all event action definitions") {
		REQUIRE(!eventActionService.eventActionDefinitionMap.empty());

		Message enableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableAllDefinitions.appendUint8(0);
		MessageParser::execute(enableAllDefinitions);

		Message disableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableAllDefinitions.appendUint8(0);
		MessageParser::execute(disableAllDefinitions);

		for(const auto& iterator : eventActionService.eventActionDefinitionMap) {
			CHECK(!iterator.second.enabled);
		}
	}
}

TEST_CASE("Request event-action definition status TC[19,6]", "[service][st19]") {
	Message statusRequest(EventActionService::ServiceType, EventActionService::MessageType::ReportStatusOfEachEventAction, Message::TC, 0);
	MessageParser::execute(statusRequest);
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 7);
}

TEST_CASE("Event-action status report TM[19,7]", "[service][st19]") {
	Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
	addDefinitions.appendUint8(2);
	addDefinitions.appendEnum16(1);
	addDefinitions.appendEnum16(0);
	addDefinitions.appendEnum16(1);
	String<CCSDSMaxMessageSize> data = "0";
	addDefinitions.appendOctetString(data);
	addDefinitions.appendEnum16(1);
	addDefinitions.appendEnum16(2);
	addDefinitions.appendEnum16(3);
	data = "2";
	addDefinitions.appendOctetString(data);
	MessageParser::execute(addDefinitions);

	Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                 0);
	enableDefinition.appendUint8(1);
	enableDefinition.appendEnum16(1);
	enableDefinition.appendEnum16(0);
	enableDefinition.appendEnum16(1);
	MessageParser::execute(enableDefinition);

	eventActionService.eventActionStatusReport();
	REQUIRE(ServiceTests::hasOneMessage());

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
	Message message(EventActionService::ServiceType, EventActionService::MessageType::EnableEventActionFunction, Message::TC, 0);
	eventActionService.enableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus());
}

TEST_CASE("Disable event-action function TC[19,9]", "[service][st19]") {
	Message message(EventActionService::ServiceType, EventActionService::MessageType::DisableEventActionFunction, Message::TC, 0);
	eventActionService.disableEventActionFunction(message);
	CHECK(eventActionService.getEventActionFunctionStatus() == false);
}

TEST_CASE("Execute a TC request", "[service][st19]") {
	SECTION("Action: Disable event-action definition") {
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(15);
		addDefinition.appendEnum16(16);

		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		messageToBeExecuted.appendUint8(1);
		messageToBeExecuted.appendEnum16(0);
		messageToBeExecuted.appendEnum16(15);
		messageToBeExecuted.appendEnum16(16);
		String<CCSDSMaxMessageSize> data = MessageParser::composeECSS(messageToBeExecuted, messageToBeExecuted.dataSize + 5);
		addDefinition.appendOctetString(data);
		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(15);
		enableDefinition.appendEnum16(16);
		MessageParser::execute(enableDefinition);
		CHECK(eventActionService.eventActionDefinitionMap.find(15)->second.enabled);

		Message enableFunction(EventActionService::ServiceType, EventActionService::MessageType::EnableEventActionFunction, Message::TC, 0);
		eventActionService.enableEventActionFunction(enableFunction);
		REQUIRE(eventActionService.getEventActionFunctionStatus());

		eventActionService.executeAction(15);

		CHECK(!eventActionService.eventActionDefinitionMap.find(15)->second.enabled);
		CHECK(ServiceTests::countErrors() == 0);
	}

	SECTION("Action: Add event-action definition") {
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(9);
		addDefinition.appendEnum16(34);

		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		messageToBeExecuted.appendUint8(1);
		messageToBeExecuted.appendEnum16(0);
		messageToBeExecuted.appendEnum16(74);
		messageToBeExecuted.appendEnum16(87);
		String<CCSDSMaxMessageSize> dt = "12345";
		messageToBeExecuted.appendOctetString(dt);
		String<CCSDSMaxMessageSize> data = MessageParser::composeECSS(messageToBeExecuted, messageToBeExecuted.dataSize + 5);
		addDefinition.appendOctetString(data);
		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(9);
		enableDefinition.appendEnum16(34);
		MessageParser::execute(enableDefinition);

		eventActionService.executeAction(9);

		auto element = eventActionService.eventActionDefinitionMap.find(74);
		CHECK(element->second.applicationId == 0);
		CHECK(element->second.eventDefinitionID == 74);
		CHECK(element->second.eventActionDefinitionID == 87);
		CHECK(!element->second.enabled);
		CHECK(element->second.request.compare(dt) == 0);
	}

	SECTION("Action: Add multiple event-action definitions") {
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(10);
		addDefinition.appendEnum16(35);

		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {3, 5, 4};
		uint16_t eventActionDefinitionIDs[] = {1, 2, 3};
		String<CCSDSMaxMessageSize> dataArray[] = {"123", "456", "789"};
		messageToBeExecuted.appendUint8(numberOfEventActionDefinitions);
		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			messageToBeExecuted.appendEnum16(applicationIDs[i]);
			messageToBeExecuted.appendEnum16(eventDefinitionIDs[i]);
			messageToBeExecuted.appendEnum16(eventActionDefinitionIDs[i]);
			messageToBeExecuted.appendOctetString(dataArray[i]);
		}
		String<CCSDSMaxMessageSize> data = MessageParser::composeECSS(messageToBeExecuted, messageToBeExecuted.dataSize + 5);

		addDefinition.appendOctetString(data);
		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(10);
		enableDefinition.appendEnum16(35);
		MessageParser::execute(enableDefinition);

		eventActionService.executeAction(10);

		for (uint8_t i = 0; i < numberOfEventActionDefinitions; i++) {
			auto element = eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i]);
			REQUIRE(element->second.applicationId == applicationIDs[i]);
			REQUIRE(element->second.eventDefinitionID == eventDefinitionIDs[i]);
			REQUIRE(element->second.eventActionDefinitionID == eventActionDefinitionIDs[i]);
			REQUIRE(!element->second.enabled);
			REQUIRE(element->second.request.compare(dataArray[i]) == 0);
		}
	}

	SECTION("Execute multiple event-action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(2);

		addDefinitions.appendEnum16(0);
		addDefinitions.appendEnum16(35);
		addDefinitions.appendEnum16(119);
		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		messageToBeExecuted.appendUint8(1);
		messageToBeExecuted.appendEnum16(0);
		messageToBeExecuted.appendEnum16(3);
		messageToBeExecuted.appendEnum16(1);
		String<CCSDSMaxMessageSize> data = MessageParser::composeECSS(messageToBeExecuted, messageToBeExecuted.dataSize + 5);
		addDefinitions.appendOctetString(data);

		addDefinitions.appendEnum16(0);
		addDefinitions.appendEnum16(35);
		addDefinitions.appendEnum16(120);
		Message messageToBeExecuted2(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		messageToBeExecuted2.appendUint8(2);
		messageToBeExecuted2.appendEnum16(1);
		messageToBeExecuted2.appendEnum16(5);
		messageToBeExecuted2.appendEnum16(2);
		messageToBeExecuted2.appendEnum16(2);
		messageToBeExecuted2.appendEnum16(4);
		messageToBeExecuted2.appendEnum16(3);
		String<CCSDSMaxMessageSize> data2 = MessageParser::composeECSS(messageToBeExecuted2, messageToBeExecuted2.dataSize + 5);
		addDefinitions.appendOctetString(data2);
		MessageParser::execute(addDefinitions);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(2);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(35);
		enableDefinitions.appendEnum16(119);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(35);
		enableDefinitions.appendEnum16(120);
		MessageParser::execute(enableDefinitions);

		eventActionService.executeAction(35);
		REQUIRE(eventActionService.getEventActionFunctionStatus());

		auto element = eventActionService.eventActionDefinitionMap.find(35);
		CHECK(element->second.request.compare(data) == 0);
		element++;
		CHECK(element->second.request.compare(data2) == 0);

		CHECK(eventActionService.eventActionDefinitionMap.find(3)->second.enabled);
		CHECK(eventActionService.eventActionDefinitionMap.count(5) == 0);
		CHECK(eventActionService.eventActionDefinitionMap.count(4) == 0);
	}
}
