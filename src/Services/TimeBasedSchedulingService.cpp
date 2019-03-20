#include "Services/TimeBasedSchedulingService.hpp"


TimeBasedSchedulingService::TimeBasedSchedulingService() {
	serviceType = 11;
}

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
	currentNumberOfActivities = 0;
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
		uint32_t currentTime = TimeGetter::getUnixSeconds(); // Get the current system time

		uint32_t releaseTime = request.readUint32(); // Get the specified release time
		if ((currentNumberOfActivities >= ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES) ||
		    (releaseTime < (currentTime + ECSS_TIME_MARGIN_FOR_ACTIVATION))) {
			// todo: Send a failed start of execution
			request.readPosition += ECSS_TC_REQUEST_STRING_SIZE;
		} else {
			// Get the TC packet request
			uint8_t requestData[ECSS_TC_REQUEST_STRING_SIZE] = {0};
			request.readString(requestData, ECSS_TC_REQUEST_STRING_SIZE);
			Message receivedTCPacket = msgParser.parseRequestTC(requestData);
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
			scheduledActivities.insert(releaseTimeOrder, newActivity);
			currentNumberOfActivities++;
		}
	}
}

void TimeBasedSchedulingService::timeShiftAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 15);

	uint32_t current_time = TimeGetter::getUnixSeconds(); // Get the current system time

	// Find the earliest release time. It will be the first element of the iterator pair
	const auto releaseTimes = etl::minmax_element(scheduledActivities.begin(),
	                                              scheduledActivities.end(),
	                                              [](ScheduledActivity const &leftSide,
	                                                 ScheduledActivity const &
	                                                 rightSide) {
		                                              return leftSide.requestReleaseTime <
		                                                     rightSide.requestReleaseTime;
	                                              });

	int32_t relativeOffset = request.readSint32(); // Get the relative offset
	if ((releaseTimes.first->requestReleaseTime + relativeOffset) <
	    (current_time + ECSS_TIME_MARGIN_FOR_ACTIVATION)) {
		// todo: generate a failed start of execution error
		std::cerr << "Relative offset error" << std::endl;
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

	uint32_t current_time = TimeGetter::getUnixSeconds(); // Get the current system time

	int32_t relativeOffset = request.readSint32(); // Get the offset first
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
	    (current_time + ECSS_TIME_MARGIN_FOR_ACTIVATION)) {
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
			currentNumberOfActivities--;
		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}
}

void TimeBasedSchedulingService::detailReportAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 16);

	// Create the report message object of telemetry message subtype 10 for each activity
	Message report = createTM(10);
	report.appendUint16(currentNumberOfActivities);

	for (auto &activity : scheduledActivities) {
		// todo: append sub-schedule and group ID if they are defined

		report.appendUint32(activity.requestReleaseTime); // todo: Replace with the time parser
		report.appendString(msgParser.convertTCToStr(activity.request));
	}
	storeMessage(report); // Save the report
	request.resetRead(); // todo: define if this statement is required
}

void TimeBasedSchedulingService::detailReportActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 9);

	// Create the report message object of telemetry message subtype 10 for each activity
	Message report = createTM(10);
	etl::vector<etl::ivector<TimeBasedSchedulingService::ScheduledActivity>::iterator,
		ECSS_MAX_REQUEST_COUNT> matchedActivities;

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
			const auto releaseTimeOrder = etl::find_if_not(matchedActivities.begin(),
			                                               matchedActivities.end(),
                   [=](etl::ivector<TimeBasedSchedulingService::ScheduledActivity>::iterator const
                       &currentElement) {
                return requestIDMatch->requestReleaseTime >=
                           currentElement->requestReleaseTime;
                   });

			// Add activities ordered by release time as per the standard requirement
			matchedActivities.insert(releaseTimeOrder, requestIDMatch);

		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t >(matchedActivities.size()));
	for (const auto &match : matchedActivities) {
		report.appendUint32(match->requestReleaseTime); // todo: Time parser here
		report.appendString(msgParser.convertTCToStr(match->request));
	}
	storeMessage(report); // Save the report
	request.resetRead(); // todo: define if this statement is required
}

void TimeBasedSchedulingService::summaryReportActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 12);

	// Create the report message object of telemetry message subtype 13 for each activity
	Message report = createTM(13);
	etl::vector<etl::ivector<TimeBasedSchedulingService::ScheduledActivity>::iterator,
	ECSS_MAX_REQUEST_COUNT> matchedActivities;

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
			const auto releaseTimeOrder = etl::find_if_not(matchedActivities.begin(),
			                                               matchedActivities.end(),
			[=](etl::ivector<TimeBasedSchedulingService::ScheduledActivity>::iterator const
			&currentElement) {
				return requestIDMatch->requestReleaseTime >=
				       currentElement->requestReleaseTime;});

			// Add activities ordered by release time as per the standard requirement
			matchedActivities.insert(releaseTimeOrder, requestIDMatch);

		} else {
			// todo: Generate failed start of execution for the failed instruction
		}
	}

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t >(matchedActivities.size()));
	for (const auto &match : matchedActivities) {
		// todo: append sub-schedule and group ID if they are defined
		report.appendUint32(match->requestReleaseTime); // todo: Time parser here
		report.appendUint8(match->requestID.sourceID);
		report.appendUint16(match->requestID.applicationID);
		report.appendUint16(match->requestID.sequenceCount);
	}
	storeMessage(report); // Save the report
	request.resetRead(); // todo: define if this statement is required
}
