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
	Parameter<uint32_t> parameter4 = Parameter<uint32_t>(24);
	/**
	 * The key of the array is the ID of the parameter as specified in PUS
	 */
	etl::array<std::reference_wrapper<ParameterBase>, ECSSParameterCount> parametersArray = {parameter1, parameter2,
	                                                                                           parameter3, parameter4};

	SystemParameters() = default;

	/**
	 * This is a simple getter function, which returns a reference to a specified parameter, from the parametersArray.
	 *
	 * @param parameterId the id of the parameter, whose reference is to be returned.
	 */
	std::optional<std::reference_wrapper<ParameterBase>> getParameter(uint16_t parameterId) {
		if (parameterId >= parametersArray.size()) {
			ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::NonExistentParameter);
			return {};
		}
		return {parametersArray[parameterId]};
	}

};

extern SystemParameters systemParameters;
