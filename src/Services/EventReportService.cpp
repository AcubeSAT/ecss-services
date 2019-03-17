#include <Services/EventReportService.hpp>
#include <Services/EventActionService.hpp>
#include "Message.hpp"

/**
 * @todo: Add message type in TCs
 * @todo: this code is error prone, depending on parameters given, add fail safes (probably?)
 */
void EventReportService::informativeEventReport(Event eventID, const String<64> & data) {
	// TM[5,1]
	if (stateOfEvents[static_cast<uint16_t> (eventID)]) {
		Message report = createTM(1);
		report.appendEnum16(eventID);
		report.appendString(data);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);

		storeMessage(report);
	}
}

void
EventReportService::lowSeverityAnomalyReport(Event eventID, const String<64> & data) {
	lowSeverityEventCount++;
	// TM[5,2]
	if (stateOfEvents[static_cast<uint16_t> (eventID)]) {
		lowSeverityReportCount++;
		Message report = createTM(2);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastLowSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void EventReportService::mediumSeverityAnomalyReport(Event eventID, const String<64> & data) {
	mediumSeverityEventCount++;
	// TM[5,3]
	if (stateOfEvents[static_cast<uint16_t> (eventID)]) {
		mediumSeverityReportCount++;
		Message report = createTM(3);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastMediumSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void
EventReportService::highSeverityAnomalyReport(Event eventID, const String<64> & data) {
	highSeverityEventCount++;
	// TM[5,4]
	if (stateOfEvents[static_cast<uint16_t> (eventID)]) {
		highSeverityReportCount++;
		Message report = createTM(4);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastHighSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void EventReportService::enableReportGeneration(Message message) {
	// TC[5,5]
	if (message.serviceType == 5 && message.packetType == Message::TC && message.messageType == 5) {
		/**
		* @todo: Report an error if length > numberOfEvents
		*/
		uint16_t length = message.readUint16();
		Event eventID[length];
		for (uint16_t i = 0; i < length; i++) {
			eventID[i] = static_cast<Event >(message.readEnum16());
		}
		if (length <= numberOfEvents) {
			for (uint16_t i = 0; i < length; i++) {
				stateOfEvents[static_cast<uint16_t> (eventID[i])] = true;
			}
		}
		disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
	}
}

void EventReportService::disableReportGeneration(Message message) {
	// TC[5,6]
	if (message.serviceType == 5 && message.packetType == Message::TC && message.messageType
	                                                                     == 6) {
		/**
		* @todo: Report an error if length > numberOfEvents
		*/
		uint16_t length = message.readUint16();
		Event eventID[length];
		for (uint16_t i = 0; i < length; i++) {
			eventID[i] = static_cast<Event >(message.readEnum16());
		}
		if (length <= numberOfEvents) {
			for (uint16_t i = 0; i < length; i++) {
				stateOfEvents[static_cast<uint16_t> (eventID[i])] = false;
			}
		}
		disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
	}
}

void EventReportService::requestListOfDisabledEvents(Message message) {
	// TC[5,7]
	// I think this is all that is needed here.
	if (message.serviceType == 5 && message.packetType == Message::TC && message.messageType == 7) {
		listOfDisabledEventsReport();
	}
}

void EventReportService::listOfDisabledEventsReport() {
	// TM[5,8]
	Message report = createTM(8);

	uint16_t numberOfDisabledEvents = stateOfEvents.size() - stateOfEvents.count();
	report.appendHalfword(numberOfDisabledEvents);
	for (uint16_t i = 0; i < stateOfEvents.size(); i++) {
		if (stateOfEvents[i] == false) {
			report.appendEnum16(i);
		}
	}

	storeMessage(report);
}
