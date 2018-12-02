#include <Services/EventReportService.hpp>
#include "Services/EventReportService.hpp"
#include "Message.hpp"
/**
 *
 * @todo: this code is error prone, depending on parameters given, add fail safes
 */
void EventReportService::informativeEventReport(Event eventID, const uint8_t *data,
                                                uint8_t length) {
	// TM[5,1]
	Message report = createTM(1);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void
EventReportService::lowSeverityAnomalyReport(Event eventID, const uint8_t *data,
                                             uint8_t length) {
	// TM[5,2]
	Message report = createTM(2);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::mediumSeverityAnomalyReport(Event eventID,
                                                     const uint8_t *data, uint8_t length) {
	// TM[5,3]
	Message report = createTM(3);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void
EventReportService::highSeverityAnomalyReport(Event eventID, const uint8_t *data,
                                              uint8_t length) {
	// TM[5,4]
	Message report = createTM(4);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::enableReportGeneration(uint8_t length, Event *eventID) {
	// TC[5,5]
	for (uint8_t i = 0; i < length; i++) {
		stateOfEvents[static_cast<uint8_t> (eventID[i])] = 1;
	}
}

void EventReportService::disableReportGeneration(uint8_t length, Event *eventID) {
	for (uint8_t i = 0; i < length; i++) {
		stateOfEvents[static_cast<uint8_t> (eventID[i])] = 0;
	}
}

void EventReportService::requestListOfDisabledEvents() {

}

void EventReportService::listOfDisabledEventsReport() {

}
