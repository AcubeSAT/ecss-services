#include "Services/HousekeepingDefinitions.hpp"
#include "etl/map.h"

/**
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class SystemHousekeeping {
public:

	HousekeepingParameter <uint8_t> parameter1 = HousekeepingParameter <uint8_t> (3);
	HousekeepingParameter <uint16_t> parameter2 = HousekeepingParameter <uint16_t> (4);
	HousekeepingParameter <uint32_t> parameter3 = HousekeepingParameter <uint32_t> (6);

	etl::map <uint16_t, std::reference_wrapper <HousekeepingParameterBase>, ECSS_PARAMETER_COUNT>
	    housekeepingParameters = { {0, parameter1},
	                               {1, parameter2},
	                               {2, parameter3} };

	SystemHousekeeping() = default;
};

extern SystemHousekeeping systemHousekeeping;
