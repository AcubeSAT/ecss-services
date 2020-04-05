#include "Services/Parameter.hpp"
#include "etl/vector.h"

class SystemParameters {
public:
	Parameter<uint8_t> parameter1 = Parameter<uint8_t>(5);
	Parameter<uint16_t> parameter2 = Parameter<uint16_t>(5);
	Parameter<uint32_t> parameter3 = Parameter<uint32_t>(5);
	etl::vector<std::reference_wrapper<ParameterBase>, ECSS_ST_20_MAX_PARAMETERS> parametersArray;
	SystemParameters() {
		parametersArray[0] = parameter1;
		parametersArray[1] = parameter2;
		parametersArray[2] = parameter3;
	}
};

extern SystemParameters systemParameters;
