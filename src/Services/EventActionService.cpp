#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

void EventActionService::addEventActionDefinitions(Message &message) {
	// TC[19,1]
	message.assertTC(19, 1);
	uint16_t applicationID = message.readEnum16();
	uint16_t eventDefinitionID = message.readEnum16();
	if (eventActionDefinitionMap.find(eventDefinitionID) == eventActionDefinitionMap.end()) {
		EventActionDefinition temp;
		temp.enabled = true;
		temp.applicationId = applicationID;
		temp.eventDefinitionID = eventDefinitionID;
		if (message.dataSize - 4 > ECSS_TC_REQUEST_STRING_SIZE) {
			ErrorHandler::reportInternalError(ErrorHandler::MessageTooLarge);
		} else {
			char data[ECSS_TC_REQUEST_STRING_SIZE];
			message.readString(data, message.dataSize - 4);
			temp.request = String<ECSS_TC_REQUEST_STRING_SIZE>(data);
		}
		eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temp));
	} else {
		ErrorHandler::reportError(message,
		                          ErrorHandler::EventActionAddExistingDefinitionError);
	}
}

void EventActionService::deleteEventActionDefinitions(Message &message) {
	message.assertTC(19, 2);

		uint16_t numberOfEventActionDefinitions = message.readUint16();
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			if (eventActionDefinitionMap.find(eventDefinitionID) !=
			    eventActionDefinitionMap.end()) {
				// I need this to buffer the first readEnum16, since cpp check fails if I don't
				// use it anywhere
				eventActionDefinitionMap[eventDefinitionID].applicationId = applicationID;
				if (eventActionDefinitionMap[eventDefinitionID].enabled == true) {
					ErrorHandler::reportError(message,
					ErrorHandler::EventActionDeleteEnabledDefinitionError);
				} else {
					eventActionDefinitionMap.erase(eventDefinitionID);
				}
			} else {
				ErrorHandler::reportError(message,
				    ErrorHandler::EventActionUnknownDefinitionError);
			}
		}
	}
}

void EventActionService::deleteAllEventActionDefinitions(Message &message) {
	// TC[19,3]
	message.assertTC(19, 3);

	setEventActionFunctionStatus(false);
		eventActionDefinitionMap.clear();
}

void EventActionService::enableEventActionDefinitions(Message &message) {
	// TC[19,4]
	message.assertTC(19, 4);

	uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
				if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap
					.end()) {
					// This is need to pass the cpp check. The applicationId should be used
					// somewhere
					eventActionDefinitionMap[eventDefinitionID].applicationId = applicationID;
					eventActionDefinitionMap[eventDefinitionID].enabled = true;
				} else {
					ErrorHandler::reportError(message,
					ErrorHandler::EventActionUnknownDefinitionError);
			}
		}
	} else {
			for (auto& element : eventActionDefinitionMap) {
				element.second.enabled = true;
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message &message) {
	// TC[19,5]
	message.assertTC(19, 5);

	uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
				if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap
					.end()) {
					// This is need to pass the cpp check. The applicationId should be used
					// somewhere
					eventActionDefinitionMap[eventDefinitionID].applicationId = applicationID;
					eventActionDefinitionMap[eventDefinitionID].enabled = false;
				} else {
					ErrorHandler::reportError(message,
				ErrorHandler::EventActionUnknownDefinitionError);
			}
		}
	} else {
			for (auto& element : eventActionDefinitionMap) {
				element.second.enabled = false;
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message &message) {
	// TC[19,6]
	message.assertTC(19, 6);

	eventActionStatusReport();
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(7);
	uint8_t count = eventActionDefinitionMap.size();
	report.appendUint8(count);
	for (const auto& element : eventActionDefinitionMap) {
		report.appendEnum16(element.second.applicationId);
		report.appendEnum16(element.second.eventDefinitionID);
		report.appendBoolean(element.second.enabled);
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message &message) {
	// TC[19,8]
	message.assertTC(19, 8);

	setEventActionFunctionStatus(true);
}

void EventActionService::disableEventActionFunction(Message &message) {
	// TC[19,9]
	message.assertTC(19, 9);

	setEventActionFunctionStatus(false);
}

// TODO: Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus) {
		if (eventActionDefinitionMap.find(eventID) != eventActionDefinitionMap.end()) {
			if (eventActionDefinitionMap[eventID].enabled) {
				MessageParser messageParser;
				Message message = messageParser.parseRequestTC(
					eventActionDefinitionMap[eventID].request);
				messageParser.execute(message);
			}
		}
	}
}
