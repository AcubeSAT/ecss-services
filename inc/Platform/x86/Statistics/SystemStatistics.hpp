#include "Services/Statistic.hpp"
#include "etl/map.h"

class SystemStatistics {
public:

	/**
	 * Map containing parameters' IDs followed by the statistics that correspond to the specified parameter
	 */
	etl::map <uint16_t, std::reference_wrapper <Statistic>, ECSS_PARAMETER_COUNT> statisticsMap;
};

extern SystemStatistics systemStatistics;