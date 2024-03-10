#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"

PMON::PMON(ParameterId monitoredParameterId, PMONRepetitionNumber repetitionNumber)
    : monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber)
{
	auto paramOpt = Services.parameterManagement.getParameter(monitoredParameterId);
	if (paramOpt.has_value()) {
		monitoredParameter.emplace(std::ref(*paramOpt));
	} else {
		ErrorHandler::reportInternalError(ErrorHandler::InvalidParameterId);
	}
}
