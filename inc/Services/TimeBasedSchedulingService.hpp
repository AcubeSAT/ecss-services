#ifndef ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
#define ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP

#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "MessageParser.hpp"
#include "Service.hpp"
#include "etl/list.h"

// Include platform specific files
#include "Helpers/TimeGetter.hpp"


/**
 * @def GROUPS_ENABLED
 * @brief Indicates whether scheduling groups are enabled
 */
#define GROUPS_ENABLED 0 // NOLINT(cppcoreguidelines-macro-usage)

/**
 * @def SUB_SCHEDULES_ENABLED
 * @brief Indicates whether sub-schedules are supported
 *
 * @details Sub-schedules are currently not implemented so this has no effect
 */
#define SUB_SCHEDULES_ENABLED 0 // NOLINT(cppcoreguidelines-macro-usage)

/**
 * @brief Namespace to access private members during test
 *
 * @details Define a namespace for the access of the private members to avoid conflicts
 */
namespace unit_test {
	struct Tester;
} // namespace unit_test

/**
 * @brief An implementation of the ECSS standard ST[11] service
 *
 * @details This service is taking care of the timed release of a received TC packet from the
 * ground.
 * @todo (#227) Define whether the parsed absolute release time is saved in the scheduled activity as an
 * uint32_t or in the time format specified by the time management service.
 *
 * @ingroup Services
 */
class TimeBasedSchedulingService : public Service {
private:
	/**
	 * @brief Indicator of the schedule execution
	 * True indicates "enabled" and False "disabled" state
	 * @details The schedule execution indicator will be updated by the process that is running
	 * the time scheduling service.
	 */
	bool executionFunctionStatus = false;

	/**
	 * @brief Request identifier of the received packet
	 *
	 * @details The request identifier consists of the application process ID, the packet
	 * sequence count and the source ID, all defined in the ECSS standard.
	 */
	struct RequestID {
		ApplicationProcessId applicationID = 0; ///< Application process ID
		SequenceCount sequenceCount = 0; ///< Packet sequence count
		ApplicationProcessUserId sourceID = 0;       ///< Packet source ID

		bool operator!=(const RequestID& rightSide) const {
			return (sequenceCount != rightSide.sequenceCount) or (applicationID != rightSide.applicationID) or
			       (sourceID != rightSide.sourceID);
		}
	};

	/**
	 * @brief Instances of activities to run in the schedule
	 *
	 * @details All scheduled activities must contain the request they exist for, their release
	 * time and the corresponding request identifier.
	 *
	 * @todo (#228) If we decide to use sub-schedules, the ID of that has to be defined
	 * @todo (#229) If groups are used, then the group ID has to be defined here
	 */
	struct ScheduledActivity {
		Message request;                         ///< Hold the received command request
		RequestID requestID;                     ///< The source ID, application process ID and sequence count of the request
		Time::DefaultCUC requestReleaseTime{0}; ///< Keep the command release time
	};

	/**
	 * The size of the release time of a TC[11,4] instruction, as Message::appendDefaultCUCTimeStamp() writes it
	 */
	inline static constexpr uint16_t ReleaseTimeSize = sizeof(uint32_t);

	/**
	 * @brief Hold the scheduled activities
	 *
	 * @details The scheduled activities in this list are ordered by their release time, as the
	 * standard requests.
	 */
	etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities> scheduledActivities;

	/**
	 * @brief Sort the activities by their release time
	 *
	 * @details The ECSS standard requires that the activities are sorted in the TM message
	 * response. Also it is better to have the activities sorted.
	 */
	inline static void
	sortActivitiesReleaseTime(etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities>& schedActivities) {
		schedActivities.sort([](ScheduledActivity const& leftSide, ScheduledActivity const& rightSide) {
			// cppcheck-suppress
			return leftSide.requestReleaseTime < rightSide.requestReleaseTime;
		});
	}

	/**
	 * @brief Define a friend in order to be able to access private members during testing
	 *
	 * @details The private members defined in this class, must not in any way be public to avoid
	 * misuse. During testing, access to private members for verification is required, so an
	 * access friend structure is defined here.
	 */
	friend struct ::unit_test::Tester;

	/**
     * Notifies the timeBasedSchedulingTask after the insertion of activities to scheduleActivity list.
     */
	void notifyNewActivityAddition();

	/**
	 * Finds the size of the request of the TC[11,4] instruction at the read position of @p request, from the packet
	 * data length field of the request's primary header
	 *
	 * @return The size of the request, or 0 if the instruction runs past the end of @p request, in which case a
	 * @ref ErrorHandler::MessageTooShort error is reported
	 */
	static uint16_t instructionRequestSize(const Message& request);

