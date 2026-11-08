#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TIMESCHEDULING

#include "Services/TimeBasedSchedulingService.hpp"
#include "etl/vector.h"

TimeBasedSchedulingService::TimeBasedSchedulingService() {
	serviceType = TimeBasedSchedulingService::ServiceType;
}

Time::DefaultCUC TimeBasedSchedulingService::executeScheduledActivity(const Time::DefaultCUC& currentTime) {
	while ((!scheduledActivities.empty()) && currentTime >= scheduledActivities.front().requestReleaseTime) {
		Message request = scheduledActivities.front().request;
		scheduledActivities.pop_front();
		if (executionFunctionStatus) {
			MessageParser::execute(request);
		}
	}

	if (!scheduledActivities.empty()) {
		return scheduledActivities.front().requestReleaseTime;
	}
	return Time::DefaultCUC::max();
}

void TimeBasedSchedulingService::enableScheduleExecution(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::EnableTimeBasedScheduleExecutionFunction)) {
		return;
	}
	executionFunctionStatus = true;
}

void TimeBasedSchedulingService::disableScheduleExecution(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DisableTimeBasedScheduleExecutionFunction)) {
		return;
	}
	executionFunctionStatus = false;
}

void TimeBasedSchedulingService::resetSchedule(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ResetTimeBasedSchedule)) {
		return;
	}
	executionFunctionStatus = false;
	scheduledActivities.clear();
	// todo (#264): Add resetting for sub-schedules and groups, if defined
}

void TimeBasedSchedulingService::insertActivities(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::InsertActivities)) {
		return;
	}

	// todo (#228): Get the sub-schedule ID if they are implemented
	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		// todo (#229): Get the group ID first, if groups are used
		const uint16_t requestSize = instructionRequestSize(request);
		if (requestSize == 0) {
			break;
		}
		const Time::DefaultCUC releaseTime(request.readDefaultCUCTimeStamp());
		const uint8_t* const requestData = request.data.data() + request.readPosition;
		request.skipBytes(requestSize);

		if (not canInsertActivity(request, releaseTime, requestSize)) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
			continue;
		}
		ScheduledActivity newActivity{MessageParser::parse(requestData, requestSize), {}, releaseTime};
		if (not isExecutable(newActivity.request)) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
			continue;
		}

		newActivity.requestID.sourceID = newActivity.request.sourceId;
		newActivity.requestID.applicationID = newActivity.request.applicationId;
		newActivity.requestID.sequenceCount = newActivity.request.packetSequenceCount;

		scheduledActivities.push_back(newActivity);
	}
	sortActivitiesReleaseTime(scheduledActivities);
	notifyNewActivityAddition();
}

uint16_t TimeBasedSchedulingService::instructionRequestSize(const Message& request) {
	const uint32_t requestPosition = request.readPosition + ReleaseTimeSize;
	if (not ErrorHandler::assertRequest((requestPosition + CCSDSPrimaryHeaderSize) <= request.dataSize, request, ErrorHandler::MessageTooShort)) {
		return 0;
	}
	const uint32_t requestSize = MessageParser::packetSize(request.data.data() + requestPosition);
	if (not ErrorHandler::assertRequest((requestPosition + requestSize) <= request.dataSize, request, ErrorHandler::MessageTooShort)) {
		return 0;
	}
	return static_cast<uint16_t>(requestSize);
}

bool TimeBasedSchedulingService::canInsertActivity(const Message& request, const Time::DefaultCUC& releaseTime, uint16_t requestSize) const {
	const Time::DefaultCUC currentTime(TimeGetter::getCurrentTimeDefaultCUC());
	return (request.applicationId == ApplicationId) && (not scheduledActivities.full()) &&
	       (releaseTime >= (currentTime + ECSSTimeMarginForActivation)) && (requestSize <= MaxRequestSize);
}

bool TimeBasedSchedulingService::isExecutable(const Message& scheduledRequest) {
	return (scheduledRequest.packetType == Message::TC) && (scheduledRequest.applicationId == ApplicationId) &&
	       MessageParser::canExecute(scheduledRequest);
}

void TimeBasedSchedulingService::timeShiftAllActivities(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::TimeShiftALlScheduledActivities)) {
		return;
	}

	const Time::DefaultCUC current_time(TimeGetter::getCurrentTimeDefaultCUC());

	// todo (#267): Define what the time format is going to be
	const Time::RelativeTime relativeOffset = request.readRelativeTime();
	if (scheduledActivities.empty()) {
		return;
	}
	if ((scheduledActivities.front().requestReleaseTime + std::chrono::seconds(relativeOffset)) < (current_time + ECSSTimeMarginForActivation)) {
		ErrorHandler::reportError(request, ErrorHandler::SubServiceExecutionStartError);
		return;
	}
	for (auto& activity: scheduledActivities) {
		activity.requestReleaseTime += std::chrono::seconds(relativeOffset);
	}
}

