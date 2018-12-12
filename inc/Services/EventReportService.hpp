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

	/**
	 * Type of the information event
	 */
	enum InformationEvent {
		/**
		 * An unknown event occured
		 */
			InformativeUnknownEvent = 0,
		/**
		 * Watchdogs have reset
		 */
			WWDGReset = 1,
		/**
		 * An assertion has failed
		 */
			AssertionFail = 2,
		/**
		 * Microcontroller has started
		 */
			MCUStart = 3,
	};

	/**
	 * Type of the low severity anomaly event
	 */
	enum LowSeverityAnomalyEvent {
		/**
		 * An unknown anomaly of low severity anomaly has occurred
		 */
			LowSeverityUnknownEvent = 1,
	};

	/**
	 * Type of the medium severity anomaly event
	 */
	enum MediumSeverityAnomalyEvent {
		/**
		 * An unknown anomaly of medium severity has occurred
		 */
			MediumSeverityUnknownEvent = 2,
	};

	/**
	 * Type of the high severity anomaly event
	 */
	enum HighSeverityAnomalyEvent {
		/**
		 * An unknown anomaly of high severity has occurred
		 */
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
	void informativeEventReport(InformationEvent eventID, String<64> data);

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
	lowSeverityAnomalyReport(LowSeverityAnomalyEvent eventID, String<64> data);

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
	void mediumSeverityAnomalyReport(MediumSeverityAnomalyEvent eventID, String<64> data);

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
	void highSeverityAnomalyReport(HighSeverityAnomalyEvent eventID, String<64> data);

};

#endif //ECSS_SERVICES_EVENTREPORTSERVICE_HPP
