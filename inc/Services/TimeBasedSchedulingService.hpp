#ifndef ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
#define ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP

#include <iostream>
#include "etl/vector.h"
#include "Service.hpp"
#include "Helpers/CRCHelper.hpp"
#include "ErrorHandler.hpp"

// Define whether groups and/or sub-schedules are in use
#define GROUPS_ENABLED          0
#define SUB_SCHEDULES_ENABLED   0

#define MAX_NUMBER_OF_ACTIVITIES    10 // todo: define the maximum number of activities
#define TIME_MARGIN_FOR_ACTIVATION  60 // todo: define the time margin for the command activation


class TimeBasedSchedulingService : public Service {
public:


private:
	bool executionFunctionStatus; // True indicates "enabled" and False "disabled" state
	uint8_t currentNumberOfActivities = 0; // Keep track of the number of activities

	// Hold the data for the scheduled activity definition
	struct ScheduledActivity {
		//Message request; // Hold the received command request
		uint32_t requestReleaseTime = 0; // Keep the command release time
		// todo: If we decide to use sub-schedules, the ID of that has to be defined
		// todo: If groups are used, then the group ID has to be defined here
	};
	std::vector<ScheduledActivity> scheduledActivities; // Scheduled activity definitions


public:
	/**
	 * TC[11,1] enable the time-based schedule execution function
	 *
	 * @details Enables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void enableScheduleExecution(Message &request);

	/**
	 * TC[11,2] disable the time-based schedule execution function
	 *
	 * @details Disables the time-based command execution scheduling
	 * @param request Provide the received message as a parameter
	 */
	void disableScheduleExecution(Message &request);

	/**
	 * TC[11,3] reset the time-based schedule
	 *
	 * @details Resets the time-based command execution schedule (deletes the schedule)
	 * @param request Provide the received message as a parameter
	 */
	void resetSchedule(Message &request);

	/**
	 * TC[11,4] insert activities into the time based schedule
	 *
	 * @details Add activities into the schedule for future execution
	 * @param request Provide the received message as a parameter
	 */
	void insertActivities(Message &request);

	/**
	 * TC[11,15] time-shift all scheduled activities
	 *
	 * @details All scheduled activities are shifted per user request
	 * @param request Provide the received message as a parameter
	 */
	void timeShiftAllActivities(Message &request);

	/**
	 * TC[11,16] detail-report all activities
	 *
	 * @details Send a detailed report about the status of all the activities
				on the current schedule
	 * @param request Provide the received message as a parameter
	 */
	void detailReportAllActivities(Message &request);

	/**
	 * TC[11,5] delete time-based scheduled activities identified by a request identifier
	 *
	 * @details Delete certain activities by using the unique request identifier
	 * @param request Provide the received message as a parameter
	 */
	void deleteActivitiesByID(Message &request);

	/**
	 * TC[11,7] time-shift scheduled activities identified by a request identifier
	 *
	 * @details Time-shift certain activities by using the unique request identifier
	 * @param request Provide the received message as a parameter
	 */
	void timeShiftActivitiesByID(Message &request);


private:


};

#endif //ECSS_SERVICES_TIMEBASEDSCHEDULINGSERVICE_HPP
