#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

void EventActionService::addEventActionDefinitions(Message message) {
	// TC[19,1]

	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType ==
	                                                                     19) {
		uint16_t applicationID = message.readEnum16();
		uint16_t eventDefinitionID = message.readEnum16();
		if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap.end()) {
			EventActionDefinition temp;
			temp.enabled = true;
			temp.applicationId = applicationID;
			temp.eventDefinitionID = eventDefinitionID;
			if (message.dataSize - 4 > ECSS_EVENT_SERVICE_STRING_SIZE) {
				ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::MessageTooLarge);
			} else {
				char data[ECSS_EVENT_SERVICE_STRING_SIZE];
				message.readString(data, message.dataSize - 4);
				temp.request = String<ECSS_EVENT_SERVICE_STRING_SIZE>(data);
			}
			eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temp));
		} else {
			// @todo: throw a failed start of execution error
			//ErrorHandler::reportError
			//(ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
	if (message.messageType == 2 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			eventActionDefinitionMap.erase(eventDefinitionID);
		}
	}
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
	if (message.messageType == 3 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(false);
		eventActionDefinitionMap.clear();
	}
}

void EventActionService::enableEventActionDefinitions(Message message) {
	// TC[19,4]
	if (message.messageType == 4 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
			for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
				uint16_t applicationID = message.readEnum16();
				uint16_t eventDefinitionID = message.readEnum16();
				if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap
				.end()){
					// @todo: Check if the use etl::map at(key_parameter_t key) function instead of
					//  overloaded [] operator is better
					eventActionDefinitionMap[eventDefinitionID].enabled = true;
				}
			}
		} else {
			for (auto element : eventActionDefinitionMap) {
				element.second.enabled = true;
			}
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
	if (message.messageType == 5 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t numberOfEventActionDefinitions = message.readUint16();
		if (numberOfEventActionDefinitions != 0){
			for (uint16_t i = 0; i < numberOfEventActionDefinitions; i++) {
				uint16_t applicationID = message.readEnum16();
				uint16_t eventDefinitionID = message.readEnum16();
				if (eventActionDefinitionMap.find(eventDefinitionID) != eventActionDefinitionMap
					.end()){
					// @todo: Check if the use etl::map at(key_parameter_t key) function instead of
					//  overloaded [] operator is better
					eventActionDefinitionMap[eventDefinitionID].enabled = false;
				}
			}
		} else {
			for (auto element : eventActionDefinitionMap) {
				element.second.enabled = false;
			}
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message message) {
	// TC[19,6]
	if (message.messageType == 6 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		eventActionStatusReport();
	}
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(7);
	uint8_t count = eventActionDefinitionMap.size();
	report.appendUint8(count);
	for (auto element : eventActionDefinitionMap){
		report.appendEnum16(element.second.applicationId);
		report.appendEnum16(element.second.eventDefinitionID);
		report.appendEnum16(element.second.enabled);
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message message) {
	// TC[19,8]
	if (message.messageType == 8 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(true);
	}
}

void EventActionService::disableEventActionFunction(Message message) {
	// TC[19,9]
	if (message.messageType == 9 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(false);
	}
}

// TODO: Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus) {
		if (eventActionDefinitionMap.find(eventID) != eventActionDefinitionMap.end()){
			// @todo: Check if the use etl::map at(key_parameter_t key) function instead of
			//  overloaded [] operator is better
			if (eventActionDefinitionMap[eventID].enabled){
				MessageParser messageParser;
				Message message = messageParser.parseRequestTC(
					eventActionDefinitionMap[eventID].request);
				messageParser.execute(message);
			}
		}
	}
}
