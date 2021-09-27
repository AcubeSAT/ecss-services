#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterStatisticsService.hpp"
#include "Services/Parameter.hpp"

void ParameterStatisticsService :: reportParameterStatistics(Message& resetFlag) {

	Message statisticsReport(ParameterStatisticsService::ServiceType,
	                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);

	ErrorHandler::assertRequest(resetFlag.packetType == Message::TC, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.messageType ==ParameterStatisticsService::MessageType::ReportParameterStatistics,
	                            resetFlag, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.serviceType == ParameterStatisticsService::ServiceType, resetFlag,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	bool resetFlagValue = resetFlag.readBoolean();
	uint16_t numOfParameters = systemParameters.parametersArray.size();

	// Here is the end time
	// append start time
	// append end time

	statisticsReport.appendUint16(numOfParameters);

	for (uint16_t i = 0; i < numOfParameters; i++) {

		 uint16_t currId = i;
		 float meanVal = 0;
		 float sdVal = 0;

		uint16_t numOfSamples = ParameterStatisticsService::parameterStatisticsVector.at(currId).get().numOfSamplesCounter;
		if (numOfSamples == 0) {
			continue;
		}
		statisticsReport.appendUint16(currId);
		statisticsReport.appendUint16(numOfSamples);

		uint16_t type = ParameterStatisticsService::parameterStatisticsVector.at(i).get().type;
		switch (type) {
			case 0: {
				static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				    .calculateStatistics();
				uint8_t maxVal =
				    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .max;
				uint8_t minVal =
				    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .min;
				meanVal =
				    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .mean;
				sdVal =
				    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .standardDeviation;
				statisticsReport.appendUint8(maxVal);
//				statisticsReport.appendUint16(maxTime);
				statisticsReport.appendUint8(minVal);
//				statisticsReport.appendUint16(minTime);
				break;
			}
			case 1: {
				static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				    .calculateStatistics();
				uint16_t maxVal =
				    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .max;
				uint16_t minVal =
				    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .min;
				meanVal =
				    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .mean;
				sdVal =
				    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .standardDeviation;
				statisticsReport.appendUint16(maxVal);
//				statisticsReport.appendUint16(maxTime);
				statisticsReport.appendUint16(minVal);
//				statisticsReport.appendUint16(minTime);
				break;
			}
			case 2: {
				static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				    .calculateStatistics();
				uint32_t maxVal =
				    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .max;
				uint32_t minVal =
				    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .min;
				meanVal =
				    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .mean;
				sdVal =
				    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
				        .standardDeviation;
				statisticsReport.appendUint32(maxVal);
//				statisticsReport.appendUint16(maxTime);
				statisticsReport.appendUint32(minVal);
//				statisticsReport.appendUint16(minTime);
				break;
			}
			default:
				//TODO: add error, might not even needed.
				break;
		}

		statisticsReport.appendFloat(meanVal);
		statisticsReport.appendFloat(sdVal);

	}

	storeMessage(statisticsReport);

	if (resetFlagValue or ParameterStatisticsService :: hasAutomaticStatisticsReset) {
		Message resetParams(ParameterStatisticsService::ServiceType,
		                    ParameterStatisticsService::MessageType::ResetParameterStatistics,Message::TC,1);
		resetParameterStatistics(resetParams);
	}

	// Here add start time

}

void ParameterStatisticsService :: resetParameterStatistics(Message& reset) {

	ErrorHandler::assertRequest(reset.packetType == Message::TC, reset,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(reset.messageType == ParameterStatisticsService::MessageType::ResetParameterStatistics,
	                            reset, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(reset.serviceType == ParameterStatisticsService::ServiceType, reset,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	bool resetSignal = reset.readBoolean();

	if (resetSignal) {

		//TODO: Stop the evaluation of parameter statistics
		uint16_t numOfParameters = systemParameters.parametersArray.size();
		for(int i = 0; i < numOfParameters; i++) {
			//Will this clear the right Statistic's samples? Because its StatisticBase but vector contains
			// Statistic
			ParameterStatisticsService::parameterStatisticsVector.at(i).get().clearStatisticSamples();

		}
		//TODO: Restart the evaluation of parameter statistics
	}

}

void ParameterStatisticsService :: enablePeriodicStatisticsReporting(Message& request) {

	// Dummy value
	uint16_t SAMPLING_PARAMETER_INTERVAL = 5; //The sampling interval of each parameter, "timeInterval" requested should
	                                          //not exceed it. It has to be defined as a constant.

	uint16_t timeInterval = request.readUint16();

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	// Added error handler to check if the time interval asked is not a valid number.
	ErrorHandler::assertRequest(timeInterval >= SAMPLING_PARAMETER_INTERVAL, request,
	                            ErrorHandler::ExecutionStartErrorType::InvalidReportingRateError);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = true; //Periodic reporting status changes to enabled
	ParameterStatisticsService :: periodicStatisticsReportingInterval = timeInterval;

	uint16_t numOfParameters = systemParameters.parametersArray.size();

	//Only generate ONE parameter statistics report after every interval passes.
	while (ParameterStatisticsService :: periodicStatisticsReportingStatus) {

		for (int i = 0; i < numOfParameters; i++) {

			Message statisticsReport(ParameterStatisticsService::ServiceType,
			                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);
			/*
			 * TODO:
			 *      1. append start time to parameterReport
			 *      2. append end time
			 */
			uint16_t numOfSamples = ParameterStatisticsService::parameterStatisticsVector.at(i).get()
			                            .numOfSamplesCounter;
			if(numOfSamples == 0) {
				continue;
			}
			statisticsReport.appendUint16(numOfParameters);  // step 3
			statisticsReport.appendUint16(i);

			float sdVal = 0;
			float meanVal = 0;

			uint16_t type = ParameterStatisticsService::parameterStatisticsVector.at(i).get().type;
			switch (type) {
				case 0: {
					static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					    .calculateStatistics();
					uint8_t maxVal =
					    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .max;
					uint8_t minVal =
					    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .min;
					meanVal =
					    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .mean;
					sdVal =
					    static_cast<Statistic<uint8_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .standardDeviation;
					statisticsReport.appendUint8(maxVal);
					//				statisticsReport.appendUint16(maxTime);
					statisticsReport.appendUint8(minVal);
					//				statisticsReport.appendUint16(minTime);
					break;
				}
				case 1: {
					static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					    .calculateStatistics();
					uint16_t maxVal =
					    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .max;
					uint16_t minVal =
					    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .min;
					meanVal =
					    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .mean;
					sdVal =
					    static_cast<Statistic<uint16_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .standardDeviation;
					statisticsReport.appendUint16(maxVal);
					//				statisticsReport.appendUint16(maxTime);
					statisticsReport.appendUint16(minVal);
					//				statisticsReport.appendUint16(minTime);
					break;
				}
				case 2: {
					static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					    .calculateStatistics();
					uint32_t maxVal =
					    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .max;
					uint32_t minVal =
					    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .min;
					meanVal =
					    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .mean;
					sdVal =
					    static_cast<Statistic<uint32_t>>(ParameterStatisticsService::parameterStatisticsVector.at(i).get())
					        .standardDeviation;
					statisticsReport.appendUint32(maxVal);
					//				statisticsReport.appendUint16(maxTime);
					statisticsReport.appendUint32(minVal);
					//				statisticsReport.appendUint16(minTime);
					break;
				}
				default:
					//TODO: add error, might not even needed.
					break;
			}

			statisticsReport.appendFloat(meanVal);
			statisticsReport.appendFloat(sdVal);

			/*
			 * TODO: put the message into a queue and continue constructing the next report, and when
			 *      it's ready put that in the queue as well, another FreeRTOS task will be accountable of
			 *      keeping track of time, and when the interval passes, it's gonna pop the next
			 *      reportMessage from the queue and report it.
			 */
		}

		//TODO: systematically reset the parameters' statistics.

	}
}

void ParameterStatisticsService :: disablePeriodicStatisticsReporting(Message& request) {

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == ParameterStatisticsService::MessageType::DisablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = false;
	ParameterStatisticsService :: periodicStatisticsReportingInterval = 0;
}

void ParameterStatisticsService :: addOrUpdateStatisticsDefinitions(Message& paramIds) {

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions,
	                            paramIds, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterStatisticsService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();

	uint16_t step = -1;
	(paramIds.hasTimeIntervals) ? (step = 2) : (step = 1);  //if there are intervals we have to iterate with step 2.

	for (uint16_t i = 0; i < numOfIds; i += step) {

		uint16_t currentId = paramIds.readUint16();

		if (currentId < systemParameters.parametersArray.size()) {
			ErrorHandler::assertRequest(ParameterStatisticsService::numOfStatisticsDefinitions < MAX_NUM_OF_DEFINITIONS, paramIds,
			                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

			// If there are intervals, get the value and check if it exceeds the sampling rate of the parameter.
			if (paramIds.hasTimeIntervals) {

				uint16_t interval = paramIds.readUint16();
				ErrorHandler::assertRequest(interval <=ParameterStatisticsService::periodicStatisticsReportingInterval, paramIds,
				                            ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
				// Get the sampling interval of the current parameter from the statistics vector
				uint16_t paramSamplingInterval = ParameterStatisticsService::parameterStatisticsVector.at(currentId).get().selfSamplingInterval;
				if (paramSamplingInterval == 0) {
					ParameterStatisticsService::parameterStatisticsVector.at(currentId).get().selfSamplingInterval =
					    interval;
					ParameterStatisticsService::nonDefinedStatistics--;
					//TODO: start the evaluation of statistics for this parameter.
				}
				else {
					ParameterStatisticsService::parameterStatisticsVector.at(currentId).get().selfSamplingInterval =
					    interval;
					// Statistics evaluation reset
					ParameterStatisticsService::parameterStatisticsVector.at(currentId).get().clearStatisticSamples();
				}
			}
		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}
}

void ParameterStatisticsService :: deleteStatisticsDefinitions(Message& paramIds) {

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterStatisticsService::MessageType::DeleteParameterStatisticsDefinitions,
	                            paramIds, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterStatisticsService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();

	// In case that not all parameter definitions have to be deleted
	if (numOfIds < systemParameters.parametersArray.size()) {
		for (uint16_t i = 0; i < numOfIds; i++) {

			uint16_t currentId = paramIds.readUint16();
			if (currentId < systemParameters.parametersArray.size()) {
				// Removing the parameter definitions
				ParameterStatisticsService::parameterStatisticsVector.at(currentId).get().selfSamplingInterval = 0;
				ParameterStatisticsService::nonDefinedStatistics++;
			} else {
				ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
			}
		}
	}// In case that we request to delete every parameter definition
	else if (numOfIds == systemParameters.parametersArray.size()) {
		for (uint16_t i = 0; i < numOfIds; i++) {
			ParameterStatisticsService::parameterStatisticsVector.at(i).get().selfSamplingInterval = 0;
		}
		ParameterStatisticsService::nonDefinedStatistics = systemParameters.parametersArray.size();
	}

	// If list of definitions is empty, stop the periodic reporting.
	if (ParameterStatisticsService::nonDefinedStatistics == systemParameters.parametersArray.size()) {
		ParameterStatisticsService::periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService :: reportStatisticsDefinitions(Message& request) {

	Message definitionsReport(ParameterStatisticsService::ServiceType,
	                        ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport, Message::TM, 1);

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType ==ParameterStatisticsService::MessageType::ReportParameterStatisticsDefinitions,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t reportingInterval = 0;
	if (ParameterStatisticsService :: periodicStatisticsReportingStatus) {
		reportingInterval = ParameterStatisticsService :: periodicStatisticsReportingInterval;
	}

	uint16_t numOfParameters = systemParameters.parametersArray.size();
	definitionsReport.appendUint16(reportingInterval);  // Append interval

	uint16_t numOfDefinedParameters = 0;
	for (int i = 0; i < numOfParameters; i++) {
		uint16_t currentId = i;
		uint16_t currentSamplingInterval = ParameterStatisticsService::parameterStatisticsVector.at(currentId).get()
		                                .selfSamplingInterval;
		if (currentSamplingInterval != 0) {
			numOfDefinedParameters++;
		}
	}

	definitionsReport.appendUint16(numOfDefinedParameters);

	for (int i = 0; i < numOfParameters; i++) {
		uint16_t currentId = i;
		uint16_t samplingInterval = ParameterStatisticsService::parameterStatisticsVector.at(currentId).get()
		                                       .selfSamplingInterval;
		if (samplingInterval != 0) {
			definitionsReport.appendUint16(currentId);
			definitionsReport.appendUint16(samplingInterval);
		}
	}

	storeMessage(definitionsReport);

}

#endif