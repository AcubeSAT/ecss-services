#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Services/Parameter.hpp"

void ParameterService::addToParameterArray(uint16_t id, ParameterBase& param) {
	systemParameters.parametersArray[id] = param;
}

void ParameterService::reportParameters(Message& paramIds) {
	// TM[20,2]
	Message reqParam(20, 2, Message::TM, 1);

	paramIds.resetRead();
	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == 1, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == 20, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();
	uint16_t validIds = 0;

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = paramIds.readUint16();
		if (currId < ECSS_ST_20_MAX_PARAMETERS) {
			reqParam.appendUint16(currId);
			reqParam.appendString(systemParameters.parametersArray[currId].get().getValueAsString());
			validIds++;
		}
	}
	reqParam.appendUint16(validIds);

	storeMessage(reqParam);
}

void ParameterService::setParameters(Message& newParamValues) {

	ErrorHandler::assertRequest(newParamValues.packetType == Message::TC, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.messageType == 3, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.serviceType == 20, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = newParamValues.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();
		if (currId < ECSS_ST_20_MAX_PARAMETERS) {
			systemParameters.parametersArray[currId].get().setValueFromMessage(newParamValues);
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
