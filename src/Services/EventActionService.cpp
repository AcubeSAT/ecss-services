#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

void EventActionService::addEventActionDefinitions(Message message){
	// TC[19,1]
	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();

	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
}

void EventActionService::enableEventActionDefinitions(Message message) {
	// TC[19,4]
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
}

void EventActionService::requestEventActionDefinitionStatus(Message message) {
	// TC[19,6]
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
}

void EventActionService::enableEventActionFunction(Message message) {
	// TC[19,8]
}

void EventActionService::disableEventActionFunction(Message message) {
	// TC[19,9]
}
