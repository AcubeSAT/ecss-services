#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterStatisticsService.hpp"
#include "Services/Parameter.hpp"

void ParameterStatisticsService :: reportParameterStatistics(Message& resetFlag) {

	Message parameterReport(ParameterStatisticsService::ServiceType,
	                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);

	ErrorHandler::assertRequest(resetFlag.packetType == Message::TC, resetFlag,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.messageType ==ParameterStatisticsService::MessageType::ReportParameterStatistics,
	                            resetFlag, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(resetFlag.serviceType == ParameterStatisticsService::ServiceType, resetFlag,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	bool resetFlagValue = resetFlag.readBoolean();
	uint16_t numOfParameters = systemParameters.parametersArray.size();

	if (resetFlagValue) {

		for (uint16_t i = 0; i < numOfParameters; i++) {

			// uint16_t currId = i;

			/*
			 * TODO:
			 *      1. append start time to parameterReport
			 *      2. append end time
			 *      3. append numOfParameters (N)
			 *      4. append N times:
			 *          a. ID of parameter
			 *          b. number of samples
			 *          c. max value
			 *          d. max time             <--- The samples will be given by another task, which takes them from
			 *          e. min value                                                                      the sensors
			 *          f. min time
			 *          g. mean value
			 *          h. standard deviation (optional)
			 */
		}
	}

	storeMessage(parameterReport);

	if (ParameterStatisticsService :: hasAutomaticStatisticsReset) {
		Message resetParams(ParameterStatisticsService::ServiceType,
		                    ParameterStatisticsService::MessageType::ResetParameterStatistics,Message::TC,1);
		resetParameterStatistics(resetParams);
	}

}

void ParameterStatisticsService :: resetParameterStatistics(Message& reset) {

	ErrorHandler::assertRequest(reset.packetType == Message::TC, reset,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(reset.messageType == ParameterStatisticsService::MessageType::ResetParameterStatistics,
	                            reset, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(reset.serviceType == ParameterStatisticsService::ServiceType, reset,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	bool resetSignal = reset.readBoolean();

	if (resetSignal) {
		/*
		 * TODO:
		 *      1. Stop the evaluation of parameter statistics
		 *      2. Clear all accumulated results
		 *      3. Restart the evaluation process
		 */
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

		Message parameterReport(ParameterStatisticsService::ServiceType,
		                        ParameterStatisticsService::MessageType::ParameterStatisticsReport, Message::TM, 1);
		/*
		 * TODO:
		 *      1. append start time to parameterReport
		 *      2. append end time
		 *      3. append numOfParameters (N = 1)
		 */
				parameterReport.appendUint16(numOfParameters);  // step 3

		/*      4. append 1 time:
		 *          a. ID of parameter
		 *          b. number of samples
		 *          c. max value
		 *          d. max time
		 *          e. min value
		 *          f. min time
		 *          g. mean value
		 *          h. standard deviation (optional)
	     */

		//TODO: systematically reset the parameters' statistics.

		storeMessage(parameterReport);
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

	// Dummy value
	uint16_t SAMPLING_RATE = 4; // the sampling rate for every parameter. Has to be defined.

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterStatisticsService::MessageType::AddOrUpdateParameterStatisticsDefinitions,
	                            paramIds, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterStatisticsService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();

	uint16_t step = -1;
	(paramIds.hasTimeIntervals) ? (step = 2) : (step = 1);  //if there are intervals we have to iterate with step 2.

	for (uint16_t i = 0; i < numOfIds; i+=step) {

		uint16_t currentId = paramIds.readUint16();

		if (currentId < systemParameters.parametersArray.size()) {
			ErrorHandler::assertRequest(ParameterStatisticsService::numOfStatisticsDefinitions < MAX_NUM_OF_DEFINITIONS, paramIds,
			                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
			// If there are intervals, get the value and check if it exceeds the sampling rate of the parameter.
			if (paramIds.hasTimeIntervals) {
				uint16_t interval = paramIds.readUint16();
				ErrorHandler::assertRequest(interval >= SAMPLING_RATE, paramIds,
				                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
				/*
				 * TODO:
				 *      1. If no definition exists for that parameter:
				 *          a. add the parameter statistics definition to the list of evaluated parameters
				 *          b. start the evaluation of the statistics for that parameter
				 *      2. if a parameter statistics definition exists for that parameter:
				 *          a. update  the  sampling  interval  of  that  parameter  statistics definition
				 *          b. restart the evaluation of the statistics for that parameter
				 */
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

	for (uint16_t i = 0; i < numOfIds; i++) {

		uint16_t currentId = paramIds.readUint16();
		if (currentId < systemParameters.parametersArray.size()) {

			/*
			 * TODO:
			 *      1. remove that parameter statistics definition from the list of evaluated parameters
			 */

		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}

	// TODO: "ParameterStatisticsService::periodicStatisticsReportingStatus = 0" if the list of evaluated parameters
	//       is empty after execution of all instructions.
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
		reportingInterval = ParameterStatisticsService ::periodicStatisticsReportingInterval;
	}

	uint16_t numOfParameters = systemParameters.parametersArray.size();
	definitionsReport.appendUint16(reportingInterval);  // Append interval
	definitionsReport.appendUint16(numOfParameters);    // Append N

	uint16_t samplingInterval = 0;  // Need to get this for every parameter.

	for (int i = 0; i < numOfParameters; i++) {
		definitionsReport.appendUint16(i);  // Append parameter ID
		definitionsReport.appendUint16(samplingInterval);   // Append sampling interval
	}

	storeMessage(definitionsReport);

}

#endif