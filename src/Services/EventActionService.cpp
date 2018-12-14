#include "Services/EventActionService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"

void EventActionService::addEventActionDefinitions(Message message){
	if (message.messageType == 1 && message.packetType == Message::TC && message.serviceType
	                                                                     == 19){
		int N = message.readUint16();

	}
}
