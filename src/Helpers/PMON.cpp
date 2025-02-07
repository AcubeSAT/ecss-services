#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),
      checkType(checkType),
      monitoredParameter(*Services.parameterManagement.getParameter(monitoredParameterId)) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}

void PMON::updatePMONAfterPerformCheck(CheckingStatus previousStatus, CheckingStatus currentStatus) {
	if (checkingStatus == previousStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
    etl::pair<CheckingStatus, CheckingStatus> transition = etl::make_pair(previousStatus, checkingStatus);
		checkTransitionList.push_back(transition);

    if (pmonEventMap.find(transition) == pmonEventMap.end()) {
      return;
    }

    EventDefinitionId eventID = pmonEventMap[transition];
    if (eventDefinitionSeverityMap.find(eventID) == eventDefinitionSeverityMap.end()) {
      return;
    }
    EventReportService::EventReportSeverity severity = eventDefinitionSeverityMap[eventID];
    if (severity == EventReportService::EventReportSeverity::Informative) {
      Services.eventReport.informativeEventReport(eventID, "checkTransitionFailedFrom"+previousStatus+"To"+checkingStatus);
    } else if (severity == EventReportService::EventReportSeverity::Low) {
      Services.eventReport.lowSeverityAnomalyReport(eventID, "checkTransitionFailedFrom"+previousStatus+"To"+checkingStatus);
    } else if (severity == EventReportService::EventReportSeverity::Medium) {
      Services.eventReport.mediumSeverityAnomalyReport(eventID, "checkTransitionFailedFrom"+previousStatus+"To"+checkingStatus);
    } else if (severity == EventReportService::EventReportSeverity::High) {
      Services.eventReport.highSeverityAnomalyReport(eventID, "checkTransitionFailedFrom"+previousStatus+"To"+checkingStatus);
    }
	}
}
