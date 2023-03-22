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
 * @todo Define whether the parsed absolute release time is saved in the scheduled activity as an
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
		uint16_t applicationID = 0; ///< Application process ID
		uint16_t sequenceCount = 0; ///< Packet sequence count
		uint16_t sourceID = 0;       ///< Packet source ID

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
	 * @todo If we decide to use sub-schedules, the ID of that has to be defined
	 * @todo If groups are used, then the group ID has to be defined here
	 */
	struct ScheduledActivity {
		Message request;                         ///< Hold the received command request
		RequestID requestID;                     ///< Request ID, characteristic of the definition
		Time::DefaultCUC requestReleaseTime{0}; ///< Keep the command release time
	};

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

public:
	inline static const uint8_t ServiceType = 11;

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
	 * This function executes the next activity and removes it from the list.
	 * @return the requestReleaseTime of next activity to be executed after this time
	 */
	Time::DefaultCUC executeScheduledActivity(Time::DefaultCUC currentTime);

	/**
	 * @brief TC[11,1] enable the time-based schedule execution function
	 *
	 * @details Enables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void enableScheduleExecution(Message& request);

	/**
	 * @brief TC[11,2] disable the time-based schedule execution function
	 *
	 * @details Disables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void disableScheduleExecution(Message& request);

	/**
	 * @brief TC[11,3] reset the time-based schedule
	 *
	 * @details Resets the time-based command execution schedule, by clearing all scheduled
	 * activities.
	 * @param request Provide the received message as a parameter
	 */
	void resetSchedule(Message& request);

	/**
	 * @brief TC[11,4] insert activities into the time based schedule
	 *
	 * @details Add activities into the schedule for future execution. The activities are inserted
	 * by ascending order of their release time. This done to avoid confusion during the
	 * execution of the schedule and also to make things easier whenever a release time sorted
	 * report is requested by he corresponding service.
	 * @param request Provide the received message as a parameter
	 * @todo Definition of the time format is required
	 * @throws ExecutionStartError If there is request to be inserted and the maximum
	 * number of activities in the current schedule has been reached, then an @ref
	 * ErrorHandler::ExecutionStartErrorType is being issued.  Also if the release time of the
	 * request is less than a set time margin, defined in @ref ECSS_TIME_MARGIN_FOR_ACTIVATION,
	 * from the current time a @ref ErrorHandler::ExecutionStartErrorType is also issued.
	 */
	void insertActivities(Message& request);

	/**
	 * @brief TC[11,15] time-shift all scheduled activities
	 *
	 * @details All scheduled activities are shifted per user request. The relative time offset
	 * received and tested against the current time.
	 * @param request Provide the received message as a parameter
	 * @todo Definition of the time format is required for the relative time format
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
	 * @todo Replace the time parsing with the time parser
	 */
	void detailReportAllActivities(Message& request);

	/**
	 * @brief TM[11,10] time-based schedule detail report
	 *
	 * @details Send a detailed report about the status of the activities listed
	 * on the provided list. Generates a TM[11,10] response.
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
	 * @todo Replace time parsing with the time parser
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
	 * @details Time-shift certain activities by using the unique request identifier
	 * @param request Provide the received message as a parameter
	 * @todo Definition of the time format is required
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
