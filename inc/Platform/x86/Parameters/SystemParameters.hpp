#include "Helpers/Parameter.hpp"
#include "etl/vector.h"

/**
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 */

/**
 * This namespace was created for the purpose of initializing
 * parameters used by ecss-services.
 * It initializes parameters that are instances of the \ref Parameter class.
 * Different subsystems should have their own implementations of this namespace,
 * containing all parameters of the specific subsystem, inside inc/Platform
 * directory of their main project.
 * These parameters are stored inside the \ref ParameterService class.
 */
namespace SystemParameters {
	inline Parameter<uint8_t> parameter1 = Parameter<uint8_t>(3);
	inline Parameter<uint16_t> parameter2 = Parameter<uint16_t>(7);
	inline Parameter<uint32_t> parameter3 = Parameter<uint32_t>(10);
	inline Parameter<uint32_t> parameter4 = Parameter<uint32_t>(24);
}