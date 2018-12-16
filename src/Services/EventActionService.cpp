#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

void EventActionService::addEventActionDefinitions(Message message){
	// TC[19,1]
	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();
		for (uint16_t i = 0; i < N; i++){

		}

	}
}

void EventActionService::deleteEventActionDefinitions(Message message) {
	// TC[19,2]
	if (message.messageType == 2 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();
		for (uint16_t i = 0; i < N; i++){

		}

	}
}

void EventActionService::deleteAllEventActionDefinitions(Message message) {
	// TC[19,3]
	if (message.messageType == 3 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){

	}
}

void EventActionService::enableEventActionDefinitions(Message message) {
	// TC[19,4]
	if (message.messageType == 4 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();
		for (uint16_t i = 0; i < N; i++){

		}

	}
}

void EventActionService::disableEventActionDefinitions(Message message) {
	// TC[19,5]
	if (message.messageType == 5 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();
		for (uint16_t i = 0; i < N; i++){

		}

	}
}

void EventActionService::requestEventActionDefinitionStatus(Message message) {
	// TC[19,6]
	if (message.messageType == 6 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){

	}
}

void EventActionService::eventActionStatusReport() {
	// TM[19,7]
	Message report = createTM(7);

	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message message) {
	// TC[19,8]
	if (message.messageType == 8 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
	}
}

void EventActionService::disableEventActionFunction(Message message) {
	// TC[19,9]
	if (message.messageType == 9 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
	}
}
