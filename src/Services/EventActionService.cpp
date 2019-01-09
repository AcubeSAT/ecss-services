#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

/**
 * @todo: Should a check be added for index to not exceed the size of eventActionDefinitionArray
 * ? Also check if there a uint16_t is needed (in case of changing the size of
 * eventActionDefinitionArray
 */
void EventActionService::addEventActionDefinitions(Message message) {
	// TC[19,1]

	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType ==
	                                                                     19) {
		bool flag = true;
		uint16_t index;
		for (index = 0; index < 256; index++) {
			if (eventActionDefinitionArray[index].empty == true) {
				flag = false;
				break;
			}
		}
		if (flag == false) {
			char data[ECSS_EVENT_SERVICE_STRING_SIZE];
			eventActionDefinitionArray[index].empty = false;
			eventActionDefinitionArray[index].enabled = true;
			eventActionDefinitionArray[index].applicationId = message.readEnum16();
			eventActionDefinitionArray[index].eventDefinitionID = message.readEnum16();
			// Tests pass with message.dataSize - 3, message.dataSize - 4, but not
			// message.dataSize - 5
			message.readString(data, message.dataSize);
			eventActionDefinitionArray[index].request = String<ECSS_EVENT_SERVICE_STRING_SIZE>(
				data);
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
	if (message.messageType == 2 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t N = message.readUint16();
		for (uint16_t i = 0; i < N; i++) {
			uint16_t index = 0;
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index < 255) {
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					eventActionDefinitionArray[index].empty = true;
					eventActionDefinitionArray[index].eventDefinitionID = 65535;
					eventActionDefinitionArray[index].request = "";
					eventActionDefinitionArray[index].applicationId = 0;
					eventActionDefinitionArray[index].enabled = false;
				}
				index++;
			}
		}

	}
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
	if (message.messageType == 3 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		for (uint16_t index = 0; index < 256; index++) {
			if (eventActionDefinitionArray[index].empty == false) {
				eventActionDefinitionArray[index].empty = true;
				eventActionDefinitionArray[index].enabled = false;
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
		for (uint16_t i = 0; i < N; i++) {
			uint16_t index = 0;
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index < 255) {
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					eventActionDefinitionArray[index].enabled = true;
				}
				index++;
			}
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
	if (message.messageType == 5 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19) {
		uint16_t N = message.readUint16();
		for (uint16_t i = 0; i < N; i++) {
			uint16_t index = 0;
			uint16_t applicationID = message.readEnum16();
			uint16_t eventDefinitionID = message.readEnum16();
			while (index < 256) {
				if (eventActionDefinitionArray[index].applicationId == applicationID &&
				    eventActionDefinitionArray[index].eventDefinitionID == eventDefinitionID) {
					eventActionDefinitionArray[index].enabled = false;
				}
				index++;
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
	uint8_t count = 0;
	for (uint16_t i = 0; i < 256; i++) {
		if (eventActionDefinitionArray[i].empty == false) {
			count++;
		}
	}
	report.appendUint8(count);
	for (uint16_t i = 0; i < 256; i++) {
		if (eventActionDefinitionArray[i].empty == false) {
			report.appendEnum16(eventActionDefinitionArray[i].applicationId);
			report.appendEnum16(eventActionDefinitionArray[i].eventDefinitionID);
			report.appendBoolean(eventActionDefinitionArray[i].enabled);
		}
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

// Should I use the name execute here instead of executeAction?
// Should I use applicationID too?
void EventActionService::executeAction(uint16_t eventID) {
	// Custom function
	if (eventActionFunctionStatus) {
		uint16_t i = 0;
		while (i < 256) {
			if (eventActionDefinitionArray[i].empty == false &&
			    eventActionDefinitionArray[i].enabled ==
			    true) {
				if (eventActionDefinitionArray[i].eventDefinitionID == eventID) {
					MessageParser messageParser;
					Message message = messageParser.parseRequestTC(
						eventActionDefinitionArray[i].request);
					messageParser.execute(message);
				}
			}
			i++;
		}
	}
}
