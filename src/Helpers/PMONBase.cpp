#include "Helpers/PMONBase.hpp"
#include "ServicePool.hpp"


PMONBase::PMONBase(uint16_t monitoredParameterId, uint16_t repetitionNumber)
    : monitoredParameter(monitoredParameter), monitoredParameterId(monitoredParameterId),
      repetitionNumber(repetitionNumber) {
	monitoredParameter = Services.parameterManagement.getParameter(monitoredParameterId)->get();
}