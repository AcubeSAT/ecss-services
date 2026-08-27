#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"
#include "Services/EventReportService.hpp"
#include "etl/to_string.h"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId).value()),
      monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),
      checkType(checkType) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}

void PMON::updateAfterCheck(const CheckingStatus newCheckingStatus) {
	if (nextCheckingStatus == newCheckingStatus) {
		repetitionCounter++;
	} else {
		repetitionCounter = 1;
		nextCheckingStatus = newCheckingStatus;
	}

	if (repetitionCounter < repetitionNumber || currentCheckingStatus == nextCheckingStatus) {
		return;
	}
	const PMONTransition transition = etl::make_pair(currentCheckingStatus, nextCheckingStatus);
	if (checkTransitions.full()) {
		ErrorHandler::reportInternalError(ErrorHandler::PMONCheckTransitionsFull);
		return;
	}
	checkTransitions.push_back(transition);
	currentCheckingStatus = nextCheckingStatus;

	// As per 6.12.3.3j and 6.12.3.6.3d(b) of ECSS-E-ST-70-41C, every parameter monitoring definition
	// associates an event definition with each of its out-of-limit checking statuses. When a transition
	// to such a status is established, the corresponding event is raised.
	const etl::optional<EventDefinitionId> event = getEventDefinitionForStatus(nextCheckingStatus);
	if (event.has_value()) {
		String<ECSSEventDataAuxiliaryMaxSize> eventData("ID ");
		etl::to_string(monitoredParameterId, eventData, true);
		eventData.append(" from ");
		etl::to_string(static_cast<uint16_t>(transition.first), eventData, true);
		eventData.append(" to ");
		etl::to_string(static_cast<uint16_t>(transition.second), eventData, true);
		Services.eventReport.raiseEvent(event.value(), eventData);
	}
}
