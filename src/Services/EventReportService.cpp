#include "ECSS_Configuration.hpp"
#ifdef SERVICE_EVENTREPORT

#include <Services/EventReportService.hpp>
#include "Message.hpp"
#include "ErrorHandler.hpp"
#include "ServicePool.hpp"

EventActionService& eventAction = Services.eventAction;
bool EventReportService::validateParameters(Event eventID) {
	if (static_cast<EventDefinitionId>(eventID) > numberOfEvents || static_cast<EventDefinitionId>(eventID) == 0) {
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::InvalidEventID);
		return false;
	}
	return true;
}


void EventReportService::informativeEventReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	if (!validateParameters(eventID)) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);
		return;
	}
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		Message report = createTM(EventReportService::MessageType::InformativeEventReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		eventAction.executeAction(eventID);

		storeMessage(report);
	}
}

void EventReportService::lowSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	if (!validateParameters(eventID)) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);
		return;
	}
	lowSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		lowSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::LowSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastLowSeverityReportID = static_cast<EventDefinitionId>(eventID);
		eventAction.executeAction(eventID);

		storeMessage(report);
	}
}

void EventReportService::mediumSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	if (!validateParameters(eventID)) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);
		return;
	}
	mediumSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		mediumSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::MediumSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastMediumSeverityReportID = static_cast<EventDefinitionId>(eventID);
		eventAction.executeAction(eventID);

		storeMessage(report);
	}
}

void EventReportService::highSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data) {
	if (!validateParameters(eventID)) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);

		return;
	}
	highSeverityEventCount++;
	if (stateOfEvents[static_cast<EventDefinitionId>(eventID)]) {
		highSeverityReportCount++;
		Message report = createTM(EventReportService::MessageType::HighSeverityAnomalyReport);
		report.append<EventDefinitionId>(eventID);
		report.appendString(data);
		lastHighSeverityReportID = static_cast<EventDefinitionId>(eventID);
		eventAction.executeAction(eventID);

		storeMessage(report);
	}
}

void EventReportService::enableReportGeneration(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::EnableReportGenerationOfEvents)) { return; }
	uint16_t const length = message.readUint16();
	if (length > numberOfEvents) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);
		return;
	}
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[message.read<EventDefinitionId>()] = true;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

void EventReportService::disableReportGeneration(Message& message) {
	if (!message.assertTC(ServiceType, MessageType::DisableReportGenerationOfEvents)) { return; }
	uint16_t const length = message.readUint16();
	if (length > numberOfEvents) {
		//Add ST[01] handling
		ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::LengthExceedsNumberOfEvents);
		return;
	}
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[message.read<EventDefinitionId>()] = false;
		}
	}
	disabledEventsCount = stateOfEvents.size() - stateOfEvents.count();
}

void EventReportService::requestListOfDisabledEvents(const Message& message) {
	if (!message.assertTC(ServiceType, MessageType::ReportListOfDisabledEvents)) { return; }

	listOfDisabledEventsReport();
}

void EventReportService::listOfDisabledEventsReport() {
	Message report = createTM(EventReportService::MessageType::DisabledListEventReport);

	uint16_t const numberOfDisabledEvents = stateOfEvents.size() - stateOfEvents.count(); // NOLINT(cppcoreguidelines-init-variables)
	report.appendHalfword(numberOfDisabledEvents);
	for (size_t i = 0; i < stateOfEvents.size(); i++) { if (not stateOfEvents[i]) { report.append<EventDefinitionId>(i); } }

	storeMessage(report);
}

void EventReportService::execute(Message& message) {
	switch (message.messageType) {
		case EnableReportGenerationOfEvents:
			enableReportGeneration(message);
			break;
		case DisableReportGenerationOfEvents:
			disableReportGeneration(message);
			break;
		case ReportListOfDisabledEvents:
			requestListOfDisabledEvents(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif