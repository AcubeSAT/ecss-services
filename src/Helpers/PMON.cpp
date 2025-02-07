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

void PMON::updatePMONAfterPerformCheck(CheckingStatus previousStatus, CheckingStatus currentStatus) {
	if (checkingStatus == previousStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
		etl::pair<CheckingStatus, CheckingStatus> transition = etl::make_pair(previousStatus, checkingStatus);
		checkTransitionList.insert(nullptr, transition);

		if (pmonTransitionEventMap.find(transition) == pmonTransitionEventMap.end()) {
			return;
		}

		EventDefinitionId eventID = pmonTransitionEventMap[transition];
		if (Services.eventReport.eventDefinitionSeverityMap.find(eventID) == Services.eventReport.eventDefinitionSeverityMap.end()) {
			return;
		}
		EventReportService::EventReportSeverity severity = Services.eventReport.eventDefinitionSeverityMap[eventID];
		String<ECSSEventDataAuxiliaryMaxSize> data = String<ECSSEventDataAuxiliaryMaxSize>("checkTransitionFailedFrom");
		data.append(reinterpret_cast<const char*>(previousStatus));
		data.append("To");
		data.append(reinterpret_cast<const char*>(currentStatus));
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
