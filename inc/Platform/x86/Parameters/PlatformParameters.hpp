#ifndef ECSS_SERVICES_PLATFORMPARAMETERS_HPP
#define ECSS_SERVICES_PLATFORMPARAMETERS_HPP

#include "Helpers/Parameter.hpp"
#include "etl/vector.h"

/**
 * This namespace was created for the purpose of initializing
 * parameters used by ecss-services.
 * It initializes parameters that are instances of the \ref Parameter class.
 * These parameters are stored inside the \ref ParameterService class.
 *
 * @note the parameters in this specific file are only used for testing purposes,
 * different subsystems should have their own implementations of this namespace,
 * containing all parameters of the specific subsystem, inside the inc/Platform
 * directory of their main project.
 */
namespace PlatformParameters {
	inline Parameter<uint8_t> parameter1(3);
	inline Parameter<uint16_t> parameter2(7);
	inline Parameter<uint32_t> parameter3(10);
	inline Parameter<uint32_t> parameter4(5);
	inline Parameter<uint8_t> parameter5(11);
	inline Parameter<uint32_t> parameter6(23);
	inline Parameter<uint32_t> parameter7(53);
	inline Parameter<uint8_t> parameter8(55);
	inline Parameter<uint16_t> parameter9(32);
	inline Parameter<uint32_t> parameter10(43);
	inline Parameter<uint32_t> parameter11(91);
	inline Parameter<uint8_t> parameter12(1);
}

#endif