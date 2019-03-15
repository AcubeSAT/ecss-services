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

	// todo: Get the sub-schedule ID if they are implemented
	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// todo: Get the group ID first, if groups are used
		// todo: Read the absolute time using the helper functions from the time service

		// Temporary definitions until the Time helper is ready
		uint32_t releaseTime = 0; // Temporary release time
		uint32_t currentTime = 50; // Temporary current time

		// Get the TC packet request
		uint8_t requestData[ECSS_EVENT_SERVICE_STRING_SIZE];
		request.readString(requestData, ECSS_EVENT_SERVICE_STRING_SIZE);
		Message receivedTCPacket = msgParser.parseRequestTC(requestData);

		if ((currentNumberOfActivities >= MAX_NUMBER_OF_ACTIVITIES) || (releaseTime <
		                                                                (currentTime +
		                                                                 TIME_MARGIN_FOR_ACTIVATION))) {
			// todo: Send a failed start of execution
		} else {
			ScheduledActivity newActivity; // Create the new activity

			// Assign the attributes to the newly created activity
			newActivity.request = receivedTCPacket;
			newActivity.requestReleaseTime = releaseTime;

			// todo: When implemented save the source ID
			newActivity.requestID.applicationID = request.applicationId;
			newActivity.requestID.sequenceCount = request.packetSequenceCount;

			const auto releaseTimeOrder = etl::find_if_not(scheduledActivities.begin(),
			                                               scheduledActivities.end(),
			                                               [=]
				                                               (ScheduledActivity const &currentElement) {
				                                               return releaseTime >=
				                                                      currentElement.requestReleaseTime;
			                                               });
			// Add activities ordered by release time as per the standard requirement
			scheduledActivities.emplace(releaseTimeOrder, newActivity);
		}
	}
}

void TimeBasedSchedulingService::timeShiftAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 15);

	// Temporary variables
	uint32_t current_time = 0;

	// Find the earliest release time. It will be the first element of the iterator pair
	const auto releaseTimes = etl::minmax_element(scheduledActivities.begin(),
	                                              scheduledActivities.end(),
	                                              [](ScheduledActivity const &leftSide,
	                                                 ScheduledActivity const &
	                                                 rightSide) {
		                                              return leftSide.requestReleaseTime <
		                                                     rightSide.requestReleaseTime;
	                                              });

	uint32_t relativeOffset = request.readUint32(); // Get the relative offset
	if ((releaseTimes.first->requestReleaseTime + relativeOffset) <
	    (current_time + TIME_MARGIN_FOR_ACTIVATION)) {
		// todo: generate a failed start of execution error
	} else {
		for (auto &activity : scheduledActivities) {
			activity.requestReleaseTime += relativeOffset; // Time shift each activity
		}
	}
}

void TimeBasedSchedulingService::timeShiftActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 7);

	// Temporary variables
	uint32_t current_time = 0;

	uint32_t relativeOffset = request.readUint32(); // Get the offset first
	/*
	 * Search for the earliest activity in the schedule. If the release time of the earliest
	 * activity + relativeOffset is earlier than current_time + time_margin, reject the request
	 * and generate a failed start of execution.
	 */
	const auto releaseTimes = etl::minmax_element(scheduledActivities.begin(),
	                                              scheduledActivities.end(),
	                                              [](ScheduledActivity const &leftSide,
	                                                 ScheduledActivity const &
	                                                 rightSide) {
		                                              return leftSide.requestReleaseTime <
		                                                     rightSide.requestReleaseTime;
	                                              });

	if ((releaseTimes.first->requestReleaseTime + relativeOffset) <
	    (current_time + TIME_MARGIN_FOR_ACTIVATION)) {
		// todo: generate a failed start of execution error
	} else {

		uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
		for (std::size_t i = 0; i < iterationCount; i++) {
			// Parse the request ID
			RequestID receivedRequestID; // Save the received request ID
			receivedRequestID.sourceID = request.readUint8(); // Get the source ID
			receivedRequestID.applicationID = request.readUint16(); // Get the application ID
			receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

			// Try to find the activity with the requested request ID
			const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
			                                             scheduledActivities.end(),
			                                             [&receivedRequestID]
				                                             (ScheduledActivity const &currentElement) {
				                                             return receivedRequestID !=
				                                                    currentElement.requestID;
			                                             });

			if (requestIDMatch != scheduledActivities.end()) {
				requestIDMatch->requestReleaseTime += relativeOffset; // Add the required offset
			} else {
				// todo: Generate failed start of execution for the failed instruction
			}
		}
	}
}

