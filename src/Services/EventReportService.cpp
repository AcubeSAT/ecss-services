#include "Services/EventReportService.hpp"
#include "Message.hpp"

/**
 * Note for the funtions informativeEventReport, lowSeverityReport, mediumSeverityReport,
 * highSeverityReport: I could use
 * for (int i=0; i<length; i++ {
 * 		report.appendByte(data[i]);
 * }
 * instead of reinterpret_cast as it is a dangerous cast
 */

void EventReportService::informativeEventReport(uint16_t eventID, const uint8_t *data,
                                                uint8_t length) {
	// TM[5,1]
	Message report = createTM(1);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::lowSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
                                                  uint8_t length) {
	// TM[5,2]
	Message report = createTM(2);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::mediumSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
                                                     uint8_t length) {
	// TM[5,3]
	Message report = createTM(3);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}

void EventReportService::highSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
                                                   uint8_t length) {
	// TM[5,4]
	Message report = createTM(4);
	report.appendEnum16(eventID);
	report.appendString(length, data);

	storeMessage(report);
}
