#ifndef ECSS_SERVICES_SYSTEMPARAMETERS_HPP
#define ECSS_SERVICES_SYSTEMPARAMETERS_HPP

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
	inline Parameter<uint32_t> parameter4 = Parameter<uint32_t>(5); //24
	inline Parameter<uint8_t> parameter5 = Parameter<uint8_t>(11);
	inline Parameter<uint32_t> parameter6 = Parameter<uint32_t>(23);
	inline Parameter<uint32_t> parameter7 = Parameter<uint32_t>(53);
	inline Parameter<uint8_t> parameter8 = Parameter<uint8_t>(55);
	inline Parameter<uint16_t> parameter9 = Parameter<uint16_t>(32);
	inline Parameter<uint32_t> parameter10 = Parameter<uint32_t>(43);
	inline Parameter<uint32_t> parameter11 = Parameter<uint32_t>(91);
	inline Parameter<uint8_t> parameter12 = Parameter<uint8_t>(1);
}

#endif