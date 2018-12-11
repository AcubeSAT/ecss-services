#ifndef ECSS_SERVICES_EVENTREPORTSERVICE_HPP
#define ECSS_SERVICES_EVENTREPORTSERVICE_HPP

#include "Service.hpp"
#include <bitset>
/**
 * Implementation of ST[05] event reporting service
 * @todo add enum event definition id (and maybe some appending?)
 *
 * @todo add more enums event IDs
 *
 * Note: enum IDs are these just for test purposes
 *
 */
#define CSS_EVENTS_MAX_COUNT 16
#define ECSS_EVENTS_BITS 16

class EventReportService : public Service {
private:
	static const uint16_t numberOfEvents = 7;
	std::bitset<numberOfEvents> stateOfEvents;
public:
	// Variables that count the event reports per severity level
	uint16_t lowSeverityReportCount;
	uint16_t mediumSeverityReportCount;
	uint16_t highSeverityReportCount;

	// Variables that count the event occurences per severity level
	uint16_t lowSeverityEventCount;
	uint16_t mediumSeverityEventCount;
	uint16_t highSeverityEventCount;

	uint16_t disabledEventsCount;

	uint16_t lastLowSeverityReportID;
	uint16_t lastMediumSeverityReportID;
	uint16_t lastHighSeverityReportID;

	EventReportService() {
		stateOfEvents.set();
		serviceType = 5;
		lowSeverityReportCount = 0;
		mediumSeverityReportCount = 0;
		highSeverityReportCount = 0;
		disabledEventsCount = 0;
		lowSeverityEventCount = 0;
		mediumSeverityEventCount = 0;
		highSeverityEventCount = 0;
		lastLowSeverityReportID = 65535;
		lastMediumSeverityReportID = 65535;
		lastHighSeverityReportID = 65535;
	}

	/**
	 * Type of the information event
	 *
	 * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
	 * changes.
	 */
	enum Event {
		/**
		 * An unknown event occured
		 */
			InformativeUnknownEvent = 0,
		/**
		 * Watchdogs have reset
		 */
			WWDGReset = 1,
		/**
		 * Assertion has failed
		 */
			AssertionFail = 2,
		/**
		 * Microcontroller has started
		 */
			MCUStart = 3,
		/**
		 * An unknown anomaly of low severity anomalyhas occurred
		 */
			LowSeverityUnknownEvent = 4,
		/**
		 * An unknown anomaly of medium severity has occurred
		 */
			MediumSeverityUnknownEvent = 5,
		/**
		 * An unknown anomaly of high severity has occurred
		 */
			HighSeverityUnknownEvent = 6,
		/**
		 * When an execution of a notification/event fails to start
		 */
			FailedStartOfExecution = 7
	};

	/**
	 * TM[5,1] informative event report
	 * Send report to inform the respective recipients about an event
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void informativeEventReport(Event eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,2] low severiity anomaly report
	 * Send report when there is an anomaly event of low severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void
	lowSeverityAnomalyReport(Event eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,3] medium severity anomaly report
	 * Send report when there is an anomaly event of medium severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void mediumSeverityAnomalyReport(Event eventID, const uint8_t *data,
	                                 uint8_t length);

	/**
	 * TM[5,4] high severity anomaly report
	 * Send report when there is an anomaly event of high severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void highSeverityAnomalyReport(Event eventID, const uint8_t *data,
	                               uint8_t length);

	/**
	 * TC[5,5] request to enable report generation
	 * Telecommand to enable the report generation of event definitions
	 */
	void enableReportGeneration(uint16_t length, Event *eventID);

	/**
	 * TC[5,6] request to disable report generation
	 * Telecommand to disable the report generation of event definitions
	 */
	void disableReportGeneration(uint16_t length, Event *eventID);

	/**
	 * TC[5,7] request to report the disabled event definitions
	 * Note: No arguments, according to the standard.
	 */
	void requestListOfDisabledEvents();

	/**
	 * TM[5,8] disabled event definitions report
	 * Telemetry package of a report of the disabled event definitions
	 */
	void listOfDisabledEventsReport();

	/**
	 * Getter for stateOfEvents bitset
	 * @return stateOfEvents, just in case the whole bitset is needed
	 */
	std::bitset<numberOfEvents> getStateOfEvents() {
		return stateOfEvents;
	}
};

#endif //ECSS_SERVICES_EVENTREPORTSERVICE_HPP
