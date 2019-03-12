#include "Services/TimeBasedCommandSchedulingService.hpp"


void TimeBasedCommandSchedulingService::enableScheduleExecution(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 1);

	executionFunctionStatus = true; // Enable the service
}

void TimeBasedCommandSchedulingService::disableScheduleExecution(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 2);

	executionFunctionStatus = false; // Disable the service
}

void TimeBasedCommandSchedulingService::resetSchedule(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 3);

	executionFunctionStatus = false; // Disable the service
	scheduledActivities.clear(); // Delete all scheduled activities
	// todo: Add resetting for sub-schedules and groups, if defined
}

void TimeBasedCommandSchedulingService::insertActivities(Message &request) {

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



