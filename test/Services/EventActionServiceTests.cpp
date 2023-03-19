#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/EventActionService.hpp>
#include <catch2/catch_all.hpp>
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
		String<ECSSTCRequestStringSize> data = "12345abcdefg";
		addDefinition.appendFixedString(data);
		MessageParser::execute(addDefinition);

		auto element = eventActionService.eventActionDefinitionMap.find(2);
		CHECK(element->second.applicationID == 0);
		CHECK(element->second.eventDefinitionID == 2);
		CHECK(!element->second.enabled);
		for (auto i = 0; i < data.size(); ++i) {
			CHECK(data[i] == element->second.request[i]);
		}
	}

	SECTION("Adding multiple event-action definitions for different events") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 3;
		uint16_t applicationIDs[] = {0, 1, 2};
		uint16_t eventDefinitionIDs[] = {3, 5, 4};
		String<ECSSTCRequestStringSize> dataArray[] = {"123", "456", "789"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendFixedString(dataArray[i]);
		}
		MessageParser::execute(addDefinitions);

		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			auto element = eventActionService.eventActionDefinitionMap.find(eventDefinitionIDs[i]);
			CHECK(element->second.applicationID == applicationIDs[i]);
			CHECK(element->second.eventDefinitionID == eventDefinitionIDs[i]);
			CHECK(element->second.request == dataArray[i]);
			CHECK(!element->second.enabled);
			CHECK(element->second.request.size() == ECSSTCRequestStringSize);
		}
	}

	SECTION("Add an event definition ID that already exists in the same message and a different one") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(3);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(3);
		String<ECSSTCRequestStringSize> data = "123";
		addDefinitions.appendFixedString(data);
		addDefinitions.appendEnum16(6);
		addDefinitions.appendEnum16(3);
		data = "123";
		addDefinitions.appendFixedString(data);
		addDefinitions.appendEnum16(2);
		addDefinitions.appendEnum16(3);
		data = "234";
		addDefinitions.appendFixedString(data);
		MessageParser::execute(addDefinitions);

		auto element = eventActionService.eventActionDefinitionMap.find(3);
		CHECK(element->second.applicationID == 2);
		CHECK(element->second.request == "234");

		ServiceTests::reset();
	}

	SECTION("Add an event action definition that is already enabled") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(1);
		addDefinitions.appendEnum16(0);
		addDefinitions.appendEnum16(3);
		String<ECSSTCRequestStringSize> data = "456";
		addDefinitions.appendFixedString(data);
		MessageParser::execute(addDefinitions);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);

		enableDefinitions.appendUint8(1);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(3);
		MessageParser::execute(enableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionEnabledError));
		CHECK(ServiceTests::countErrors() == 1);

		eventActionService.eventActionDefinitionMap.clear();
		ServiceTests::reset();
	}

	SECTION("Add an event-action definition when the eventActionDefinitionMap is full") {
		Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		String<ECSSTCRequestStringSize> data = "123";
		message.appendFixedString(data);
		uint16_t applicationID = 257;

		for (uint16_t eventDefinitionID = 0; eventDefinitionID < 256; ++eventDefinitionID) {
			EventActionService::EventActionDefinition temp(--applicationID, eventDefinitionID, message);
			eventActionService.eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temp));
			message.resetRead();
		}

		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinitions.appendUint8(2);
		addDefinitions.appendEnum16(1);
		addDefinitions.appendEnum16(256);
		addDefinitions.appendEnum16(0);
		addDefinitions.appendEnum16(257);
		addDefinitions.appendFixedString(data);
		MessageParser::execute(addDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDefinitionsMapIsFull));
		CHECK(ServiceTests::countErrors() == 2);
		eventActionService.eventActionDefinitionMap.clear();
	}
}

