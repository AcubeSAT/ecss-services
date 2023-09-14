#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER
#include "ServicePool.hpp"
#include "Services/ParameterStatisticsService.hpp"

ParameterStatisticsService::ParameterStatisticsService() : evaluationStartTime(TimeGetter::getCurrentTimeDefaultCUC()) {
	initializeStatisticsMap();
	serviceType = ServiceType;
}

void ParameterStatisticsService::reportParameterStatistics(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportParameterStatistics)) {
		return;
	}
	parameterStatisticsReport();

	if (hasAutomaticStatisticsReset or request.readBoolean()) {
		resetParameterStatistics();
	}
}

void ParameterStatisticsService::reportParameterStatistics(bool reset) {
	parameterStatisticsReport();

	if (hasAutomaticStatisticsReset or reset) {
		resetParameterStatistics();
	}
}

void ParameterStatisticsService::parameterStatisticsReport() {
	Message report = createTM(ParameterStatisticsReport);
	report.append(evaluationStartTime);
	auto evaluationStopTime = TimeGetter::getCurrentTimeDefaultCUC();
	report.append(evaluationStopTime);

	uint16_t numOfValidParameters = 0;
	for (auto& currentStatistic: statisticsMap) {
		ParameterSampleCount numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		numOfValidParameters++;
	}
	report.appendUint16(numOfValidParameters);

	for (auto& currentStatistic: statisticsMap) {
		ParameterId currentId = currentStatistic.first;
		ParameterSampleCount numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		report.append(currentId);
		report.appendUint16(numOfSamples);
		currentStatistic.second.appendStatisticsToMessage(report);
	}
	storeMessage(report);
}

void ParameterStatisticsService::resetParameterStatistics(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ResetParameterStatistics)) {
		return;
	}
	resetParameterStatistics();
}

void ParameterStatisticsService::resetParameterStatistics() {
	for (auto& it: statisticsMap) {
		it.second.resetStatistics();
	}
	evaluationStartTime = TimeGetter::getCurrentTimeDefaultCUC();
}

void ParameterStatisticsService::enablePeriodicStatisticsReporting(Message& request) {
	/**
	 * @todo: The sampling interval of each parameter. the "timeInterval" requested should not exceed it.
	 * 		  It has to be defined as a constant.
	 */
	SamplingInterval SAMPLING_PARAMETER_INTERVAL = 5;

	if (!request.assertTC(ServiceType, MessageType::EnablePeriodicParameterReporting)) {
		return;
	}

	SamplingInterval timeInterval = request.read<SamplingInterval>();
	if (timeInterval < SAMPLING_PARAMETER_INTERVAL) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
		return;
	}
	periodicStatisticsReportingStatus = true;
	reportingIntervalMs = timeInterval;
}

void ParameterStatisticsService::disablePeriodicStatisticsReporting(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DisablePeriodicParameterReporting)) {
		return;
	}

	periodicStatisticsReportingStatus = false;
	reportingIntervalMs = 0;
}

void ParameterStatisticsService::addOrUpdateStatisticsDefinitions(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::AddOrUpdateParameterStatisticsDefinitions)) {
		return;
	}

	uint16_t numOfIds = request.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		ParameterId currentId = request.read<ParameterId>();
		if (!Services.parameterManagement.parameterExists(currentId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
			if (supportsSamplingInterval) {
				request.skipBytes(2);
			}
			continue;
		}
		bool exists = statisticsMap.find(currentId) != statisticsMap.end();
		SamplingInterval interval = 0;
		if (supportsSamplingInterval) {
			interval = request.read<SamplingInterval>();
			if (interval < reportingIntervalMs) {
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
	if (!request.assertTC(ServiceType, MessageType::DeleteParameterStatisticsDefinitions)) {
		return;
	}

	uint16_t numOfIds = request.readUint16();
	if (numOfIds == 0) {
		statisticsMap.clear();
		periodicStatisticsReportingStatus = false;
		return;
	}
	for (uint16_t i = 0; i < numOfIds; i++) {
		ParameterId currentId = request.read<ParameterId>();
		if (!Services.parameterManagement.parameterExists(currentId)) {
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
	if (!request.assertTC(ServiceType, MessageType::ReportParameterStatisticsDefinitions)) {
		return;
	}
	statisticsDefinitionsReport();
}

void ParameterStatisticsService::statisticsDefinitionsReport() {
	Message definitionsReport = createTM(ParameterStatisticsDefinitionsReport);

	SamplingInterval currentReportingIntervalMs = 0;
	if (periodicStatisticsReportingStatus) {
		currentReportingIntervalMs = reportingIntervalMs;
	}
	definitionsReport.append(currentReportingIntervalMs);
	definitionsReport.appendUint16(statisticsMap.size());

	for (auto& currentParam: statisticsMap) {
		ParameterId currentId = currentParam.first;
		SamplingInterval samplingInterval = currentParam.second.selfSamplingInterval;
		definitionsReport.append(currentId);
		if (supportsSamplingInterval) {
			definitionsReport.append(samplingInterval);
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
