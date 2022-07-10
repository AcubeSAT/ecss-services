#pragma once

#include "Services/ParameterStatisticsService.hpp"

namespace PlatformParameters {
	enum parameterIDs : uint16_t {
		onBoardMinute = 0,
		temperature = 1,
	};
}

namespace ParameterStatistics {
	static etl::array<Statistic, ECSSMaxStatisticParameters> statistics = {
	    Statistic(),
	    Statistic(),
	};
}
