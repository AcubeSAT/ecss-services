#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"


PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber)
{
	auto paramOpt = Services.parameterManagement.getParameter(monitoredParameterId);
	if (paramOpt.has_value()) {
		monitoredParameter = paramOpt->get(); // Assuming get() returns a reference
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}