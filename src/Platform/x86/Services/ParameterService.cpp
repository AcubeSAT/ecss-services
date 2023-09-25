#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Parameters/PlatformParameters.hpp"
#include "Services/ParameterService.hpp"

void ParameterService::initializeParameterMap() {
	parameters = {{static_cast<uint16_t>(0), PlatformParameters::parameter1},
	              {static_cast<uint16_t>(1), PlatformParameters::parameter2},
	              {static_cast<uint16_t>(2), PlatformParameters::parameter3}};
}

#endif
