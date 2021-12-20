#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Helpers/Parameter.hpp"
#include "Parameters/SystemParameters.hpp"


etl::array<std::reference_wrapper<ParameterBase>, ECSSParameterCount>
ParameterService::initializeParametersArray() {
	return etl::array<std::reference_wrapper<ParameterBase>, ECSSParameterCount>(
	    {SystemParameters::parameter1, SystemParameters::parameter2,
	     SystemParameters::parameter3, SystemParameters::parameter4});
}

ParameterService::ParameterService() : parametersArray(initializeParametersArray()) {}

#endif
