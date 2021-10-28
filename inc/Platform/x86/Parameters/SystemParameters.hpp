#ifndef ECSS_SERVICES_SYSTEMPARAMETERS_HPP
#define ECSS_SERVICES_SYSTEMPARAMETERS_HPP

#include "Services/Parameter.hpp"
#include "etl/vector.h"
/**
 * @author Athanasios Theocharis <athatheoc@gmail.com>
 */

/**
 * This class was created for the purpose of initializing and storing explicitly
 * parameters (that are instances of the \ref Parameter class). It stores all the parameters
 * of the specific application. Different subsystems should have their own implementations of this class.
 * The position of the parameter in the vector is also called the parameter ID.
 *
 * It is initialised statically.
 *
 * The parameters here are under the responsibility of \ref ParameterService.
 */
class SystemParameters {
public:
	Parameter<uint8_t> parameter1 = Parameter<uint8_t>(3);
	Parameter<uint16_t> parameter2 = Parameter<uint16_t>(7);
	Parameter<uint32_t> parameter3 = Parameter<uint32_t>(10);
	/**
	 * The key of the array is the ID of the parameter as specified in PUS
	 */
	etl::array<std::reference_wrapper <ParameterBase>, ECSS_PARAMETER_COUNT> parametersArray = {
		parameter1, parameter2, parameter3
	};

	SystemParameters() = default;
};

extern SystemParameters systemParameters;

#endif
