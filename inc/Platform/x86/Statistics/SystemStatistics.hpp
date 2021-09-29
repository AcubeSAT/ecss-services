#include "Services/ParameterStatisticsService.hpp"
#include "etl/vector.h"

class SystemStatistics {
public:
	Statistic <uint8_t> stat1 = Statistic <uint8_t>();
	Statistic <uint16_t> stat2 = Statistic <uint16_t>();
	Statistic <uint32_t> stat3 = Statistic <uint32_t>();
	/**
	 * The key of the array is the ID of the parameter as specified in PUS
	 */
	etl::vector <std::reference_wrapper <StatisticBase>, ECSS_MAX_PARAMETERS> statisticsVec = {
	    stat1, stat2, stat3
	};

	SystemStatistics() = default;
};

extern SystemStatistics systemStatistics;