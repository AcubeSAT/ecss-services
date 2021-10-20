#include "Services/Statistic.hpp"
#include "etl/map.h"

class SystemStatistics {
public:

	/**
	 * Map containing parameters' IDs followed by the statistics that correspond to the specified parameter
	 */
	etl::map <uint16_t, Statistic, ECSS_MAX_STATISTIC_PARAMETERS> statisticsMap;
};

extern SystemStatistics systemStatistics;