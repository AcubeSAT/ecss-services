#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Helpers/Parameter.hpp"
#include "Services/ParameterService.hpp"


void ParameterService::reportParameters(Message& paramIds) {

	paramIds.assertTC(ServiceType, ReportParameterValues);
	Message parameterReport = createTM(ParameterValuesReport);

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
		if (auto parameter = getParameter(currId)) {
			parameterReport.appendUint16(currId);
			parameter->get().appendValueToMessage(parameterReport);
		} else {
			ErrorHandler::reportError(paramIds, ErrorHandler::GetNonExistingParameter);
		}
	}

	storeMessage(parameterReport);
}

void ParameterService::setParameters(Message& newParamValues) const {
	newParamValues.assertTC(ServiceType, MessageType::SetParameterValues);

	uint16_t numOfIds = newParamValues.readUint16();

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();
		if (auto parameter = getParameter(currId)) {
			parameter->get().setValueFromMessage(newParamValues);
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
