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
namespace SystemParameters
{
inline Parameter<uint8_t> parameter1 = Parameter<uint8_t>(3);
inline Parameter<uint16_t> parameter2 = Parameter<uint16_t>(7);
inline Parameter<uint32_t> parameter3 = Parameter<uint32_t>(10);
inline Parameter<uint32_t> parameter4 = Parameter<uint32_t>(24);

} // namespace SystemParameters