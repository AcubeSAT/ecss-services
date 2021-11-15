#include <iostream>
#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
#include "Services/ParameterStatisticsService.hpp"

void ParameterStatisticsService::reportParameterStatistics(Message& resetFlag) {

	ErrorHandler::assertRequest(resetFlag.packetType == Message::TC, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.messageType == MessageType::ReportParameterStatistics,
	                            resetFlag, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.serviceType == ServiceType, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message statisticsReport(ServiceType,MessageType::ParameterStatisticsReport, Message::TM, 1);

	bool resetFlagValue = resetFlag.readBoolean();
	statisticsReport.appendUint16(1);  //Dummy value for start and min time, will change in the end
	statisticsReport.appendUint16(1);
	uint16_t numOfValidParameters = 0;

	// TODO: Here is the end time
	//       append start time
	//       append end time

	for (auto &currentStatistic : systemStatistics.statisticsMap) {
		uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		numOfValidParameters++;
	}
	statisticsReport.appendUint16(numOfValidParameters);

	for (auto &currentStatistic : systemStatistics.statisticsMap) {
	    uint16_t currId = currentStatistic.first;
	    uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		statisticsReport.appendUint16(currId);
		statisticsReport.appendUint16(numOfSamples);
		currentStatistic.second.appendStatisticsToMessage(statisticsReport); // WORKS! MAGIC!
	}

	// TODO: First add start time and end time
	storeMessage(statisticsReport);

	if (resetFlagValue or hasAutomaticStatisticsReset) {
		resetParameterStatistics();
	}
	// Here add start time
}

void ParameterStatisticsService::resetParameterStatistics() {

	// TODO: Stop the evaluation of parameter statistics
	for (auto &it: systemStatistics.statisticsMap) {
		it.second.resetStatistics();
	}
	// TODO: Restart the evaluation of parameter statistics
}

void ParameterStatisticsService::enablePeriodicStatisticsReporting(Message& request) {

	/**
	 * @todo: The sampling interval of each parameter. the "timeInterval" requested should not exceed it.
	 * 		  It has to be defined as a constant.
	 */
	uint16_t SAMPLING_PARAMETER_INTERVAL = 5;

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t timeInterval = request.readUint16();
	if (timeInterval >= SAMPLING_PARAMETER_INTERVAL) {
		ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
		return;
	}

	periodicStatisticsReportingStatus = true;
	reportingInterval = timeInterval;

	uint16_t numOfParameters = systemStatistics.statisticsMap.size();
	uint16_t count = 0;
	//Only generate ONE parameter statistics report after every interval passes.
	while (count < 10) {    //TODO: while(periodicStatisticsReportingStatus)

		Message message = Message(ServiceType,MessageType::ReportParameterStatistics, Message::TC, 1);
		reportParameterStatistics(message);
		for (auto &currentStatistic : systemStatistics.statisticsMap) {

			Message statisticsReport(ServiceType,MessageType::ParameterStatisticsReport, Message::TM, 1);
			/*
			 * TODO:
			 *      1. append start time to parameterReport
			 *      2. append end time
			 */

			uint16_t currId = currentStatistic.first;
			uint16_t numOfSamples = currentStatistic.second.sampleCounter;
			if(numOfSamples == 0) {
				continue;
			}
			statisticsReport.appendUint16(numOfParameters);
			statisticsReport.appendUint16(currId);
			currentStatistic.second.appendStatisticsToMessage(statisticsReport);

			/*
			 * TODO: put the message into a queue and continue constructing the next report, and when
			 *      it's ready put that in the queue as well, another FreeRTOS task will be accountable of
			 *      keeping track of time, and when the interval passes, it's gonna pop the next
			 *      reportMessage from the queue and report it.
			 *      Or maybe use just one FreeRTOS task that will run this function per interval.
			 */
		}
		count++; //TODO: Delete
		//TODO: systematically reset the parameters' statistics.
	}
}

void ParameterStatisticsService::disablePeriodicStatisticsReporting(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DisablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	periodicStatisticsReportingStatus = false;
	reportingInterval = 0;
}

void ParameterStatisticsService::addOrUpdateStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddOrUpdateParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = request.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = request.readUint16();
		// TODO: actually define what does it mean to have an invalid parameter ID, what is the max ID??
		//       Maybe not parametersArray.size
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
			return;
		}
		bool exists = systemStatistics.statisticsMap.find(currentId) != systemStatistics.statisticsMap.end();
//		ErrorHandler::assertRequest(numOfStatisticsDefinitions < MAX_NUM_OF_DEFINITIONS, request,
//		                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

		if (hasTimeIntervals) {
			uint16_t interval = request.readUint16();
			if (interval <= reportingInterval) {
				ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
				return;
			}
			if (not exists) {
				Statistic newStat;
				systemStatistics.statisticsMap.insert({currentId, newStatistic});
				systemStatistics.statisticsMap.at(currentId).setSelfSamplingInterval(interval);
				// TODO: start the evaluation of statistics for this parameter.
			} else {
				systemStatistics.statisticsMap.at(currentId).setSelfSamplingInterval(interval);
				systemStatistics.statisticsMap.at(currentId).resetStatistics();
			}
		}
	}
}

void ParameterStatisticsService::deleteStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = request.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currentId = request.readUint16();

		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::GetNonExistingParameter);
			continue;
		}
		systemStatistics.statisticsMap.erase(currentId);
	}
	if (systemStatistics.statisticsMap.empty()) {
		periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService::deleteAllStatisticsDefinitions() {

	systemStatistics.statisticsMap.clear();
	periodicStatisticsReportingStatus = false;
}

void ParameterStatisticsService::reportStatisticsDefinitions(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message definitionsReport(ServiceType,MessageType::ParameterStatisticsDefinitionsReport,Message::TM, 1);

	uint16_t currentReportingInterval = 0;
	if (periodicStatisticsReportingStatus) {
		currentReportingInterval = reportingInterval;
	}
	definitionsReport.appendUint16(currentReportingInterval);

	uint16_t numOfDefinedParameters = 0;
	for (auto &statistic : systemStatistics.statisticsMap) {
		uint16_t currentSamplingInterval = statistic.second.selfSamplingInterval;
		if (currentSamplingInterval != 0) {
			numOfDefinedParameters++;
		}
	}
	definitionsReport.appendUint16(numOfDefinedParameters);

	for (auto &currentParam : systemStatistics.statisticsMap) {
		uint16_t currentId = currentParam.first;
		uint16_t samplingInterval = currentParam.second.selfSamplingInterval;

		if (samplingInterval != 0 and supportsSamplingInterval) {
			definitionsReport.appendUint16(currentId);
			definitionsReport.appendUint16(samplingInterval);
		}
	}
	storeMessage(definitionsReport);
}

void ParameterStatisticsService::execute(Message& message) {
	switch (message.messageType) {
		case 1:
			reportParameterStatistics(message);
			break;
		case 4:
			enablePeriodicStatisticsReporting(message);
			break;
		case 5:
			disablePeriodicStatisticsReporting(message);
			break;
		case 6:
			addOrUpdateStatisticsDefinitions(message);
			break;
		case 7:
			deleteStatisticsDefinitions(message);
			break;
		case 8:
			reportStatisticsDefinitions(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}


#endif
