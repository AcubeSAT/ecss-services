#ifndef ECSS_SERVICES_PLATFORMPARAMETERS_HPP
#define ECSS_SERVICES_PLATFORMPARAMETERS_HPP

#include "Helpers/Parameter.hpp"

/**
 * This namespace was created for the purpose of initializing
 * parameters used by ecss-services.
 * It initializes parameters that are instances of the \ref Parameter class.
 * \ref ParameterService class stores references to these parameters in the
 * form of a map, using parameter IDs as keys.
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
}

#endif
