#include "Services/EventReportService.hpp"
#include "Message.hpp"


void EventReportService::informativeEventReport(uint16_t eventID, const uint8_t *data,
																		uint8_t length){
	// TM[5,1]
	Message report = createTM(1);
	report.appendBits(16,eventID);
	report.appendString(length,(char *)data);
	storeMessage(report);
}

void EventReportService::lowSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
																			uint8_t length ){
	// TM[5,2]
	Message report = createTM(2);
	report.appendBits(16,eventID);
	report.appendString(length,(char *)data);
	storeMessage(report);
}

void EventReportService::mediumSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
																				uint8_t length){
	// TM[5,3]
	Message report = createTM(3);
	report.appendBits(16,eventID);
	report.appendString(length,(char *)data);
	storeMessage(report);
}

void EventReportService::highSeverityAnomalyReport(uint16_t eventID, const uint8_t *data,
																			uint8_t length){
	// TM[5,4]
	Message report = createTM(4);
	report.appendBits(16,eventID);
	report.appendString(length,(char *)data);
	storeMessage(report);
}
