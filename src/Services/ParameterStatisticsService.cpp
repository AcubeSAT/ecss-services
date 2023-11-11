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
	for (const auto& currentStatistic: statisticsMap) {
		const ParameterSampleCount numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		numOfValidParameters++;
	}
	report.appendUint16(numOfValidParameters);

	for (auto& currentStatistic: statisticsMap) {
		const ParameterId currentId = currentStatistic.first;
		const ParameterSampleCount numOfSamples = currentStatistic.second.sampleCounter;
		if (numOfSamples == 0) {
			continue;
		}
		report.append<ParameterId>(currentId);
		report.append<ParameterSampleCount>(numOfSamples);
		currentStatistic.second.appendStatisticsToMessage(report);
	}
	storeMessage(report);
}

void ParameterStatisticsService::resetParameterStatistics(const Message& request) {
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
	Time::RelativeTime constexpr SamplingParameterInterval = 5;

	if (!request.assertTC(ServiceType, MessageType::EnablePeriodicParameterReporting)) {
		return;
	}

	const SamplingInterval timeInterval = request.readUint16();

	if (timeInterval < SamplingParameterInterval) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidSamplingRateError);
		return;
	}

	periodicStatisticsReportingStatus = true;
	reportingIntervalMs = timeInterval;
}

void ParameterStatisticsService::disablePeriodicStatisticsReporting(const Message& request) {
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

	uint16_t const numOfIds = request.readUint16();
	for (uint16_t i = 0; i < numOfIds; i++) {
		const ParameterId currentId = request.read<ParameterId>();
		if (!Services.parameterManagement.parameterExists(currentId)) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingParameter);
			if (supportsSamplingInterval) {
				request.skipBytes(sizeof(SamplingInterval));
			}
			continue;
		}
		bool const exists = statisticsMap.find(currentId) != statisticsMap.end();
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
			// TODO(athanasios): start the evaluation of statistics for this parameter.
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

	uint16_t const numOfIds = request.readUint16();
	if (numOfIds == 0) {
		statisticsMap.clear();
		periodicStatisticsReportingStatus = false;
		return;
	}
	for (uint16_t i = 0; i < numOfIds; i++) {
		const ParameterId currentId = request.read<ParameterId>();
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

void ParameterStatisticsService::reportStatisticsDefinitions(const Message& request) {
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
	definitionsReport.append<SamplingInterval>(currentReportingIntervalMs);
	definitionsReport.appendUint16(statisticsMap.size());

	for (const auto& currentParam: statisticsMap) {
		const ParameterId currentId = currentParam.first;
		const SamplingInterval samplingInterval = currentParam.second.selfSamplingInterval;
		definitionsReport.append<ParameterId>(currentId);
		if (supportsSamplingInterval) {
			definitionsReport.append<SamplingInterval>(samplingInterval);
		}
	}
	storeMessage(definitionsReport);
}

void ParameterStatisticsService::execute(const Message& message) {
	DefaultTimestamp currentTime;
	switch (message.messageType) {
		case ReportParameterStatistics:
			reportParameterStatistics(message);
			break;
		case ResetParameterStatistics:
			resetParameterStatistics(message);
			currentTime = getCurrentTime();
			break;
		case EnablePeriodicParameterReporting:
			currentTime = getCurrentTime();
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
			currentTime = getCurrentTime();
			reportStatisticsDefinitions(message);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

ParameterStatisticsService::DefaultTimestamp ParameterStatisticsService::getCurrentTime() {
	return TimeGetter::getCurrentTimeDefaultCUC();
}

#endif
