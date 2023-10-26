#include "Helpers/PMONBase.hpp"
#include "ServicePool.hpp"


PMONBase::PMONBase(uint16_t monitoredParameterId, uint16_t repetitionNumber)
    : monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId)->get()), monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber) {}