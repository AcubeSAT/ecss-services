#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber, CheckType checkType)
    : monitoredParameterId(monitoredParameterId),
      monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId).value()),
      repetitionNumber(repetitionNumber),
      checkType(checkType) {
	if (!Services.parameterManagement.getParameter(monitoredParameterId)) {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}