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
	HousekeepingParameter <uint32_t> parameter4 = HousekeepingParameter <uint32_t> (2);
	HousekeepingParameter <uint16_t> parameter5 = HousekeepingParameter <uint16_t> (7);
	HousekeepingParameter <uint8_t> parameter6 = HousekeepingParameter <uint8_t> (8);
	HousekeepingParameter <uint16_t> parameter7 = HousekeepingParameter <uint16_t> (1);
	HousekeepingParameter <uint32_t> parameter8 = HousekeepingParameter <uint32_t> (5);
	HousekeepingParameter <uint8_t> parameter9 = HousekeepingParameter <uint8_t> (1);

	etl::map <uint16_t, std::reference_wrapper <HousekeepingParameterBase>, ECSS_HOUSEKEEPING_PARAMETERS>
	    housekeepingParameters = { {0, parameter1},
	                               {1, parameter2},
	                               {2, parameter3},
	                               {3, parameter4},
	                               {4, parameter5},
	                               {5, parameter6},
	                               {6, parameter7},
	                               {7, parameter8},
	                               {8, parameter9} };

	SystemHousekeeping() = default;
};