	/**
	 * Checks whether the TC[11,4] instruction with the given release time and request size can add an activity,
	 * before its request is parsed
	 */
	bool canInsertActivity(const Message& request, const Time::DefaultCUC& releaseTime, uint16_t requestSize) const;

	/**
	 * Checks whether a parsed request is a TC addressed to this application process, of a type that this build executes
	 */
	static bool isExecutable(const Message& scheduledRequest);

public:
	inline static constexpr ServiceTypeNum ServiceType = 11;

	/**
	 * The size of the largest request, as a TC packet, that an activity can hold, so that a TM[11,10] can report a
	 * full schedule
	 */
	inline static constexpr uint16_t MaxRequestSize =
	    ((ECSSMaxMessageSize - sizeof(uint16_t)) / ECSSMaxNumberOfTimeSchedActivities) - ReleaseTimeSize;
	static_assert((MaxRequestSize >= (CCSDSPrimaryHeaderSize + ECSSSecondaryTCHeaderSize + (ECSSCRCIncluded ? CRCHelper::CRCField : 0U))) &&
	                  ((sizeof(uint16_t) + (static_cast<size_t>(ECSSMaxNumberOfTimeSchedActivities) * (ReleaseTimeSize + MaxRequestSize))) <= ECSSMaxMessageSize),
	              "A TM[11,10] for a full schedule cannot fit a TC packet per activity");

	enum MessageType : uint8_t {
		EnableTimeBasedScheduleExecutionFunction = 1,
		DisableTimeBasedScheduleExecutionFunction = 2,
		ResetTimeBasedSchedule = 3,
		InsertActivities = 4,
		DeleteActivitiesById = 5,
		TimeShiftActivitiesById = 7,
		DetailReportActivitiesById = 9,
		TimeBasedScheduleReportById = 10,
		ActivitiesSummaryReportById = 12,
		TimeBasedScheduledSummaryReport = 13,
		TimeShiftALlScheduledActivities = 15,
		DetailReportAllScheduledActivities = 16,
	};

	/**
	 * @brief Class constructor
	 * @details Initializes the serviceType
	 */
	TimeBasedSchedulingService();

	/**
	 * Removes every activity whose release time is not later than currentTime from the schedule, in the order of their
	 * release times, and executes its request.
	 *
	 * If the time-based schedule execution function is disabled, each activity is removed without executing its
	 * request, as ECSS 6.11.4.6 requires, and no report is generated for it. The function is disabled when the service
	 * starts and after a TC[11,2] or TC[11,3], so nothing scheduled is executed until a TC[11,1] enables it.
	 * @return the requestReleaseTime of the next activity, which is later than currentTime, or
	 * Time::DefaultCUC::max() if the schedule is empty
	 */
	Time::DefaultCUC executeScheduledActivity(const Time::DefaultCUC& currentTime);

	/**
	 * @brief TC[11,1] enable the time-based schedule execution function
	 *
	 * @details Enables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void enableScheduleExecution(const Message& request);

	/**
	 * @brief TC[11,2] disable the time-based schedule execution function
	 *
	 * @details Disables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void disableScheduleExecution(const Message& request);

	/**
	 * @brief TC[11,3] reset the time-based schedule
	 *
	 * @details Resets the time-based command execution schedule, by clearing all scheduled
	 * activities.
	 * @param request Provide the received message as a parameter
	 */
	void resetSchedule(const Message& request);

	/**
	 * @brief TC[11,4] insert activities into the time based schedule
	 *
	 * @details Add activities into the schedule for future execution. The activities are inserted
	 * by ascending order of their release time. This done to avoid confusion during the
	 * execution of the schedule and also to make things easier whenever a release time sorted
	 * report is requested by he corresponding service.
	 *
	 * Each instruction contains a release time and the request as a full TC packet, as ECSS 8.11.2.4 specifies. The
	 * activity is identified by the source ID, application process ID and sequence count of its request.
	 *
	 * The checks performed on the request of each instruction, as ECSS 6.11.4.5f requires, are:
	 * - the request is a TC packet that @ref MessageParser::parse accepts,
	 * - the request is addressed to @ref ApplicationId,
	 * - the request is at most @ref MaxRequestSize bytes long,
	 * - @ref MessageParser::canExecute accepts the service type and message type of the request.
	 * @param request Provide the received message as a parameter
	 * @todo (#230) Definition of the time format is required
	 * @throws ExecutionStartError If there is request to be inserted and the maximum
	 * number of activities in the current schedule has been reached, then an @ref
	 * ErrorHandler::ExecutionStartErrorType is being issued.  Also if the release time of the
	 * request is less than a set time margin, defined in @ref ECSS_TIME_MARGIN_FOR_ACTIVATION,
	 * from the current time a @ref ErrorHandler::ExecutionStartErrorType is also issued. If the
	 * request fails any of the checks above, a @ref ErrorHandler::ExecutionStartErrorType is also issued. If the
	 * TC[11,4] has an application process ID other than @ref ApplicationId, a @ref
	 * ErrorHandler::ExecutionStartErrorType is issued for each of its instructions.
	 * @throws AcceptanceError If an instruction runs past the end of the TC[11,4], a single @ref
	 * ErrorHandler::MessageTooShort is issued and the rest of the TC[11,4] is not read.
	 */
	void insertActivities(Message& request);