void TimeBasedSchedulingService::deleteActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 5);

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// Parse the request ID
		RequestID receivedRequestID; // Save the received request ID
		receivedRequestID.sourceID = request.readUint8(); // Get the source ID
		receivedRequestID.applicationID = request.readUint16(); // Get the application ID
		receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

		// Try to find the activity with the requested request ID
		const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
		                                             scheduledActivities.end(), [&receivedRequestID]
			                                             (ScheduledActivity const &currentElement) {
				return receivedRequestID != currentElement
					.requestID;
			});

		if (requestIDMatch != scheduledActivities.end()) {
			scheduledActivities.erase(requestIDMatch); // Delete activity from the schedule
		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}
}

void TimeBasedSchedulingService::detailReportAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 16);

	for (auto& activity : scheduledActivities) {
		// Create the report message object of telemetry message subtype 10 for each activity
		Message report = createTM(10);
		// todo: append sub-schedule and group ID if they are defined

		report.appendUint32(activity.requestReleaseTime); // todo: Replace with the time parser
		report.appendString(msgParser.convertTCToStr(activity.request));

		storeMessage(report); // Save the report
		request.resetRead(); // todo: define if this statement is required
	}

}

void TimeBasedSchedulingService::detailReporActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 9);

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// Parse the request ID
		RequestID receivedRequestID; // Save the received request ID
		receivedRequestID.sourceID = request.readUint8(); // Get the source ID
		receivedRequestID.applicationID = request.readUint16(); // Get the application ID
		receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

		// Try to find the activity with the requested request ID
		const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
		                                             scheduledActivities.end(), [&receivedRequestID]
			                                             (ScheduledActivity const &currentElement) {
				return receivedRequestID != currentElement
					.requestID;
			});

		if (requestIDMatch != scheduledActivities.end()) {
			// Create the report message object of telemetry message subtype 10 for each activity
			Message report = createTM(10);
			// todo: append sub-schedule and group ID if they are defined

			report.appendUint32(requestIDMatch->requestReleaseTime); // todo: Time parser here
			report.appendString(msgParser.convertTCToStr(requestIDMatch->request));

			storeMessage(report); // Save the report
			request.resetRead(); // todo: define if this statement is required
		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}
}

void TimeBasedSchedulingService::summaryReporActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 12);

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// Parse the request ID
		RequestID receivedRequestID; // Save the received request ID
		receivedRequestID.sourceID = request.readUint8(); // Get the source ID
		receivedRequestID.applicationID = request.readUint16(); // Get the application ID
		receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

		// Try to find the activity with the requested request ID
		const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
		                                             scheduledActivities.end(), [&receivedRequestID]
			                                             (ScheduledActivity const &currentElement) {
				return receivedRequestID != currentElement
					.requestID;
			});

		if (requestIDMatch != scheduledActivities.end()) {
			// Create the report message object of telemetry message subtype 13 for each activity
			Message report = createTM(13);
			// todo: append sub-schedule and group ID if they are defined

			report.appendUint32(requestIDMatch->requestReleaseTime); // todo: Time parser here

			// todo: Replace with enumeration wherever is required (source ID and app ID)
			report.appendUint8(requestIDMatch->requestID.sourceID);
			report.appendUint16(requestIDMatch->requestID.applicationID);
			report.appendUint16(requestIDMatch->requestID.sequenceCount);

			storeMessage(report); // Save the report
			request.resetRead(); // todo: define if this statement is required
		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}
}
