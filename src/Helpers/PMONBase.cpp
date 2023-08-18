#include "Helpers/PMONBase.hpp"
#include "ServicePool.hpp"


PMONBase::PMONBase(PMONIdSize monitoredParameterId, uint16_t repetitionNumber)
    : monitoredParameter(Services.parameterManagement.getParameter(monitoredParameterId)->get()), monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber) {}