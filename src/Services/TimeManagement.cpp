#include "ECSS_Configuration.hpp"
#include "Services/TimeManagement.hpp"
#include <cmath>

void TimeManagementService :: setTimerReportGenerationRate(Message& rateExponentialValue) {

	ErrorHandler::assertRequest(rateExponentialValue.packetType == Message::TC, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.messageType == TimeManagementService::MessageType::SetTimeReportGenerationRate,
	                            rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(rateExponentialValue.serviceType == TimeManagementService::ServiceType, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t newRate = rateExponentialValue.readUint16();

	// Check overflow as described in ECSS Datasheet
	ErrorHandler::assertRequest(newRate <= 8, rateExponentialValue,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	newRate = (uint16_t) pow(2, newRate);

	TimeManagementService :: timeReportGenerationRate = newRate;
}

void TimeManagementService :: cucTimeReport(Message& timeStampMs) {

	Message cucReport(TimeManagementService::ServiceType, TimeManagementService::MessageType::CucTimeReport,
	                  Message::TM, 1);

	ErrorHandler::assertRequest(timeStampMs.packetType == Message::TC, timeStampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timeStampMs.messageType == TimeManagementService::MessageType::CucTimeReport,
	                            timeStampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(timeStampMs.serviceType == TimeManagementService::ServiceType, timeStampMs,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	cucReport.appendUint16(log2(TimeManagementService::timeReportGenerationRate));

	//TODO: convert the timeStampMs to CUC format and append it to the report message.
}
