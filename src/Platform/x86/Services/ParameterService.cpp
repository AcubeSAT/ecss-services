#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Services/ParameterService.hpp"
#include "Parameters/PlatformParameters.hpp"

ParameterService::ParameterMap ParameterService::initializeParametersMap() {
	return ParameterMap({{(uint16_t)0, PlatformParameters::parameter1},
	                     {static_cast<uint16_t>(1), PlatformParameters::parameter2},
	                     {static_cast<uint16_t>(2), PlatformParameters::parameter3},
	                     {static_cast<uint16_t>(3), PlatformParameters::parameter4},
	                     {static_cast<uint16_t>(4), PlatformParameters::parameter5},
	                     {static_cast<uint16_t>(5), PlatformParameters::parameter6},
	                     {static_cast<uint16_t>(6), PlatformParameters::parameter7},
	                     {static_cast<uint16_t>(7), PlatformParameters::parameter8},
	                     {static_cast<uint16_t>(8), PlatformParameters::parameter9},
	                     {static_cast<uint16_t>(9), PlatformParameters::parameter10},
	                     {static_cast<uint16_t>(10), PlatformParameters::parameter11},
	                     {static_cast<uint16_t>(11), PlatformParameters::parameter12}});
}

#endif