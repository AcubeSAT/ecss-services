#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

/**
 * @todo: Should a check be added for index to not exceed the size of eventActionDefinitionArray
 * ? Also check if there is needed a uint16_t (in case of changing the size of
 * eventActionDefinitionArray
 */
void EventActionService::addEventActionDefinitions(Message message) {
	// TC[19,1]

	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType ==
																						19) {
		uint16_t index = 0;
		uint8_t flag = 0; // used as boolean 0 is false, 1 is true
		while (eventActionDefinitionArray[index].empty == 0) {
			if (index == 255) { // 255 should be changed depending on size of the array
				flag = 1;
				break;
			}
			index++;
		}
		if (flag == 0) {
			char data[128];
			eventActionDefinitionArray[index].empty = 0;
			eventActionDefinitionArray[index].applicationId = message.readEnum16();
			eventActionDefinitionArray[index].eventDefinitionID = message.readEnum16();
			// Tests pass with message.dataSize - 3, message.dataSize - 4, but not
			// message.dataSize - 5
			message.readString(data, message.dataSize);
			eventActionDefinitionArray[index].request = String<64>(data);
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
	if (message.messageType == 2 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t N = message.readUint16();
		uint8_t index = 0;
		for (uint16_t i = 0; i < N; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index<255){
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
			       eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					eventActionDefinitionArray[index].empty = 1;
					eventActionDefinitionArray[index].eventDefinitionID = 65535;
					eventActionDefinitionArray[index].request = "";
					eventActionDefinitionArray[index].applicationId = 0;
				}
				index++;
			}
			index = 0;
		}

	}
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
	if (message.messageType == 3 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		for (uint16_t index = 0; index < 256; index++) {
			if (eventActionDefinitionArray[index].empty == 0) {
				eventActionDefinitionArray[index].empty = 1;
				eventActionDefinitionArray[index].eventDefinitionID = 65535;
				eventActionDefinitionArray[index].request = "";
				eventActionDefinitionArray[index].applicationId = 0;
			}
		}
	}
}

void EventActionService::enableEventActionDefinitions(Message message) {
	// TC[19,4]
	if (message.messageType == 4 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t N = message.readUint16();
		uint8_t index = 0;
		for (uint16_t i = 0; i < N; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index<255){
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					stateOfEventAction[index] = 1;
				}
				index++;
			}
			index = 0;
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
	if (message.messageType == 5 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t N = message.readUint16();
		uint8_t index = 0;
		for (uint16_t i = 0; i < N; i++) {
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index<255){
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					stateOfEventAction[index] = 0;
				}
				index++;
			}
			index = 0;
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
	for (uint16_t i = 0; i < 256; i++) {
		if (eventActionDefinitionArray[i].empty == 0) {
			report.appendEnum16(eventActionDefinitionArray[i].applicationId);
			report.appendEnum16(eventActionDefinitionArray[i].eventDefinitionID);
			report.appendUint8(stateOfEventAction[i]);
		}
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message message) {
	// TC[19,8]
	if (message.messageType == 8 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(EventActionFunctionStatus::enabledFunction);
	}
}

void EventActionService::disableEventActionFunction(Message message) {
	// TC[19,9]
	if (message.messageType == 9 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		setEventActionFunctionStatus(EventActionFunctionStatus::disabledFunction);
	}
}

// Should I use the name execute here instead of executeAction?
// Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus == enabledFunction) {
		uint16_t i = 0;
		while (i < 256) {
			if (eventActionDefinitionArray[i].empty == 0) {
				if (eventActionDefinitionArray[i].eventDefinitionID == eventID) {
					MessageParser messageParser;
					Message message = messageParser.parseRequestTC(eventActionDefinitionArray[i].request);
					messageParser.execute(message);				}
			}
			i++;
		}
	}
}
