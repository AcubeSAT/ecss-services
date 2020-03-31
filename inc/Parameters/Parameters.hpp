#include "Services/Parameter.hpp"
#include "etl/vector.h"
class SystemParameters {
public:
	Parameter<uint8_t> parameter1 = Parameter<uint8_t>(5);
	Parameter<char> parameter2=  Parameter<char>('a');
	Parameter<int> parameter3 = Parameter<int>('b');
	Parameter<uint8_t> parameter4 = Parameter<uint8_t>(5);
	Parameter<uint8_t> parameter5 = Parameter<uint8_t>(5);
	etl::vector<std::reference_wrapper<ParameterBase>, ECSS_ST_20_MAX_PARAMETERS> parametersArray;
	SystemParameters() = default;
};

extern SystemParameters systemParameters;
