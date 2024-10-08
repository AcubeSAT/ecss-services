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