void TimeBasedSchedulingService::timeShiftActivitiesByID(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::TimeShiftActivitiesById)) {
		return;
	}

	const Time::DefaultCUC current_time(TimeGetter::getCurrentTimeDefaultCUC());

	auto relativeOffset = std::chrono::seconds(request.readRelativeTime());
	etl::vector<ScheduledActivity*, ECSSMaxNumberOfTimeSchedActivities> activitiesToShift;
	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.read<ApplicationProcessUserId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

		auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(), scheduledActivities.end(),
		                                       [&receivedRequestID](ScheduledActivity const& currentElement) {
			                                       return receivedRequestID != currentElement.requestID;
		                                       });

		if (requestIDMatch == scheduledActivities.end()) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		} else if (etl::find(activitiesToShift.begin(), activitiesToShift.end(), &*requestIDMatch) == activitiesToShift.end()) {
			activitiesToShift.push_back(&*requestIDMatch);
		}
	}

	if (etl::any_of(activitiesToShift.begin(), activitiesToShift.end(), [&](const ScheduledActivity* activity) {
		    return (activity->requestReleaseTime + relativeOffset) < (current_time + ECSSTimeMarginForActivation);
	    })) {
		ErrorHandler::reportError(request, ErrorHandler::SubServiceExecutionStartError);
		return;
	}
	for (auto* activity: activitiesToShift) {
		activity->requestReleaseTime += relativeOffset;
	}
	sortActivitiesReleaseTime(scheduledActivities);
}

void TimeBasedSchedulingService::deleteActivitiesByID(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeleteActivitiesById)) {
		return;
	}

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.read<ApplicationProcessUserId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

		const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(), scheduledActivities.end(),
		                                             [&receivedRequestID](ScheduledActivity const& currentElement) {
			                                             return receivedRequestID != currentElement.requestID;
		                                             });

		if (requestIDMatch != scheduledActivities.end()) {
			scheduledActivities.erase(requestIDMatch);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}
}

void TimeBasedSchedulingService::detailReportAllActivities(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DetailReportAllScheduledActivities)) {
		return;
	}

	timeBasedScheduleDetailReport(scheduledActivities);
}

void TimeBasedSchedulingService::timeBasedScheduleDetailReport(const etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities>& listOfActivities) {
	// todo (#228): (#229) append sub-schedule and group ID if they are defined
	Message report = createTM(TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);
	report.appendUint16(static_cast<uint16_t>(listOfActivities.size()));

	for (const auto& activity: listOfActivities) {
		report.appendDefaultCUCTimeStamp(activity.requestReleaseTime); // todo (#267): Replace with the time parser
		report.appendString(MessageParser::compose(activity.request));
	}
	storeMessage(report);
}

void TimeBasedSchedulingService::detailReportActivitiesByID(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DetailReportActivitiesById)) {
		return;
	}

	etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities> matchedActivities;

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.read<ApplicationProcessUserId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

		if (matchedActivities.full()) {
			ErrorHandler::reportError(request, ErrorHandler::TimeBasedScheduleReportIsFull);
			continue;
		}

		const auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(), scheduledActivities.end(),
		                                             [&receivedRequestID](ScheduledActivity const& currentElement) {
			                                             return receivedRequestID != currentElement.requestID;
		                                             });

		if (requestIDMatch != scheduledActivities.end()) {
			matchedActivities.push_back(*requestIDMatch);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}

	sortActivitiesReleaseTime(matchedActivities);

	timeBasedScheduleDetailReport(matchedActivities);
}

void TimeBasedSchedulingService::summaryReportActivitiesByID(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ActivitiesSummaryReportById)) {
		return;
	}

	etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities> matchedActivities;

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.read<ApplicationProcessUserId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

		if (matchedActivities.full()) {
			ErrorHandler::reportError(request, ErrorHandler::TimeBasedScheduleReportIsFull);
			continue;
		}

		auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(), scheduledActivities.end(),
		                                       [&receivedRequestID](ScheduledActivity const& currentElement) {
			                                       return receivedRequestID != currentElement.requestID;
		                                       });

		if (requestIDMatch != scheduledActivities.end()) {
			matchedActivities.push_back(*requestIDMatch);
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
	}
	sortActivitiesReleaseTime(matchedActivities);

	timeBasedScheduleSummaryReport(matchedActivities);
}

void TimeBasedSchedulingService::timeBasedScheduleSummaryReport(const etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities>& listOfActivities) {
	Message report = createTM(TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport);

	// todo (#228): append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t>(listOfActivities.size()));
	for (const auto& match: listOfActivities) {
		// todo (#229): append sub-schedule and group ID if they are defined
		report.appendDefaultCUCTimeStamp(match.requestReleaseTime);
		report.append<ApplicationProcessUserId>(match.requestID.sourceID);
		report.append<ApplicationProcessId>(match.requestID.applicationID);
		report.append<SequenceCount>(match.requestID.sequenceCount);
	}
	storeMessage(report);
}

void TimeBasedSchedulingService::execute(Message& message) {
	switch (message.messageType) {
		case EnableTimeBasedScheduleExecutionFunction:
			enableScheduleExecution(message);
			break;
		case DisableTimeBasedScheduleExecutionFunction:
			disableScheduleExecution(message);
			break;
		case ResetTimeBasedSchedule:
			resetSchedule(message);
			break;
		case InsertActivities:
			insertActivities(message);
			break;
		case DeleteActivitiesById:
			deleteActivitiesByID(message);
			break;
		case TimeShiftActivitiesById:
			timeShiftActivitiesByID(message);
			break;
		case DetailReportActivitiesById:
			detailReportActivitiesByID(message);
			break;
		case ActivitiesSummaryReportById:
			summaryReportActivitiesByID(message);
			break;
		case TimeShiftALlScheduledActivities:
			timeShiftAllActivities(message);
			break;
		case DetailReportAllScheduledActivities:
			detailReportAllActivities(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
