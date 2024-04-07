#include "ECSS_Configuration.hpp"
#ifdef SERVICE_TIMESCHEDULING

#include "Services/TimeBasedSchedulingService.hpp"

TimeBasedSchedulingService::TimeBasedSchedulingService() {
	serviceType = TimeBasedSchedulingService::ServiceType;
}

Time::DefaultCUC TimeBasedSchedulingService::executeScheduledActivity(Time::DefaultCUC currentTime) {
	if (currentTime >= scheduledActivities.front().requestReleaseTime && !scheduledActivities.empty()) {
		if (scheduledActivities.front().requestID.applicationID == ApplicationId) {
			MessageParser::execute(scheduledActivities.front().request);
		}
		scheduledActivities.pop_front();
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
		const Time::DefaultCUC currentTime(TimeGetter::getCurrentTimeDefaultCUC());

		const Time::DefaultCUC releaseTime(request.readDefaultCUCTimeStamp());
		if ((scheduledActivities.available() == 0) || (releaseTime < (currentTime + ECSSTimeMarginForActivation))) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
			request.skipBytes(ECSSTCRequestStringSize);
		} else {
			etl::array<uint8_t, ECSSTCRequestStringSize> requestData = {0};
			request.readString(requestData.data(), ECSSTCRequestStringSize);
			const Message receivedTCPacket = MessageParser::parseECSSTC(requestData.data());
			ScheduledActivity newActivity;

			newActivity.request = receivedTCPacket;
			newActivity.requestReleaseTime = releaseTime;

			newActivity.requestID.sourceID = request.sourceId;
			newActivity.requestID.applicationID = request.applicationId;
			newActivity.requestID.sequenceCount = request.packetSequenceCount;

			scheduledActivities.push_back(newActivity);
		}
	}
	sortActivitiesReleaseTime(scheduledActivities);
	notifyNewActivityAddition();
}

void TimeBasedSchedulingService::timeShiftAllActivities(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::TimeShiftALlScheduledActivities)) {
		return;
	}

	const Time::DefaultCUC current_time(TimeGetter::getCurrentTimeDefaultCUC());

	const auto releaseTimes =
	    etl::minmax_element(scheduledActivities.begin(), scheduledActivities.end(),
	                        [](ScheduledActivity const& leftSide, ScheduledActivity const& rightSide) {
		                        return leftSide.requestReleaseTime < rightSide.requestReleaseTime;
	                        });
	// todo (#267): Define what the time format is going to be
	const Time::RelativeTime relativeOffset = request.readRelativeTime();
	if ((releaseTimes.first->requestReleaseTime + std::chrono::seconds(relativeOffset)) < (current_time + ECSSTimeMarginForActivation)) {
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
	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.read<SourceId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

		auto requestIDMatch = etl::find_if_not(scheduledActivities.begin(), scheduledActivities.end(),
		                                       [&receivedRequestID](ScheduledActivity const& currentElement) {
			                                       return receivedRequestID != currentElement.requestID;
		                                       });

		if (requestIDMatch != scheduledActivities.end()) {
			if ((requestIDMatch->requestReleaseTime + relativeOffset) <
			    (current_time + ECSSTimeMarginForActivation)) {
				ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
			} else {
				requestIDMatch->requestReleaseTime += relativeOffset;
			}
		} else {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
		}
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
		receivedRequestID.sourceID = request.read<SourceId>();
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
		report.appendString(MessageParser::composeECSS(activity.request));
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
		receivedRequestID.sourceID = request.read<SourceId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

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
		receivedRequestID.sourceID = request.read<SourceId>();
		receivedRequestID.applicationID = request.read<ApplicationProcessId>();
		receivedRequestID.sequenceCount = request.read<SequenceCount>();

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
		report.append<SourceId>(match.requestID.sourceID);
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
