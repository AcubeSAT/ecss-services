#ifndef ECSS_SERVICES_SYSTEMPARAMETERS_HPP
#define ECSS_SERVICES_SYSTEMPARAMETERS_HPP

#include "Helpers/Parameter.hpp"
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
	Parameter<uint32_t> parameter4 = Parameter<uint32_t>(5);
	Parameter<uint8_t> parameter5 = Parameter<uint8_t>(11);
	Parameter<uint32_t> parameter6 = Parameter<uint32_t>(23);
	Parameter<uint32_t> parameter7 = Parameter<uint32_t>(53);
	Parameter<uint8_t> parameter8 = Parameter<uint8_t>(55);
	Parameter<uint16_t> parameter9 = Parameter<uint16_t>(32);
	Parameter<uint32_t> parameter10 = Parameter<uint32_t>(43);
	Parameter<uint32_t> parameter11 = Parameter<uint32_t>(91);
	Parameter<uint8_t> parameter12 = Parameter<uint8_t>(1);

	/**
	 * The key of the array is the ID of the parameter as specified in PUS
	 */
	etl::array<std::reference_wrapper<ParameterBase>, ECSS_PARAMETER_COUNT> parametersArray = {
	    parameter1, parameter2, parameter3, parameter4,  parameter5,  parameter6,
	    parameter7, parameter8, parameter9, parameter10, parameter11, parameter12};

	SystemParameters() = default;

	/**
	 * This is a simple getter function, which returns a reference to a specified parameter, from the parametersArray.
	 *
	 * @param parameterId the id of the parameter, whose reference is to be returned.
	 */
	ParameterBase& getParameter(uint16_t parameterId);
};

extern SystemParameters systemParameters;

#endif
