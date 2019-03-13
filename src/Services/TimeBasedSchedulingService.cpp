#include "Services/TimeBasedSchedulingService.hpp"


void TimeBasedSchedulingService::enableScheduleExecution(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 1);

	executionFunctionStatus = true; // Enable the service
}

void TimeBasedSchedulingService::disableScheduleExecution(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 2);

	executionFunctionStatus = false; // Disable the service
}

void TimeBasedSchedulingService::resetSchedule(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 3);

	executionFunctionStatus = false; // Disable the service
	scheduledActivities.clear(); // Delete all scheduled activities
	// todo: Add resetting for sub-schedules and groups, if defined
}

void TimeBasedSchedulingService::insertActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 4);

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// todo: Get the group ID first, if groups are used
		// todo: Read the absolute time using the helper functions from the time service

		// Temporary definitions until the Time helper is ready
		uint32_t releaseTime = 0; // Temporary release time
		uint32_t currentTime = 50; // Temporary current time
		// Message receivedPacket; // Temporary message field

		if ((currentNumberOfActivities >= MAX_NUMBER_OF_ACTIVITIES) || (releaseTime <
		                                                                (currentTime +
		                                                                 TIME_MARGIN_FOR_ACTIVATION))) {
			// todo: Send a failed start of execution
		} else {
			ScheduledActivity newActivity;
			// newActivity.request = receivedTCPacket;
			newActivity.requestReleaseTime = releaseTime;

			scheduledActivities.push_back(newActivity); // Insert the new activity into the schedule
		}
		// If the verification passes then do the following
		// Create a new scheduled activity in the schedule
		// Place the request specified in that instruction into the new scheduled activity
		// Set the release time of the new activity to the specified one
	}

}

void TimeBasedSchedulingService::timeShiftAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 15);

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		uint32_t relativeOffset = request.readUint32();
		// todo: define the enumeration types for the source ID and application process ID
		uint16_t receivedSequenceCount = request.readUint16(); // Get the sequence count
		/*
		* Perform a search in the vector containing the activity definitions to find a match for
		* the received parameters
		*/
	}
}

void TimeBasedSchedulingService::timeShiftActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 7);

	uint32_t relativeOffset = request.readUint32(); // Get the offset first
	/*
	 * Search for the earliest activity in the schedule. If the release time of the earliest
	 * activity + relativeOffset is earlier than current_time + time_margin, reject the request
	 * and generate a failed start of execution.
	 */
	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// todo: define the enumeration types for the source ID and application process ID
		uint16_t receivedSequenceCount = request.readUint16(); // Get the sequence count
		/*
		* Perform a search in the vector containing the activity definitions to find a match for
		* the received parameters
		*/
	}
}

void TimeBasedSchedulingService::deleteActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 5);

}

void TimeBasedSchedulingService::detailReportAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 16);

	// todo: add an extra check for the vector size to make sure it matches with
	//  currentNumberOfActivities?
	for (auto const& activity : scheduledActivities) {
		// Create the report message object of telemetry message subtype 10 for each activity
		Message report = createTM(10);
		// todo: append sub-schedule and group ID if they are defined
		// todo: append the release time of the current activity "activity.requestReleaseTime;"
		// todo: append the request contained in the activity "activity.request;"
		storeMessage(report); // Save the report
		request.resetRead(); // todo: define if this statement is required
	}

}


