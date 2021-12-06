#include <iostream>
#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
#include "Services/ParameterStatisticsService.hpp"

void ParameterStatisticsService::reportParameterStatistics(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportParameterStatistics);
	parameterStatisticsReport();

	// TODO: append start time and end time to the report

	if (hasAutomaticStatisticsReset) {
		resetParameterStatistics();
	} else {
		bool resetFlagValue = request.readBoolean();
		if (resetFlagValue) {
			resetParameterStatistics();
		}
	}
}

void ParameterStatisticsService::parameterStatisticsReport() {
	Message report(ServiceType, MessageType::ParameterStatisticsReport, Message::TM, 1);
	report.appendUint16(1); // Dummy value for start and end time, will change in the end
	report.appendUint16(1);
	uint16_t numOfValidParameters = 0;

	for (auto& currentStatistic : statisticsMap) {
		uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		numOfValidParameters++;
	}
	report.appendUint16(numOfValidParameters);

	for (auto& currentStatistic : statisticsMap) {
		uint16_t currentId = currentStatistic.first;
		uint16_t numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		report.appendUint16(currentId);
		report.appendUint16(numOfSamples);
		currentStatistic.second.appendStatisticsToMessage(report);
	}
	storeMessage(report);
}

void ParameterStatisticsService::resetParameterStatistics(Message& request) {
	request.assertTC(ServiceType, MessageType::ResetParameterStatistics);
	resetParameterStatistics();
}

void ParameterStatisticsService::resetParameterStatistics() {
	// TODO: Stop the evaluation of parameter statistics
	for (auto& it : statisticsMap) {
		it.second.resetStatistics();
	}
	// TODO: Restart the evaluation of parameter statistics
}

void ParameterStatisticsService::enablePeriodicStatisticsReporting(Message& request) {
	/**
	 * @todo: The sampling interval of each parameter. the "timeInterval" requested should not exceed it.
	 * 		  It has to be defined as a constant.
	 */
	uint16_t SAMPLING_PARAMETER_INTERVAL = 5;

	request.assertTC(ServiceType, MessageType::EnablePeriodicParameterReporting);

	uint16_t timeInterval = request.readUint16();
	if (timeInterval < SAMPLING_PARAMETER_INTERVAL) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
		return;
	}
	periodicStatisticsReportingStatus = true;
	reportingInterval = timeInterval;
}

void ParameterStatisticsService::disablePeriodicStatisticsReporting(Message& request) {
	request.assertTC(ServiceType, MessageType::DisablePeriodicParameterReporting);

	periodicStatisticsReportingStatus = false;
	reportingInterval = 0;
}

void ParameterStatisticsService::addOrUpdateStatisticsDefinitions(Message& request) {
	request.assertTC(ServiceType, MessageType::AddOrUpdateParameterStatisticsDefinitions);

	uint16_t numOfIds = request.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currentId = request.readUint16();
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
			if (supportsSamplingInterval) {
				request.skipBytes(2);
			}
			continue;
		}
		bool exists = statisticsMap.find(currentId) != statisticsMap.end();
		uint16_t interval = 0;
		if (supportsSamplingInterval) {
			interval = request.readUint16();
			if (interval < reportingInterval) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
				continue;
			}
		}
		if (not exists) {
			if (statisticsMap.size() >= ECSSMaxStatisticParameters) {
				ErrorHandler::reportError(request,
				                          ErrorHandler::ExecutionStartErrorType::MaxStatisticDefinitionsReached);
				return;
			}
			Statistic newStatistic;
			if (supportsSamplingInterval) {
				newStatistic.setSelfSamplingInterval(interval);
			}
			statisticsMap.insert({currentId, newStatistic});
			// TODO: start the evaluation of statistics for this parameter.
		} else {
			if (supportsSamplingInterval) {
				statisticsMap.at(currentId).setSelfSamplingInterval(interval);
			}
			statisticsMap.at(currentId).resetStatistics();
		}
	}
}

void ParameterStatisticsService::deleteStatisticsDefinitions(Message& request) {
	request.assertTC(ServiceType, MessageType::DeleteParameterStatisticsDefinitions);

	uint16_t numOfIds = request.readUint16();
	if (numOfIds == 0) {
		statisticsMap.clear();
		periodicStatisticsReportingStatus = false;
		return;
	}
	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currentId = request.readUint16();
		if (currentId >= systemParameters.parametersArray.size()) {
			ErrorHandler::reportError(request, ErrorHandler::GetNonExistingParameter);
			continue;
		}
		statisticsMap.erase(currentId);
	}
	if (statisticsMap.empty()) {
		periodicStatisticsReportingStatus = false;
	}
}

void ParameterStatisticsService::reportStatisticsDefinitions(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportParameterStatisticsDefinitions);
	statisticsDefinitionsReport();
}

void ParameterStatisticsService::statisticsDefinitionsReport() {
	Message definitionsReport(ServiceType, MessageType::ParameterStatisticsDefinitionsReport, Message::TM, 1);

	uint16_t currentReportingInterval = 0;
	if (periodicStatisticsReportingStatus) {
		currentReportingInterval = reportingInterval;
	}
	definitionsReport.appendUint16(currentReportingInterval);
	definitionsReport.appendUint16(statisticsMap.size());

	for (auto& currentParam : statisticsMap) {
		uint16_t currentId = currentParam.first;
		uint16_t samplingInterval = currentParam.second.selfSamplingInterval;
		definitionsReport.appendUint16(currentId);
		if (supportsSamplingInterval) {
			definitionsReport.appendUint16(samplingInterval);
		}
	}
	storeMessage(definitionsReport);
}

void ParameterStatisticsService::execute(Message& message) {
	switch (message.messageType) {
		case ReportParameterStatistics:
			reportParameterStatistics(message);
			break;
		case ResetParameterStatistics:
			resetParameterStatistics(message);
			break;
		case EnablePeriodicParameterReporting:
			enablePeriodicStatisticsReporting(message);
			break;
		case DisablePeriodicParameterReporting:
			disablePeriodicStatisticsReporting(message);
			break;
		case AddOrUpdateParameterStatisticsDefinitions:
			addOrUpdateStatisticsDefinitions(message);
			break;
		case DeleteParameterStatisticsDefinitions:
			deleteStatisticsDefinitions(message);
			break;
		case ReportParameterStatisticsDefinitions:
			reportStatisticsDefinitions(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif
