#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"


PMON::PMON(PMONId monitoredParameterId, PMONRepetitionNumber repetitionNumber)
    : monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId)->get()), monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber),checkType(CheckType::Limit)
{
	auto paramOpt = Services.parameterManagement.getParameter(monitoredParameterId);
	if (paramOpt.has_value()) {
		monitoredParameter = paramOpt->get(); // Assuming get() returns a reference
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}
