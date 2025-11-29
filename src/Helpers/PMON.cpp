#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"
#include "Services/EventReportService.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),
      checkType(checkType),
      monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId).value()),
      	newTrackedCheckingStatus(Unchecked) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}

void PMON::updateAfterCheck(const CheckingStatus newCheckingStatus) {
	if (newTrackedCheckingStatus == newCheckingStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
		newTrackedCheckingStatus = newCheckingStatus;
	}

	if (repetitionCounter < repetitionNumber || currentCheckingStatus == newTrackedCheckingStatus) {
		return;
	}
	const PMONTransition transition = etl::make_pair(currentCheckingStatus, newTrackedCheckingStatus);
	checkTransitions.push_back(transition);
	currentCheckingStatus = newTrackedCheckingStatus;

    auto pmonMapResult = pmonTransitionEventMap.find(transition)
    if (pmonMapResult == pmonTransitionEventMap.end()) {
      return;
    }

    EventDefinitionId eventID = pmonMapResult->second;

	if (eventID - 1 >= Services.eventReport.eventDefinitionSeverityArray.size()) {
		return;
	}

	Services.eventReport.raiseTransitionEvent(monitoredParameterId, transition, eventID);
}
