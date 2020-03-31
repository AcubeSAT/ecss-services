#include "Services/Parameter.hpp"
#include "etl/vector.h"
namespace SystemParameters {

	// initialize all system parameters here
    Parameter<u_int8_t> parameter1(5);
	Parameter<char> parameter2('a');
	Parameter<int> parameter3('b');
	Parameter<u_int8_t> parameter4(5);
	Parameter<u_int8_t> parameter5(5);
	etl::vector<ParameterBase*, ECSS_ST_20_MAX_PARAMETERS> arr;
	arr.push_back(paramter1)
	/**= {
		parameter1,
		parameter2,
		parameter3,
		parameter4,
		parameter5};**/
}
