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
	int numOfParameters = systemParameters.parametersArray.size();

	if (resetFlagValue) {

		for (uint16_t i = 0; i < numOfParameters; i++) {

			uint16_t currId = i;
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

	uint16_t SAMPLING_PARAMETER_INTERVAL; // The sampling interval of each parameter, "timeInterval" requested should
	                                      // not exceed it. It has to be defined as a constant.

	uint16_t timeInterval = request.readUint16();

	ErrorHandler::assertRequest(request.packetType == Message::TC, request,ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == ParameterStatisticsService::MessageType::EnablePeriodicParameterReporting,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ParameterStatisticsService::ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	// Added error handler to check if the time interval asked is not a valid number.
	ErrorHandler::assertRequest(timeInterval >= SAMPLING_PARAMETER_INTERVAL, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	ParameterStatisticsService :: periodicStatisticsReportingStatus = 1;    //Periodic reporting status changes to enabled
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
		 *      4. append 1 time:
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

	ParameterStatisticsService :: periodicStatisticsReportingStatus = 0;
	ParameterStatisticsService :: periodicStatisticsReportingInterval = 0;
}

#endif