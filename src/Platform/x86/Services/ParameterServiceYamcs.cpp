#include "ECSS_Configuration.hpp"
#ifdef SERVICE_PARAMETER

#include "Parameters/PlatformParametersYamcs.hpp"
#include "Services/ParameterServiceYamcs.hpp"

void ParameterServiceYamcs::initializeParameterMapYamcs() {

	parametersYamcs = {{static_cast<uint16_t>(0), PlatformParametersYamcs::parameter1},
	              {static_cast<uint16_t>(1), PlatformParametersYamcs::parameter2},
	              {static_cast<uint16_t>(2), PlatformParametersYamcs::parameter3},
	              {static_cast<uint16_t>(1013), PlatformParametersYamcs::parameter4},
	              {static_cast<uint16_t>(1014), PlatformParametersYamcs::parameter5},
	              {static_cast<uint16_t>(1015), PlatformParametersYamcs::parameter6},
	              {static_cast<uint16_t>(1043), PlatformParametersYamcs::parameter7},
	              {static_cast<uint16_t>(1044), PlatformParametersYamcs::parameter8},
	              {static_cast<uint16_t>(1045), PlatformParametersYamcs::parameter9},
	              {static_cast<uint16_t>(5000), PlatformParametersYamcs::parameter10},
	              {static_cast<uint16_t>(5001), PlatformParametersYamcs::parameter11},
	              {static_cast<uint16_t>(1092), PlatformParametersYamcs::parameter12},
	              {static_cast<uint16_t>(1093), PlatformParametersYamcs::parameter13},
	              {static_cast<uint16_t>(5010), PlatformParametersYamcs::parameter14} };
}

#endif