	/**
	 * @brief TC[11,15] time-shift all scheduled activities
	 *
	 * @details All scheduled activities are shifted per user request. The relative time offset
	 * received and tested against the current time.
	 * @param request Provide the received message as a parameter
	 * @todo (#231) Definition of the time format is required for the relative time format
	 * @throws ExecutionStartError If the release time of the request is less than a
	 * set time margin, defined in @ref ECSS_TIME_MARGIN_FOR_ACTIVATION, from the current time an
	 * @ref ErrorHandler::ExecutionStartErrorType report is issued for that instruction.
	 */
	void timeShiftAllActivities(Message& request);

	/**
	 * @brief TC[11,16] detail-report all activities
	 *
	 * @details Send a detailed report about the status of all the activities
	 * on the current schedule. Generates a TM[11,10] response.
	 * @param request Provide the received message as a parameter
	 * @todo (#232) Replace the time parsing with the time parser
	 */
	void detailReportAllActivities(const Message& request);

	/**
	 * @brief TM[11,10] time-based schedule detail report
	 *
	 * @details Send a detailed report about the status of the activities listed
	 * on the provided list. Generates a TM[11,10] response. Each activity is reported with its release time and its
	 * request as a full TC packet.
	 * @param listOfActivities Provide the list of activities that need to be reported on
	 */
	void timeBasedScheduleDetailReport(const etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities>& listOfActivities);

	/**
	 * @brief TC[11,9] detail-report activities identified by request identifier
	 *
	 * @details Send a detailed report about the status of the requested activities, based on the
	 * provided request identifier. Generates a TM[11,10] response. The matched activities are
	 * contained in the report, in an ascending order based on their release time.
	 * @param request Provide the received message as a parameter
	 * @todo (#233) Replace time parsing with the time parser
	 * @throws ExecutionStartError If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue an @ref
	 * ErrorHandler::ExecutionStartErrorType for that instruction.
	 */
	void detailReportActivitiesByID(Message& request);

	/**
	 * @brief TC[11,12] summary-report activities identified by request identifier
	 *
	 * @details Send a summary report about the status of the requested activities. Generates a
	 * TM[11,13] response, with activities ordered in an ascending order, based on their release
	 * time.
	 * @param request Provide the received message as a parameter
	 * @throws ExecutionStartError If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue an @ref
	 * ErrorHandler::ExecutionStartErrorType for that instruction.
	 */
	void summaryReportActivitiesByID(Message& request);

	/**
	 * @brief TM[11,13] time-based schedule summary report
	 *
	 * @details Send a summary report about the status of the activities listed
	 * on the provided list. Generates a TM[11,13] response.
	 * @param listOfActivities Provide the list of activities that need to be reported on
	 */
	void timeBasedScheduleSummaryReport(const etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities>& listOfActivities);

	/**
	 * @brief TC[11,5] delete time-based scheduled activities identified by a request identifier
	 *
	 * @details Delete certain activities by using the unique request identifier.
	 * @param request Provide the received message as a parameter
	 * @throws ExecutionStartError If a requested activity, identified by the provided
	 * request identifier is not found in the schedule issue an @ref
	 * ErrorHandler::ExecutionStartErrorType for that instruction.
	 */
	void deleteActivitiesByID(Message& request);

	/**
	 * @brief TC[11,7] time-shift scheduled activities identified by a request identifier
	 *
	 * @details Time-shift certain activities by using the unique request identifier. An activity
	 * identified by more than one instruction is shifted once.
	 * @param request Provide the received message as a parameter
	 * @todo (#234) Definition of the time format is required
	 * @throws ExecutionStartError If the requested time offset is less than the earliest
	 * time from the currently scheduled activities plus the @ref ECSS_TIME_MARGIN_FOR_ACTIVATION,
	 * then the request is rejected and an @ref ErrorHandler::ExecutionStartErrorType is issued.
	 * Also if an activity with a specified request identifier is not found, generate a failed
	 * start of execution for that specific instruction.
	 */
	void timeShiftActivitiesByID(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif // ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
