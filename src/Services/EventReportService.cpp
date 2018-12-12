#include "Services/EventReportService.hpp"
#include "Message.hpp"


void EventReportService::informativeEventReport(InformationEvent eventID, String<64> data) {
	// TM[5,1]
	Message report = createTM(1);
	report.appendEnum16(eventID);
	report.appendString(data);

	storeMessage(report);
}

void
EventReportService::lowSeverityAnomalyReport(LowSeverityAnomalyEvent eventID, String<64> data) {
	// TM[5,2]
	Message report = createTM(2);
	report.appendEnum16(eventID);
	report.appendString(data);

	storeMessage(report);
}

void EventReportService::mediumSeverityAnomalyReport(MediumSeverityAnomalyEvent eventID,
                                                     String<64> data) {
	// TM[5,3]
	Message report = createTM(3);
	report.appendEnum16(eventID);
	report.appendString(data);

	storeMessage(report);
}

void
EventReportService::highSeverityAnomalyReport(HighSeverityAnomalyEvent eventID, String<64> data) {
	// TM[5,4]
	Message report = createTM(4);
	report.appendEnum16(eventID);
	report.appendString(data);

	storeMessage(report);
}
