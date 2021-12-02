#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTREPORT

#include <Services/EventReportService.hpp>
#include <Services/EventActionService.hpp>
#include "Message.hpp"

/**
 * @todo: Add message type in TCs
 * @todo: this code is error prone, depending on parameters given, add fail safes (probably?)
 */
void EventReportService::informativeEventReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	// TM[5,1]
	if (stateOfEvents[static_cast<uint16_t>(eventID)]) {
		Message report = createTM(EventReportService::MessageType::InformativeEventReport);
		report.appendEnum16(eventID);
		report.appendString(data);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);

		storeMessage(report);
	}
}

void
EventReportService::lowSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	lowSeverityEventCount++;
	// TM[5,2]
	if (stateOfEvents[static_cast<uint16_t>(eventID)]) {
		lowSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::LowSeverityAnomalyReport);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastLowSeverityReportID = static_cast<uint16_t>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void
EventReportService::mediumSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	mediumSeverityEventCount++;
	// TM[5,3]
	if (stateOfEvents[static_cast<uint16_t>(eventID)]) {
		mediumSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::MediumSeverityAnomalyReport);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastMediumSeverityReportID = static_cast<uint16_t>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void
EventReportService::highSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	highSeverityEventCount++;
	// TM[5,4]
	if (stateOfEvents[static_cast<uint16_t>(eventID)]) {
		highSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::HighSeverityAnomalyReport);
		report.appendEnum16(eventID);
		report.appendString(data);
		lastHighSeverityReportID = static_cast<uint16_t>(eventID);

		storeMessage(report);
		EventActionService eventActionService;
		eventActionService.executeAction(eventID);
	}
}

void EventReportService::enableReportGeneration(Message message) {
	// TC[5,5]
	message.assertTC(EventReportService::ServiceType, EventReportService::MessageType::EnableReportGenerationOfEvents);

	/**
	 * @todo: Report an error if length > numberOfEvents
	 */
	uint16_t length = message.readUint16();
	Event eventID[length];
	for (uint16_t i = 0; i < length; i++) {
		eventID[i] = static_cast<Event>(message.readEnum16());
	}
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[static_cast<uint16_t>(eventID[i])] = true;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

void EventReportService::disableReportGeneration(Message message) {
	// TC[5,6]
	message.assertTC(EventReportService::ServiceType, EventReportService::MessageType::DisableReportGenerationOfEvents);

	/**
	 * @todo: Report an error if length > numberOfEvents
	 */
	uint16_t length = message.readUint16();
	Event eventID[length];
	for (uint16_t i = 0; i < length; i++) {
		eventID[i] = static_cast<Event>(message.readEnum16());
	}
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[static_cast<uint16_t>(eventID[i])] = false;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

void EventReportService::requestListOfDisabledEvents(Message message) {
	// TC[5,7]
	message.assertTC(EventReportService::ServiceType, EventReportService::MessageType::ReportListOfDisabledEvent);

	listOfDisabledEventsReport();
}

void EventReportService::listOfDisabledEventsReport() {
	// TM[5,8]
	Message report = createTM(EventReportService::MessageType::DisabledListEventReport);

	uint16_t numberOfDisabledEvents = stateOfEvents.size() - stateOfEvents.count();
	report.appendHalfword(numberOfDisabledEvents);
	for (size_t i = 0; i < stateOfEvents.size(); i++) {
		if (not stateOfEvents[i]) {
			report.appendEnum16(i);
		}
	}

	storeMessage(report);
}

void EventReportService::execute(Message& message) {
	switch (message.messageType) {
		case EnableReportGenerationOfEvents: enableReportGeneration(message); // TC[5,5]
			break;
		case DisableReportGenerationOfEvents: disableReportGeneration(message); // TC[5,6]
			break;
		case ReportListOfDisabledEvent: requestListOfDisabledEvents(message); // TC[5,7]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