TEST_CASE("Enable event-action definitions TC[19,4]", "[service][st19]") {
	SECTION("Simple enable some event-action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 6;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 6, 1, 5};
		String<ECSSTCRequestStringSize> data[] = {"0", "1", "2", "3", "4", "5"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendFixedString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		auto element = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(!element->second.enabled);
		REQUIRE(!std::next(element)->second.enabled);
		REQUIRE(!eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(3);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(2);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(1);
		MessageParser::execute(enableDefinitions);

		CHECK(element->second.enabled);
		CHECK(std::next(element)->second.enabled);
		CHECK(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);
	}

	SECTION("Trying to enable an unknown definition") {
		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendUint16(1);
		enableDefinition.appendEnum16(7);
		MessageParser::execute(enableDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Trying to enable an existing event-action definition with the wrong event definition ID") {
		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(2);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(3);
		MessageParser::execute(enableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 2);

		eventActionService.eventActionDefinitionMap.clear();
	}

	SECTION("Enable all event action definitions") {
		Message addDefinitions(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		uint8_t numberOfEventActionDefinitions = 8;
		uint16_t applicationIDs[] = {1, 0, 1, 0, 0, 2, 0, 1};
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 6, 1, 5, 1, 23};
		String<ECSSTCRequestStringSize> data[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendFixedString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message enableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableAllDefinitions.appendUint8(0);
		MessageParser::execute(enableAllDefinitions);

		for (const auto& iterator: eventActionService.eventActionDefinitionMap) {
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
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 6, 1, 7, 5};
		String<ECSSTCRequestStringSize> data[] = {"0", "1", "2", "3", "4", "5", "6"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendFixedString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(2);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(2);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(7);
		MessageParser::execute(deleteDefinition);

		REQUIRE(eventActionService.eventActionDefinitionMap.count(0) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(2) == 0);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(1) == 1);
		REQUIRE(eventActionService.eventActionDefinitionMap.count(7) == 0);
	}

	SECTION("Trying to delete an enabled event-action definition") {
		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(1);
		MessageParser::execute(enableDefinition);

		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(1);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(1);
		MessageParser::execute(deleteDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionDeleteEnabledDefinitionError));
		CHECK(ServiceTests::countErrors() == 1);
	}

	SECTION("Trying to delete an unknown event-action definition") {
		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(1);
		deleteDefinition.appendEnum16(1);
		deleteDefinition.appendEnum16(8);
		MessageParser::execute(deleteDefinition);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 2);
	}

	SECTION("Trying to delete an existing event-action definition with the wrong event definition ID") {
		Message deleteDefinition(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction, Message::TC, 0);
		deleteDefinition.appendUint8(2);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(0);
		deleteDefinition.appendEnum16(5);
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
	String<ECSSTCRequestStringSize> data[] = {"0", "1", "2", "3", "4"};
	addDefinition.appendUint8(numberOfEventActionDefinitions);
	for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
		addDefinition.appendEnum16(applicationIDs[i]);
		addDefinition.appendEnum16(eventDefinitionIDs[i]);
		addDefinition.appendFixedString(data[i]);
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
		uint16_t eventDefinitionIDs[] = {0, 4, 2, 12, 1, 5, 8, 23, 3};
		String<ECSSTCRequestStringSize> data[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
		addDefinitions.appendUint8(numberOfEventActionDefinitions);
		for (auto i = 0; i < numberOfEventActionDefinitions; i++) {
			addDefinitions.appendEnum16(applicationIDs[i]);
			addDefinitions.appendEnum16(eventDefinitionIDs[i]);
			addDefinitions.appendFixedString(data[i]);
		}
		MessageParser::execute(addDefinitions);

		Message enableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinitions.appendUint8(4);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(4);
		enableDefinitions.appendEnum16(0);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(2);
		enableDefinitions.appendEnum16(1);
		enableDefinitions.appendEnum16(23);
		MessageParser::execute(enableDefinitions);

		auto range = eventActionService.eventActionDefinitionMap.find(4);
		REQUIRE(range->second.enabled);
		REQUIRE(std::next(range)->second.enabled);
		REQUIRE(eventActionService.eventActionDefinitionMap.find(1)->second.enabled);

		Message disableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableDefinitions.appendUint8(4);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(4);
		disableDefinitions.appendEnum16(2);
		disableDefinitions.appendEnum16(5);
		disableDefinitions.appendEnum16(1);
		disableDefinitions.appendEnum16(2);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(1);
		MessageParser::execute(disableDefinitions);

		CHECK(!range->second.enabled);
		CHECK(!std::next(range)->second.enabled);
		CHECK(!eventActionService.eventActionDefinitionMap.find(5)->second.enabled);
		CHECK(!eventActionService.eventActionDefinitionMap.find(1)->second.enabled);
	}

	SECTION("Trying to disable unknown definitions") {
		Message disableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableDefinitions.appendUint8(2);
		disableDefinitions.appendUint16(1);
		disableDefinitions.appendEnum16(6);
		disableDefinitions.appendUint16(0);
		disableDefinitions.appendEnum16(13);
		MessageParser::execute(disableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventActionDefinitionIDError));
		CHECK(ServiceTests::countErrors() == 2);
	}

	SECTION("Trying to disable an existing event-action definition with the wrong event definition ID") {
		Message disableDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableDefinitions.appendUint8(2);
		disableDefinitions.appendEnum16(1);
		disableDefinitions.appendEnum16(5);
		disableDefinitions.appendEnum16(0);
		disableDefinitions.appendEnum16(5);
		MessageParser::execute(disableDefinitions);

		CHECK(ServiceTests::thrownError(ErrorHandler::EventActionUnknownEventDefinitionError));
		CHECK(ServiceTests::countErrors() == 4);
	}

	SECTION("Disable all event action definitions") {
		REQUIRE(!eventActionService.eventActionDefinitionMap.empty());

		Message enableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableAllDefinitions.appendUint8(0);
		MessageParser::execute(enableAllDefinitions);

		Message disableAllDefinitions(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		disableAllDefinitions.appendUint8(0);
		MessageParser::execute(disableAllDefinitions);

		for (const auto& iterator: eventActionService.eventActionDefinitionMap) {
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
	String<ECSSTCRequestStringSize> data = "0";
	addDefinitions.appendFixedString(data);
	addDefinitions.appendEnum16(1);
	addDefinitions.appendEnum16(2);
	data = "2";
	addDefinitions.appendFixedString(data);
	MessageParser::execute(addDefinitions);

	Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC,
	                         0);
	enableDefinition.appendUint8(1);
	enableDefinition.appendEnum16(1);
	enableDefinition.appendEnum16(0);
	MessageParser::execute(enableDefinition);

	eventActionService.eventActionStatusReport();
	REQUIRE(ServiceTests::hasOneMessage());

	Message report = ServiceTests::get(0);
	CHECK(report.readUint16() == 2);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 0);
	CHECK(report.readBoolean() == 1);
	CHECK(report.readEnum16() == 1);
	CHECK(report.readEnum16() == 2);
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

		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction, Message::TC, 0);
		messageToBeExecuted.appendUint8(1);
		messageToBeExecuted.appendEnum16(0);
		messageToBeExecuted.appendEnum16(15);
		addDefinition.appendMessage(messageToBeExecuted, ECSSTCRequestStringSize);
		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(15);
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

		Message messageToBeExecuted(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		messageToBeExecuted.appendUint8(1);
		messageToBeExecuted.appendEnum16(0);
		messageToBeExecuted.appendEnum16(74);
		String<ECSSTCRequestStringSize> dt = "12345";
		messageToBeExecuted.appendFixedString(dt);
		addDefinition.appendMessage(messageToBeExecuted, ECSSTCRequestStringSize);
		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(9);
		MessageParser::execute(enableDefinition);

		eventActionService.executeAction(9);

		auto element = eventActionService.eventActionDefinitionMap.find(74);
		CHECK(element->second.applicationID == 0);
		CHECK(element->second.eventDefinitionID == 74);
		CHECK(!element->second.enabled);
		CHECK(element->second.request.size() == ECSSTCRequestStringSize);
	}

	SECTION("Action: ParameterService::ReportParameterValues") {
		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
		addDefinition.appendUint8(1);
		addDefinition.appendEnum16(0);
		addDefinition.appendEnum16(10);

		Message TCToBeExecuted = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                                 Message::TC, 1);
		TCToBeExecuted.appendUint16(3);
		TCToBeExecuted.appendUint16(0);
		TCToBeExecuted.appendUint16(1);
		TCToBeExecuted.appendUint16(2);
		addDefinition.appendMessage(TCToBeExecuted, ECSSTCRequestStringSize);

		MessageParser::execute(addDefinition);

		Message enableDefinition(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction, Message::TC, 0);
		enableDefinition.appendUint8(1);
		enableDefinition.appendEnum16(0);
		enableDefinition.appendEnum16(10);
		MessageParser::execute(enableDefinition);

		eventActionService.executeAction(10);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == ParameterService::ServiceType);
		CHECK(report.messageType == ParameterService::MessageType::ParameterValuesReport);
		CHECK(report.readUint16() == 3);
		CHECK(report.readUint16() == 0);
		CHECK(report.readUint8() == 3);
		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint32() == 10);
	}
}

//TEST_CASE("Testing actionDefinitionExists function") {
//	SECTION("Simple existing definition") {
//		Message addDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
//		addDefinition.appendUint8(1);
//		addDefinition.appendEnum16(0);
//		addDefinition.appendEnum16(2);
//		String<ECSSTCRequestStringSize> data = "12345abcdefg";
//		addDefinition.appendFixedString(data);
//		MessageParser::execute(addDefinition);
//
//		etl::multimap<uint16_t, EventActionService::EventActionDefinition, 256> eventActionDefinitionMap;
//
//		CHECK(eventActionService.actionDefinitionExists(eventActionDefinitionMap.begin(), 2) == true);
//		CHECK(eventActionService.actionDefinitionExists(eventActionDefinitionMap.begin(), 125) == false);
//	}
//}