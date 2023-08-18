#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTACTION

#include "Message.hpp"
#include "MessageParser.hpp"
#include "Services/EventActionService.hpp"

EventActionService::EventActionDefinition::EventActionDefinition(ApplicationIdSize applicationID, EventDefinitionIdSize eventDefinitionID, Message& message)
    : applicationID(applicationID), eventDefinitionID(eventDefinitionID), request(message.data + message.readPosition) {
	message.readPosition += ECSSTCRequestStringSize;
}

void EventActionService::addEventActionDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::AddEventAction)) {
		return;
	}
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	while (numberOfEventActionDefinitions-- != 0) {
		ApplicationIdSize applicationID = message.readEnum16();
		EventDefinitionIdSize eventDefinitionID = message.readEnum16();
		bool canBeAdded = true;

		for (auto& element: eventActionDefinitionMap) {
			if (element.first == eventDefinitionID) {
				if (element.second.enabled) {
					canBeAdded = false;
					ErrorHandler::reportError(message, ErrorHandler::EventActionEnabledError);
				} else {
					eventActionDefinitionMap.erase(eventDefinitionID);
				}
				break;
			}
		}
		if (canBeAdded) {
			if (eventActionDefinitionMap.size() == ECSSEventActionStructMapSize) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionDefinitionsMapIsFull);
				continue;
			}
			EventActionDefinition temporaryEventActionDefinition(applicationID, eventDefinitionID, message);
			eventActionDefinitionMap.insert(std::make_pair(eventDefinitionID, temporaryEventActionDefinition));
		}
	}
}

void EventActionService::deleteEventActionDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::DeleteEventAction)) {
		return;
	}
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	while (numberOfEventActionDefinitions-- != 0) {
		ApplicationIdSize applicationID = message.readEnum16();
		EventDefinitionIdSize eventDefinitionID = message.readEnum16();
		bool actionDefinitionExists = false;

		for (auto& element: eventActionDefinitionMap) {
			if (element.first == eventDefinitionID) {
				actionDefinitionExists = true;
				if (element.second.applicationID != applicationID) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
				} else if (element.second.enabled) {
					ErrorHandler::reportError(message, ErrorHandler::EventActionDeleteEnabledDefinitionError);
				} else {
					eventActionDefinitionMap.erase(eventActionDefinitionMap.find(eventDefinitionID));
				}
				break;
			}
		}
		if (not actionDefinitionExists) {
			ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
		}
	}
}

void EventActionService::deleteAllEventActionDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::DeleteAllEventAction)) {
		return;
	}
	setEventActionFunctionStatus(false);
	eventActionDefinitionMap.clear();
}

void EventActionService::enableEventActionDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::EnableEventAction)) {
		return;
	}
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	if (numberOfEventActionDefinitions != 0U) {
		while (numberOfEventActionDefinitions-- != 0) {
			ApplicationIdSize applicationID = message.readEnum16();
			EventDefinitionIdSize eventDefinitionID = message.readEnum16();
			bool actionDefinitionExists = false;

			for (auto& element: eventActionDefinitionMap) {
				if (element.first == eventDefinitionID) {
					actionDefinitionExists = true;
					if (element.second.applicationID != applicationID) {
						ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
					} else {
						element.second.enabled = true;
					}
					break;
				}
			}
			if (not actionDefinitionExists) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
			}
		}
	} else {
		for (auto& element: eventActionDefinitionMap) {
			element.second.enabled = true;
		}
	}
}

void EventActionService::disableEventActionDefinitions(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::DisableEventAction)) {
		return;
	}
	uint8_t numberOfEventActionDefinitions = message.readUint8();
	if (numberOfEventActionDefinitions != 0U) {
		while (numberOfEventActionDefinitions-- != 0) {
			ApplicationIdSize applicationID = message.readEnum16();
			EventDefinitionIdSize eventDefinitionID = message.readEnum16();
			bool actionDefinitionExists = false;

			for (auto& element: eventActionDefinitionMap) {
				if (element.first == eventDefinitionID) {
					actionDefinitionExists = true;
					if (element.second.applicationID != applicationID) {
						ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
					} else {
						element.second.enabled = false;
					}
					break;
				}
			}
			if (not actionDefinitionExists) {
				ErrorHandler::reportError(message, ErrorHandler::EventActionUnknownEventActionDefinitionError);
			}
		}
	} else {
		for (auto& element: eventActionDefinitionMap) {
			element.second.enabled = false;
		}
	}
}

void EventActionService::requestEventActionDefinitionStatus(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::ReportStatusOfEachEventAction)) {
		return;
	}
	eventActionStatusReport();
}

void EventActionService::eventActionStatusReport() {
	Message report = createTM(EventActionStatusReport);
	uint16_t count = eventActionDefinitionMap.size();
	report.appendUint16(count);
	for (const auto& element: eventActionDefinitionMap) {
		report.appendEnum16(element.second.applicationID);
		report.appendEnum16(element.second.eventDefinitionID);
		report.appendBoolean(element.second.enabled);
	}
	storeMessage(report);
}

void EventActionService::enableEventActionFunction(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::EnableEventActionFunction)) {
		return;
	}
	setEventActionFunctionStatus(true);
}

void EventActionService::disableEventActionFunction(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::DisableEventActionFunction)) {
		return;
	}
	setEventActionFunctionStatus(false);
}

void EventActionService::executeAction(EventDefinitionIdSize eventDefinitionID) {
	// Custom function
	if (eventActionFunctionStatus) {
		auto range = eventActionDefinitionMap.equal_range(eventDefinitionID);
		for (auto& element = range.first; element != range.second; ++element) {
			if (element->second.enabled) {
				Message message = MessageParser::parseECSSTC(element->second.request);
				MessageParser::execute(message);
			}
		}
	}
}

void EventActionService::execute(Message& message) {
	switch (message.messageType) {
		case AddEventAction:
			addEventActionDefinitions(message);
			break;
		case DeleteEventAction:
			deleteEventActionDefinitions(message);
			break;
		case DeleteAllEventAction:
			deleteAllEventActionDefinitions(message);
			break;
		case EnableEventAction:
			enableEventActionDefinitions(message);
			break;
		case DisableEventAction:
			disableEventActionDefinitions(message);
			break;
		case ReportStatusOfEachEventAction:
			requestEventActionDefinitionStatus(message);
			break;
		case EnableEventActionFunction:
			enableEventActionFunction(message);
			break;
		case DisableEventActionFunction:
			disableEventActionFunction(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
