	#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Parameters/PlatformParameters.hpp"
#include "Services/ParameterService.hpp"

void ParameterService::initializeParameterMap() {
	parameters = {{static_cast<uint16_t>(1013), PlatformParameters::parameter1},
	              {static_cast<uint16_t>(1014), PlatformParameters::parameter2},
	              {static_cast<uint16_t>(1015), PlatformParameters::parameter3},
	              {static_cast<uint16_t>(1043), PlatformParameters::parameter4},
	              {static_cast<uint16_t>(1044), PlatformParameters::parameter5},
	              {static_cast<uint16_t>(1045), PlatformParameters::parameter6},

	              {static_cast<uint16_t>(5000), PlatformParameters::parameter7},
	              {static_cast<uint16_t>(5001), PlatformParameters::parameter8},
	              {static_cast<uint16_t>(1092), PlatformParameters::parameter9},
	              {static_cast<uint16_t>(1093), PlatformParameters::parameter10} };

}

#endif
