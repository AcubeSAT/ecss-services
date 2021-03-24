#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Services/Parameter.hpp"
#include "ECSS_ST_Definitions.hpp"

void ParameterService::reportParameters(Message& paramIds) {
	// TM[20,2]
	Message parameterReport(ParameterManagement, ParameterValueReport, Message::TM, 1);

	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == ReportParameterValues, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == ParameterManagement, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();
	uint16_t numberOfValidIds = 0;
	for (uint16_t i = 0; i < numOfIds; i++) {
		if (paramIds.readUint16() < systemParameters.parametersArray.size()) {
			numberOfValidIds++;
		}
	}
	parameterReport.appendUint16(numberOfValidIds);
	paramIds.resetRead();

	numOfIds = paramIds.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = paramIds.readUint16();
		if (currId < systemParameters.parametersArray.size()) {
			parameterReport.appendUint16(currId);
			systemParameters.parametersArray[currId].get().appendValueToMessage(parameterReport);
		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}

	storeMessage(parameterReport);
}

void ParameterService::setParameters(Message& newParamValues) {

	ErrorHandler::assertRequest(newParamValues.packetType == Message::TC, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.messageType == SetParameterValues, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.serviceType == ParameterManagement, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = newParamValues.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();
		if (currId < systemParameters.parametersArray.size()) {
			systemParameters.parametersArray[currId].get().setValueFromMessage(newParamValues);
		} else {
			ErrorHandler::reportError(newParamValues, ErrorHandler::SetNonExistingParameter);
			break; // Setting next parameters is impossible, since the size of value to be read is unknown
		}
	}
}

void ParameterService::execute(Message& message) {
	switch (message.messageType) {
		case 1:
			reportParameters(message); // TC[20,1]
			break;
		case 3:
			setParameters(message); // TC[20,3]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
