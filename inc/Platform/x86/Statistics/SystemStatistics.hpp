#include "Services/Statistic.hpp"
#include "etl/map.h"

class SystemStatistics {
public:
	Statistic <uint8_t> stat1 = Statistic <uint8_t>();
	Statistic <uint16_t> stat2 = Statistic <uint16_t>();
	Statistic <uint32_t> stat3 = Statistic <uint32_t>();

//	etl::array <std::reference_wrapper <StatisticBase>, ECSS_PARAMETER_COUNT> statisticsArray = {
//	    stat1, stat2, stat3
//	};

	etl::map <uint16_t, std::reference_wrapper <StatisticBase>, ECSS_PARAMETER_COUNT> statisticsMap;

	/**
	 * Has to be called before the first usage of the statisticsMap.
	 */
	void statisticsMapInit() {
		statisticsMap.insert({0, stat1});
		statisticsMap.insert({1, stat2});
		statisticsMap.insert({2, stat3});
	}

	SystemStatistics() = default;
};

extern SystemStatistics systemStatistics;