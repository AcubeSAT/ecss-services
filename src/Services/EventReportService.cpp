#include <Services/EventReportService.hpp>
#include "Services/EventReportService.hpp"
#include "Message.hpp"

void EventReportService::informativeEventReport(InformationEvent eventID, const uint8_t *data,
                                                uint8_t length) {
	// TM[5,1]
	Message report = createTM(1);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void
EventReportService::lowSeverityAnomalyReport(LowSeverityAnomalyEvent eventID, const uint8_t *data,
                                             uint8_t length) {
	// TM[5,2]
	Message report = createTM(2);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::mediumSeverityAnomalyReport(MediumSeverityAnomalyEvent eventID,
                                                     const uint8_t *data, uint8_t length) {
	// TM[5,3]
	Message report = createTM(3);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void
EventReportService::highSeverityAnomalyReport(HighSeverityAnomalyEvent eventID, const uint8_t *data,
                                              uint8_t length) {
	// TM[5,4]
	Message report = createTM(4);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::enableReportGeneration(uint8_t N) {

}

void EventReportService::disableReportGeneration(uint8_t N) {

}

void EventReportService::requestListOfDisabledEvents() {

}

void EventReportService::listOfDisabledEventsReport() {

}
