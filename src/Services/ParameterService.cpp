#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Helpers/Parameter.hpp"


void ParameterService::reportParameters(Message& paramIds) {
	// TM[20,2]
	Message parameterReport(ParameterService::ServiceType, ParameterService::MessageType::ParameterValuesReport,
	                        Message::TM, 1);

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ParameterService::MessageType::ReportParameterValues, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterService::ServiceType, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();
	uint16_t numberOfValidIds = 0;
	for (uint16_t i = 0; i < numOfIds; i++) {
		if (parameterExists(paramIds.readUint16())) {
			numberOfValidIds++;
		}
	}
	parameterReport.appendUint16(numberOfValidIds);
	paramIds.resetRead();

	numOfIds = paramIds.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = paramIds.readUint16();
		if (parameterExists(currId)) {
			parameterReport.appendUint16(currId);
			parameters.at(currId).get().appendValueToMessage(parameterReport);
		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}

	storeMessage(parameterReport);
}

void ParameterService::setParameters(Message& newParamValues) {
	ErrorHandler::assertRequest(newParamValues.packetType == Message::TC, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.messageType == ParameterService::MessageType::SetParameterValues,
	                            newParamValues, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.serviceType == ParameterService::ServiceType, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = newParamValues.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();
		if (parameterExists(currId)) {
			parameters.at(currId).get().setValueFromMessage(newParamValues);
		} else {
			ErrorHandler::reportError(newParamValues, ErrorHandler::SetNonExistingParameter);
			break; // Setting next parameters is impossible, since the size of value to be read is unknown
		}
	}
}

void ParameterService::execute(Message& message) {
	switch (message.messageType) {
		case ReportParameterValues:
			reportParameters(message);
			break;
		case SetParameterValues:
			setParameters(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
