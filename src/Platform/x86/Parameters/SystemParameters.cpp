#include "Parameters/SystemParameters.hpp"

ParameterBase& SystemParameters::getParameter(uint16_t parameterId) {
	return parametersArray[parameterId];
}

SystemParameters systemParameters;
