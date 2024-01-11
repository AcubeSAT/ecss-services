#include "Helpers/PMON.hpp"
#include "ServicePool.hpp"


PMON::PMON(PMONId monitoredParameterId, uint16_t repetitionNumber)
    : monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId)->get()), monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber) {}