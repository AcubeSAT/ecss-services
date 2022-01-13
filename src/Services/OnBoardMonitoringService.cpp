#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING
#include <Message.hpp>
#include "Services/OnBoardMonitoringService.hpp"
#include "etl/map.h"


void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, EnableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = true;
	uint16_t numberOfParameters = message.readUint16();
	for (uint16_t i = 0; i < numberOfParameters; i++) {
		uint16_t currentId = message.readEnum16();
		if (ParameterMonitoringList.find(currentId) == ParameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
			break;
		}
		ParameterMonitoringList.at(currentId).get().repetitionNumber = 0;
		ParameterMonitoringList.at(currentId).get().monitoringStatus = true;
	}
}

void OnBoardMonitoringService::disableParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DisableParameterMonitoringDefinitions);
	parameterMonitoringFunctionStatus = false;
	uint16_t numberOfParameters = message.readUint16();
	for (uint16_t i = 0; i < numberOfParameters; i++) {
		uint16_t currentId = message.readEnum16();
		if (ParameterMonitoringList.find(currentId) == ParameterMonitoringList.end()) {
			ErrorHandler::reportError(
			    message, ErrorHandler::ExecutionStartErrorType::GetNonExistingParameterMonitoringDefinition);
		}
		ParameterMonitoringList.at(currentId).get().monitoringStatus = false;
		bool status = ParameterMonitoringList.at(currentId).get().monitoringStatus;
		ParameterMonitoringList.at(currentId).get().checkingStatus = PMONBase::Unchecked;
	}
}

void OnBoardMonitoringService::changeMaximumTransitionReportingDelay(Message& message) {
	message.assertTC(ServiceType, ChangeMaximumTransitionReportingDelay);
	maximumTransitionReportingDelay = message.readUint16();
}

void OnBoardMonitoringService::deleteAllParameterMonitoringDefinitions(Message& message) {
	message.assertTC(ServiceType, DeleteAllParameterMonitoringDefinitions);
	if (parameterMonitoringFunctionStatus) {
		ErrorHandler::reportError(
		    message,
		    ErrorHandler::ExecutionStartErrorType::InvalidRequestToDeleteAllParameterMonitoringDefinitionsError);
	} else {
		ParameterMonitoringList.clear();
	}
}


void OnBoardMonitoringService::execute(Message& message) {
	switch (message.messageType) {
		case EnableParameterMonitoringDefinitions:
			enableParameterMonitoringDefinitions(message);
			break;
		case DisableParameterMonitoringDefinitions:
			disableParameterMonitoringDefinitions(message);
			break;
		case ChangeMaximumTransitionReportingDelay:
			changeMaximumTransitionReportingDelay(message);
			break;
		case DeleteAllParameterMonitoringDefinitions:
			deleteAllParameterMonitoringDefinitions(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif