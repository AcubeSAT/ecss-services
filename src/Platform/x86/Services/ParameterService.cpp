#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Parameters/SystemParameters.hpp"

etl::array<std::reference_wrapper<ParameterBase>, ECSSParameterCount> ParameterService::initializeParametersArray() {
	return etl::array<std::reference_wrapper<ParameterBase>, ECSSParameterCount>(
	    {SystemParameters::parameter1, SystemParameters::parameter2, SystemParameters::parameter3,
	     SystemParameters::parameter4, SystemParameters::parameter5, SystemParameters::parameter6,
	     SystemParameters::parameter7, SystemParameters::parameter8, SystemParameters::parameter9,
	     SystemParameters::parameter10, SystemParameters::parameter11, SystemParameters::parameter12});
}

#endif