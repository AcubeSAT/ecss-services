#ifndef ECSS_SERVICES_EVENTREPORTSERVICE_HPP
#define ECSS_SERVICES_EVENTREPORTSERVICE_HPP

#include "Service.hpp"
/**
 * Implementation of ST[05] event reporting service
 * @todo add enum event definition id (and maybe some appending?)
 *
 * @todo changes enums event IDs
 *
 * Note: enum IDs are these just for test purposes
 *
 */
#define CSS_EVENTS_MAX_COUNT 16
#define ECSS_EVENTS_BITS 16

class EventReportService : public Service {
public:
	EventReportService() {
		serviceType = 5;
	}

	enum InformationEvent {
		UnknownEvent = 0,
		WWDGReset = 1,
		AssertionFail = 2,
		MCUStart = 3,
	};

	enum LowSeverityAnomalyEvent {
		LowSeverityUnknownEvent = 1,
	};

	enum MediumSeverityAnomalyEvent {
		MediumSeverityUnknownEvent = 2,
	};

	enum HighSeverityAnomalyEvent {
		HighSeverityUnknownEvent = 3,
	};

	/**
	 * TM[5,1] informative event report
	 * Send report to inform the respective recipients about an event
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void informativeEventReport(InformationEvent eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,2] low severiity anomaly report
	 * Send report when there is an anomaly event of low severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void
	lowSeverityAnomalyReport(LowSeverityAnomalyEvent eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,3] medium severity anomaly report
	 * Send report when there is an anomaly event of medium severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void mediumSeverityAnomalyReport(MediumSeverityAnomalyEvent eventID, const uint8_t *data,
	                                 uint8_t length);

	/**
	 * TM[5,4] high severity anomaly report
	 * Send report when there is an anomaly event of hgih severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void highSeverityAnomalyReport(HighSeverityAnomalyEvent, const uint8_t *data,
	                               uint8_t length);

	/**
	 * TC[5,5]
	 */
	void enableReportGeneration(uint8_t N);

	/**
	 * TC[5,6]
	 */
	void disableReportGeneration(uint8_t N);

	/**
	 * TC[5,7]
	 */
	void requestListOfDisabledEvents();

	/**
	 * TM[5,8]
	 */
	void listOfDisabledEventsReport();

};

#endif //ECSS_SERVICES_EVENTREPORTSERVICE_HPP
