#ifndef ECSS_SERVICES_EVENTREPORTSERVICE_HPP
#define ECSS_SERVICES_EVENTREPORTSERVICE_HPP

#include <etl/bitset.h>
#include "Service.hpp"

/**
 * Implementation of ST[05] event reporting service
 *
 * @ingroup Services
 * @todo (#27) add more enums event IDs
 * Event definition IDs are 1-based. ID 0 is reserved because ST[19] Event Action
 * uses the same EventDefinitionId space and already treats 0 as a valid action key.
 * Note: enum IDs are these just for test purposes
 *
 */

class EventReportService : public Service
{
private:
    static constexpr uint16_t NumberOfEvents = 5;
    static constexpr uint16_t EventBitsetSize = NumberOfEvents + 1;
    etl::bitset<EventBitsetSize> enabledEvents;
    static constexpr uint16_t LastElementID = std::numeric_limits<uint16_t>::max();
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
        FailedStartOfExecution = 5
    };

    static_assert(UnknownEvent != 0 && WWDGReset != 0 && AssertionFail != 0 && MCUStart != 0 &&
                      FailedStartOfExecution != 0,
                  "Event ID 0 is reserved");
    static_assert(FailedStartOfExecution == NumberOfEvents,
                  "NumberOfEvents must equal the highest Event ID");


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
    etl::bitset<EventBitsetSize> getStateOfEvents()
    {
        return enabledEvents;
    }

    /**
    * Disabled report generation for all events
    */
    inline void disableAllEvents() {
        enabledEvents.reset();
        enabledEvents[0] = true;
    }

    /**
     * Validates the parameters for an event.
     * Ensures the event ID is within the allowable range and not 0.
     *
     * @param eventID The ID of the event to validate.
     * @return True if parameters are valid, false otherwise.
     */
    static inline bool validateParameters(EventDefinitionId eventID);

	/**
	 * Checks if the number of events included in a TC is larger than the number of events in this service.
	 * Throws a InternalErrorType::LengthExceedsNumberOfEvents if false
	 * @param tcNumberOfEvents the TC's number of events
	 * @return True if the number of events is smaller or equal to the number of events in the service, false otherwise.
	 */
	static inline bool isNumberOfEventsValid(uint16_t tcNumberOfEvents);

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
