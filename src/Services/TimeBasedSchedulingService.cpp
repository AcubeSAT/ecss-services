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
		uint32_t currentTime = TimeGetter::getSeconds(); // Get the current system time

		uint32_t releaseTime = request.readUint32(); // Get the specified release time
		if ((scheduledActivities.size() >= ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES) ||
		    (releaseTime < (currentTime + ECSS_TIME_MARGIN_FOR_ACTIVATION))) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
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

			scheduledActivities.push_back(newActivity); // Insert the new activities
		}
	}
	sortActivitiesReleaseTime(scheduledActivities); // Sort activities by their release time
}

void TimeBasedSchedulingService::timeShiftAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 15);

	uint32_t current_time = TimeGetter::getSeconds(); // Get the current system time

	// Find the earliest release time. It will be the first element of the iterator pair
	const auto releaseTimes = etl::minmax_element(scheduledActivities.begin(),
	                                              scheduledActivities.end(),
	                                              [](ScheduledActivity const &leftSide,
	                                                 ScheduledActivity const &
	                                                 rightSide) {
		                                              return leftSide.requestReleaseTime <
		                                                     rightSide.requestReleaseTime;
	                                              });
	// todo: Define what the time format is going to be
	int32_t relativeOffset = request.readSint32(); // Get the relative offset
	if ((releaseTimes.first->requestReleaseTime + relativeOffset) <
	    (current_time + ECSS_TIME_MARGIN_FOR_ACTIVATION)) {
		// Report the error
		ErrorHandler::reportError(request, ErrorHandler::SubServiceExecutionStartError);
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

	uint32_t current_time = TimeGetter::getSeconds(); // Get the current system time

	int32_t relativeOffset = request.readSint32(); // Get the offset first

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// Parse the request ID
		RequestID receivedRequestID; // Save the received request ID
		receivedRequestID.sourceID = request.readUint8(); // Get the source ID
		receivedRequestID.applicationID = request.readUint16(); // Get the application ID
		receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

		// Try to find the activity with the requested request ID
		auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
		                                       scheduledActivities.end(),
		                                       [&receivedRequestID]
			                                       (ScheduledActivity const &currentElement) {
			                                       return receivedRequestID !=
			                                              currentElement.requestID;
		                                       });

		if (requestIDMatch != scheduledActivities.end()) {
			// If the relative offset does not meet the restrictions issue an error
			if ((requestIDMatch->requestReleaseTime + relativeOffset) <
			    (current_time + ECSS_TIME_MARGIN_FOR_ACTIVATION)) {
				ErrorHandler::reportError(request, ErrorHandler::SubServiceExecutionStartError);
			} else {
				requestIDMatch->requestReleaseTime += relativeOffset; // Add the time offset
			}
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}
	sortActivitiesReleaseTime(scheduledActivities); // Sort activities by their release time
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
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}
}

void TimeBasedSchedulingService::detailReportAllActivities(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 16);

	// Create the report message object of telemetry message subtype 10 for each activity
	Message report = createTM(10);
	report.appendUint16(static_cast<uint16_t >(scheduledActivities.size()));

	for (auto &activity : scheduledActivities) {
		// todo: append sub-schedule and group ID if they are defined

		report.appendUint32(activity.requestReleaseTime);
		report.appendString(msgParser.convertTCToStr(activity.request));
	}
	storeMessage(report); // Save the report
}

void TimeBasedSchedulingService::detailReportActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 9);

	// Create the report message object of telemetry message subtype 10 for each activity
	Message report = createTM(10);
	etl::list<ScheduledActivity, ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES> matchedActivities;

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
			matchedActivities.push_back(*requestIDMatch); // Save the matched activity
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}

    sortActivitiesReleaseTime(matchedActivities); // Sort activities by their release time

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t >(matchedActivities.size()));
	for (auto &match : matchedActivities) {
		report.appendUint32(match.requestReleaseTime); // todo: Replace with the time parser
		report.appendString(msgParser.convertTCToStr(match.request));
	}
	storeMessage(report); // Save the report
}

void TimeBasedSchedulingService::summaryReportActivitiesByID(Message &request) {

	// Check if the correct packet is being processed
	assert(request.serviceType == 11);
	assert(request.messageType == 12);

	// Create the report message object of telemetry message subtype 13 for each activity
	Message report = createTM(13);
	etl::list<ScheduledActivity, ECSS_MAX_NUMBER_OF_TIME_SCHED_ACTIVITIES> matchedActivities;

	uint16_t iterationCount = request.readUint16(); // Get the iteration count, (N)
	for (std::size_t i = 0; i < iterationCount; i++) {
		// Parse the request ID
		RequestID receivedRequestID; // Save the received request ID
		receivedRequestID.sourceID = request.readUint8(); // Get the source ID
		receivedRequestID.applicationID = request.readUint16(); // Get the application ID
		receivedRequestID.sequenceCount = request.readUint16(); // Get the sequence count

		// Try to find the activity with the requested request ID
		auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(),
		                                       scheduledActivities.end(), [&receivedRequestID]
			                                       (ScheduledActivity const &currentElement) {
				return receivedRequestID != currentElement
					.requestID;
			});

		if (requestIDMatch != scheduledActivities.end()) {
			matchedActivities.push_back(*requestIDMatch);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}
	sortActivitiesReleaseTime(matchedActivities); // Sort activities by their release time

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t >(matchedActivities.size()));
	for (auto &match : matchedActivities) {
		// todo: append sub-schedule and group ID if they are defined
		report.appendUint32(match.requestReleaseTime);
		report.appendUint8(match.requestID.sourceID);
		report.appendUint16(match.requestID.applicationID);
		report.appendUint16(match.requestID.sequenceCount);
	}
	storeMessage(report); // Save the report
}
