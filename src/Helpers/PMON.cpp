#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"
#include "Services/EventReportService.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),
      checkType(checkType),
      monitoredParameter(*Services.parameterManagement.getParameter(monitoredParameterId)) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}

void PMON::updatePMONAfterPerformCheck(const CheckingStatus newStatus) {
	if (newTrackedCheckingStatus == newStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
		newTrackedCheckingStatus = newStatus;
	}

	if (repetitionCounter >= repetitionNumber) {
		const etl::pair<CheckingStatus, CheckingStatus> transition = etl::make_pair(currentCheckingStatus, newTrackedCheckingStatus);
		checkTransitionList.insert(checkTransitionList.begin(), transition);
		currentCheckingStatus = newTrackedCheckingStatus;
		newTrackedCheckingStatus = Unchecked;


		if (pmonTransitionEventMap.find(transition) == pmonTransitionEventMap.end()) {
			return;
		}

		EventDefinitionId eventID = pmonTransitionEventMap[transition];
		if (Services.eventReport.eventDefinitionSeverityMap.find(eventID) == Services.eventReport.eventDefinitionSeverityMap.end()) {
			return;
		}
		EventReportService::EventReportSeverity severity = Services.eventReport.eventDefinitionSeverityMap[eventID];
		auto data = String<ECSSEventDataAuxiliaryMaxSize>("checkTransitionFailedFrom");
		data.append(reinterpret_cast<const char*>(transition.first));
		data.append("To");
		data.append(reinterpret_cast<const char*>(transition.second));
		if (severity == EventReportService::EventReportSeverity::Informative) {
			Services.eventReport.informativeEventReport(static_cast<EventReportService::Event>(eventID), data);
		} else if (severity == EventReportService::EventReportSeverity::Low) {
			Services.eventReport.lowSeverityAnomalyReport(static_cast<EventReportService::Event>(eventID), data);
		} else if (severity == EventReportService::EventReportSeverity::Medium) {
			Services.eventReport.mediumSeverityAnomalyReport(static_cast<EventReportService::Event>(eventID), data);
		} else if (severity == EventReportService::EventReportSeverity::High) {
			Services.eventReport.highSeverityAnomalyReport(static_cast<EventReportService::Event>(eventID), data);
		}

	}
}
