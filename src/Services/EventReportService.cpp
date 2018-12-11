#include <Services/EventReportService.hpp>
#include "Services/EventReportService.hpp"
#include "Message.hpp"

/**
 * @todo: Add message type in TCs
 * @todo: this code is error prone, depending on parameters given, add fail safes (probably?)
 */
void EventReportService::informativeEventReport(Event eventID, const uint8_t *data,
                                                uint8_t length) {
	// TM[5,1]
	if (stateOfEvents[static_cast<uint16_t> (eventID)] == 1){
		Message report = createTM(1);
		report.appendEnum16(eventID);
		report.appendString(length, data);

		storeMessage(report);
	}
}

void
EventReportService::lowSeverityAnomalyReport(Event eventID, const uint8_t *data,
                                             uint8_t length) {
	lowSeverityEventCount++;
	// TM[5,2]
	if (stateOfEvents[static_cast<uint16_t> (eventID)] == 1) {
		lowSeverityReportsCount++;
		Message report = createTM(2);
		report.appendEnum16(eventID);
		report.appendString(length, data);
		lastLowSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
	}
}

void EventReportService::mediumSeverityAnomalyReport(Event eventID,
                                                     const uint8_t *data, uint8_t length) {
	mediumSeverityEventCount++;
	// TM[5,3]
	if (stateOfEvents[static_cast<uint16_t> (eventID)] == 1) {
		mediumSeverityReportCount++;
		Message report = createTM(3);
		report.appendEnum16(eventID);
		report.appendString(length, data);
		lastMediumSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
	}
}

void
EventReportService::highSeverityAnomalyReport(Event eventID, const uint8_t *data,
                                              uint8_t length) {
	highSeverityEventCount++;
	// TM[5,4]
	if (stateOfEvents[static_cast<uint16_t> (eventID)] == 1) {
		highSeverityReportCount++;
		Message report = createTM(4);
		report.appendEnum16(eventID);
		report.appendString(length, data);
		lastHighSeverityReportID = static_cast<uint16_t >(eventID);

		storeMessage(report);
	}
}

void EventReportService::enableReportGeneration(uint16_t length, Event *eventID) {
	// TC[5,5]
	/**
	 * @todo: Report an error if length>numberOfEvents
	 */
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[static_cast<uint16_t> (eventID[i])] = 1;
		}
	}
}

void EventReportService::disableReportGeneration(uint16_t length, Event *eventID) {
	// TC[5,6]
	/**
	 * @todo: Report an error if length>numberOfEvents
	 */
	if (length <= numberOfEvents) {
		for (uint16_t i = 0; i < length; i++) {
			stateOfEvents[static_cast<uint16_t> (eventID[i])] = 0;
		}
	}
}

void EventReportService::requestListOfDisabledEvents() {
	// TC[5,7]
	// I think this is all that is needed here.
	listOfDisabledEventsReport();
}

void EventReportService::listOfDisabledEventsReport() {
	// TM[5,8]
	Message report = createTM(8);

	uint16_t numberOfDisabledEvents = stateOfEvents.size() - stateOfEvents.count();
	report.appendHalfword(numberOfDisabledEvents);
	for (uint16_t i = 0; i < stateOfEvents.size(); i++) {
		if (stateOfEvents[i] == 0) {
			report.appendEnum16(i);
		}
	}

	storeMessage(report);
}
