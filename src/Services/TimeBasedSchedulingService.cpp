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
	} else {
		return Time::DefaultCUC::max();
	}
}

void TimeBasedSchedulingService::enableScheduleExecution(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::EnableTimeBasedScheduleExecutionFunction);
	executionFunctionStatus = true;
}

void TimeBasedSchedulingService::disableScheduleExecution(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DisableTimeBasedScheduleExecutionFunction);
	executionFunctionStatus = false;
}

void TimeBasedSchedulingService::resetSchedule(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ResetTimeBasedSchedule);
	executionFunctionStatus = false;
	scheduledActivities.clear();
	// todo: Add resetting for sub-schedules and groups, if defined
}

void TimeBasedSchedulingService::insertActivities(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::InsertActivities);

	// todo: Get the sub-schedule ID if they are implemented
	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		// todo: Get the group ID first, if groups are used
		Time::DefaultCUC currentTime = TimeGetter::getCurrentTimeDefaultCUC();

		Time::DefaultCUC releaseTime = request.readDefaultCUCTimeStamp();
		if ((scheduledActivities.available() == 0) || (releaseTime < (currentTime + ECSSTimeMarginForActivation))) {
			ErrorHandler::reportError(request, ErrorHandler::InstructionExecutionStartError);
			request.skipBytes(ECSSTCRequestStringSize);
		} else {
			uint8_t requestData[ECSSTCRequestStringSize] = {0};
			request.readString(requestData, ECSSTCRequestStringSize);
			ECSSMessage receivedTCPacket = MessageParser::parseECSSTC(requestData);
			ScheduledActivity newActivity;

			newActivity.request = receivedTCPacket;
			newActivity.requestReleaseTime = releaseTime;

			// todo: When implemented save the source ID
			newActivity.requestID.applicationID = request.applicationId;
			newActivity.requestID.sequenceCount = request.packetSequenceCount;

			scheduledActivities.push_back(newActivity);
		}
	}
	sortActivitiesReleaseTime(scheduledActivities);
	notifyNewActivityAddition();
}

void TimeBasedSchedulingService::timeShiftAllActivities(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftALlScheduledActivities);

	Time::DefaultCUC current_time = TimeGetter::getCurrentTimeDefaultCUC();

	const auto releaseTimes =
	    etl::minmax_element(scheduledActivities.begin(), scheduledActivities.end(),
	                        [](ScheduledActivity const& leftSide, ScheduledActivity const& rightSide) {
		                        return leftSide.requestReleaseTime < rightSide.requestReleaseTime;
	                        });
	// todo: Define what the time format is going to be
	Time::RelativeTime relativeOffset = request.readRelativeTime();
	if ((releaseTimes.first->requestReleaseTime + std::chrono::seconds(relativeOffset)) < (current_time + ECSSTimeMarginForActivation)) {
		ErrorHandler::reportError(request, ErrorHandler::SubServiceExecutionStartError);
	} else {
		for (auto& activity: scheduledActivities) {
			activity.requestReleaseTime += std::chrono::seconds(relativeOffset);
		}
	}
}

void TimeBasedSchedulingService::timeShiftActivitiesByID(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::TimeShiftActivitiesById);

	Time::DefaultCUC current_time = TimeGetter::getCurrentTimeDefaultCUC();

	auto relativeOffset = std::chrono::seconds(request.readRelativeTime());
	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.readUint8();
		receivedRequestID.applicationID = request.readUint16();
		receivedRequestID.sequenceCount = request.readUint16();

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

void TimeBasedSchedulingService::deleteActivitiesByID(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DeleteActivitiesById);

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.readUint8();
		receivedRequestID.applicationID = request.readUint16();
		receivedRequestID.sequenceCount = request.readUint16();

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

void TimeBasedSchedulingService::detailReportAllActivities(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportAllScheduledActivities);

	ECSSMessage report = createTM(TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);
	report.appendUint16(static_cast<uint16_t>(scheduledActivities.size()));

	for (auto& activity: scheduledActivities) {
		// todo: append sub-schedule and group ID if they are defined

		report.appendDefaultCUCTimeStamp(activity.requestReleaseTime);
		report.appendString(MessageParser::composeECSS(activity.request));
	}
	storeMessage(report);
}

void TimeBasedSchedulingService::detailReportActivitiesByID(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::DetailReportActivitiesById);

	ECSSMessage report = createTM(TimeBasedSchedulingService::MessageType::TimeBasedScheduleReportById);
	etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities> matchedActivities;

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.readUint8();
		receivedRequestID.applicationID = request.readUint16();
		receivedRequestID.sequenceCount = request.readUint16();

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

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t>(matchedActivities.size()));
	for (auto& match: matchedActivities) {
		report.appendDefaultCUCTimeStamp(match.requestReleaseTime); // todo: Replace with the time parser
		report.appendString(MessageParser::composeECSS(match.request));
	}
	storeMessage(report);
}

void TimeBasedSchedulingService::summaryReportActivitiesByID(ECSSMessage& request) {
	request.assertTC(TimeBasedSchedulingService::ServiceType, TimeBasedSchedulingService::MessageType::ActivitiesSummaryReportById);

	ECSSMessage report = createTM(TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport);
	etl::list<ScheduledActivity, ECSSMaxNumberOfTimeSchedActivities> matchedActivities;

	uint16_t iterationCount = request.readUint16();
	while (iterationCount-- != 0) {
		RequestID receivedRequestID;
		receivedRequestID.sourceID = request.readUint8();
		receivedRequestID.applicationID = request.readUint16();
		receivedRequestID.sequenceCount = request.readUint16();

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

	// todo: append sub-schedule and group ID if they are defined
	report.appendUint16(static_cast<uint16_t>(matchedActivities.size()));
	for (auto& match: matchedActivities) {
		// todo: append sub-schedule and group ID if they are defined
		report.appendDefaultCUCTimeStamp(match.requestReleaseTime);
		report.appendUint8(match.requestID.sourceID);
		report.appendUint16(match.requestID.applicationID);
		report.appendUint16(match.requestID.sequenceCount);
	}
	storeMessage(report);
}

void TimeBasedSchedulingService::execute(ECSSMessage& message) {
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
