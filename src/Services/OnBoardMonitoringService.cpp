#include "ECSS_Configuration.hpp"
#ifdef SERVICE_ONBOARDMONITORING

#include <Message.hpp>
#include "Services/OnBoardMonitoringService.hpp"

void OnBoardMonitoringService::enableParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, EnableParameterMonitoringDefinitions);
}

void OnBoardMonitoringService::disableParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, DisableParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::changeMaximumTransitionReportingDelay(Message& message){
	message.assertTC(ServiceType, ChangeMaximumTransitionReportingDelay);

}

void OnBoardMonitoringService::deleteAllParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, DeleteAllParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::addParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, AddParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::deleteParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, DeleteParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::modifyParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, ModifyParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::reportParameterMonitoringDefinitions(Message& message){
	message.assertTC(ServiceType, ReportParameterMonitoringDefinitions);

}

void OnBoardMonitoringService::parameterMonitoringDefinitionReport(Message& message){
	message.assertTC(ServiceType, ParameterMonitoringDefinitionReport);

}

void OnBoardMonitoringService::reportOutOfLimits(Message& message){
	message.assertTC(ServiceType, ReportOutOfLimits);

}

void OnBoardMonitoringService::outOfLimitsReport(){

}

void OnBoardMonitoringService::checkTransitionReport(){

}

void OnBoardMonitoringService::reportStatusOfParameterMonitoringDefinition(Message& message){
	message.assertTC(ServiceType, ReportStatusOfParameterMonitoringDefinition);

}

void OnBoardMonitoringService::parameterMonitoringDefinitionStatusReport(){

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
		case AddParameterMonitoringDefinitions:
			addParameterMonitoringDefinitions(message);
			break;
		case DeleteParameterMonitoringDefinitions:
			deleteParameterMonitoringDefinitions(message);
			break;
		case ModifyParameterMonitoringDefinitions:
			modifyParameterMonitoringDefinitions(message);
			break;
		case ReportParameterMonitoringDefinitions:
			reportParameterMonitoringDefinitions(message);
			break;
		case ParameterMonitoringDefinitionReport:
			parameterMonitoringDefinitionReport(message);
			break;
		case ReportOutOfLimits:
			reportOutOfLimits(message);
			break;
		case OutOfLimitsReport:
			outOfLimitsReport();
			break;
		case CheckTransitionReport:
			checkTransitionReport();
			break;
		case ReportStatusOfParameterMonitoringDefinition:
			reportStatusOfParameterMonitoringDefinition(message);
			break;
		case ParameterMonitoringDefinitionStatusReport:
			parameterMonitoringDefinitionStatusReport();
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif