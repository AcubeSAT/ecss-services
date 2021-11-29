#include "Parameters/SystemParameters.hpp"

std::reference_wrapper<ParameterBase> SystemParameters::getParameter(uint16_t parameterId) {
	return parametersArray[parameterId];
}

SystemParameters systemParameters;
