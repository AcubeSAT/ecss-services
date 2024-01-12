#ifndef ECSS_SERVICES_EVENTREPORTSERVICE_HPP
#define ECSS_SERVICES_EVENTREPORTSERVICE_HPP

#include <etl/bitset.h>
#include "Service.hpp"

/**
 * Implementation of ST[05] event reporting service
 *
 * @ingroup Services
 * @todo (#27) add more enums event IDs
 * @todo (#219) make sure there isn't an event ID == 0, because there's a confliction with another service
 * Note: enum IDs are these just for test purposes
 *
 */

class EventReportService : public Service {
private:
	static constexpr uint16_t numberOfEvents = 7;
	etl::bitset<numberOfEvents> stateOfEvents;
	static constexpr uint16_t LastElementID = std::numeric_limits<uint16_t>::max();

public:
	inline static constexpr ServiceTypeNum ServiceType = 5;

	enum MessageType : uint8_t {
		InformativeEventReport = 1,
		LowSeverityAnomalyReport = 2,
		MediumSeverityAnomalyReport = 3,
		HighSeverityAnomalyReport = 4,
		EnableReportGenerationOfEvents = 5,
		DisableReportGenerationOfEvents = 6,
		ReportListOfDisabledEvents = 7,
		DisabledListEventReport = 8,
	};

	// Variables that count the event reports per severity level
	uint16_t lowSeverityReportCount = 0;
	uint16_t mediumSeverityReportCount = 0;
	uint16_t highSeverityReportCount = 0;

	// Variables that count the event occurrences per severity level
	uint16_t lowSeverityEventCount = 0;
	uint16_t mediumSeverityEventCount = 0;
	uint16_t highSeverityEventCount = 0;

	uint16_t disabledEventsCount = 0;

	uint16_t lastLowSeverityReportID = LastElementID;
	uint16_t lastMediumSeverityReportID = LastElementID;
	uint16_t lastHighSeverityReportID = LastElementID;

	EventReportService() {
		stateOfEvents.set();
		serviceType = ServiceType;
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
	 */
	void informativeEventReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data);

	/**
	 * TM[5,2] low severiity anomaly report
	 * Send report when there is an anomaly event of low severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 */
	void lowSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data);

	/**
	 * TM[5,3] medium severity anomaly report
	 * Send report when there is an anomaly event of medium severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 */
	void mediumSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data);

	/**
	 * TM[5,4] high severity anomaly report
	 * Send report when there is an anomaly event of high severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 */
	void highSeverityAnomalyReport(Event eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data);

	/**
	 * TC[5,5] request to enable report generation
	 * Telecommand to enable the report generation of event definitions
	 */
	void enableReportGeneration(Message& message);

	/**
	 * TC[5,6] request to disable report generation
	 * Telecommand to disable the report generation of event definitions
	 * @param message
	 */
	void disableReportGeneration(Message& message);

	/**
	 * TC[5,7] request to report the disabled event definitions
	 * Note: No arguments, according to the standard.
	 * @param message
	 */
	void requestListOfDisabledEvents(const Message& message);

	/**
	 * TM[5,8] disabled event definitions report
	 * Telemetry package of a report of the disabled event definitions
	 */
	void listOfDisabledEventsReport();

	/**
	 * Getter for stateOfEvents bitset
	 * @return stateOfEvents, just in case the whole bitset is needed
	 */
	etl::bitset<numberOfEvents> getStateOfEvents() {
		return stateOfEvents;
	}

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_EVENTREPORTSERVICE_HPP
