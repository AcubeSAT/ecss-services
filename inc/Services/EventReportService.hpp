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

class EventReportService : public Service
{
private:
    static constexpr uint16_t NumberOfEvents = 6;
    /**
     * Report generation state per event definition. The bitset is indexed by the raw event definition ID
     * (IDs start at 1), so it holds one extra slot; slot 0 is unused.
     */
    etl::bitset<NumberOfEvents + 1> enabledEvents;
    static constexpr uint16_t LastElementID = std::numeric_limits<uint16_t>::max();
	/**
    * Initializes the Event Severity array by setting the default value for each event
    */
    void initializeEventDefinitionSeverityArray();

    /**
     * Counts the event definitions whose report generation is disabled, ignoring the unused slot 0
     * of the enabledEvents bitset.
     */
    uint16_t countDisabledEvents() const {
        uint16_t disabledEvents = 0;
        for (size_t i = 1; i < enabledEvents.size(); i++) {
            if (not enabledEvents[i]) {
                disabledEvents++;
            }
        }
        return disabledEvents;
    }
public:
    inline static constexpr ServiceTypeNum ServiceType = 5;

    enum MessageType : uint8_t
    {
        InformativeEventReport = 1,
        LowSeverityAnomalyReport = 2,
        MediumSeverityAnomalyReport = 3,
        HighSeverityAnomalyReport = 4,
        EnableReportGenerationOfEvents = 5,
        DisableReportGenerationOfEvents = 6,
        ReportListOfDisabledEvents = 7,
        DisabledListEventReport = 8,
    };

    enum class EventReportSeverity : uint8_t {
        Informative = 1,
        Low = 2,
        Medium = 3,
        High = 4
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

    EventReportService()
    {
        enabledEvents.set();
        serviceType = ServiceType;
        initializeEventDefinitionSeverityArray();
    }


    /**
     * Type of the information event
     *
     * Note: Numbers are kept in code explicitly, so that there is no uncertainty when something
     * changes.
     */
    enum Event
    {
        /**
         * An unknown event occured
         */
        UnknownEvent = 1,
        /**
         * Watchdogs have reset
         */
        WWDGReset = 2,
        /**
         * Assertion has failed
         */
        AssertionFail = 3,
        /**
         * Microcontroller has started
         */
        MCUStart = 4,

        /**
         * When an execution of a notification/event fails to start
         */
        FailedStartOfExecution = 5,

        /**
         * Generic event to be used by default for parameter monitoring definitions (ST[12]) whose checking
         * status transitioned to an out-of-limits status. Each parameter monitoring definition is configured
         * with the event definition it raises per checking status, so mission-specific event definitions can
         * be used instead of this one.
         */
        ParameterOutOfLimits = 6,
    };

    /**
     * Map of event definitions to their severity.
     * Index i of the array holds the severity of the event definition with ID i + 1, since event
     * definition IDs start at 1.
     *
     * e.g. eventDefinitionSeverityArray[0] = EventReportSeverity::Informative means that the event
     * with ID 1 is raised with Informative severity.
     */
    etl::array<EventReportSeverity, NumberOfEvents> eventDefinitionSeverityArray = {};

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
     * Getter for enabledEvents bitset
     * @return enabledEvents, just in case the whole bitset is needed
     */
    etl::bitset<NumberOfEvents + 1> getStateOfEvents()
    {
        return enabledEvents;
    }

    /**
    * Disabled report generation for all events
    */
    inline void disableAllEvents() {
        enabledEvents.reset();
    }

    /**
     * Validates the parameters for an event.
     * Ensures the event ID is within the allowable range and not 0.
     *
     * @param eventID The ID of the event to validate.
     * @return True if parameters are valid, false otherwise.
     */
    static inline bool validateParameters(Event eventID);

	/**
	 * Checks if the number of events included in a TC is larger than the number of events in this service.
	 * Throws a InternalErrorType::LengthExceedsNumberOfEvents if false
	 * @param tcNumberOfEvents the TC's number of events
	 * @return True if the number of events is smaller or equal to the number of events in the service, false otherwise.
	 */
	static inline bool isNumberOfEventsValid(uint16_t tcNumberOfEvents);

    /**
     * Raises the event with the given event definition ID.
     *
     * The severity of the event is determined by the eventDefinitionSeverityArray, and the corresponding
     * event report (TM[5,1] informative, TM[5,2] low, TM[5,3] medium or TM[5,4] high severity) is
     * generated. Events with no known severity are raised as informative.
     *
     * @param eventID The ID of the event to be raised. Invalid IDs (0, or larger than the number of event
     * definitions) are reported as an internal error and no event is raised.
     * @param data The auxiliary data of the event report
     */
    void raiseEvent(EventDefinitionId eventID, const String<ECSSEventDataAuxiliaryMaxSize>& data);

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
