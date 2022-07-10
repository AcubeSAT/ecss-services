#include "Platform/ParameterStatisticsInitialization.hpp"

void ParameterStatisticsService::initializeStatisticsMap() {
	using namespace PlatformParameters;
	uint16_t statisticParameterIDs[] = {parameterIDs::onBoardMinute, parameterIDs::temperature};
	uint8_t idIndex = 0;

	for (auto& statistic: ParameterStatistics::statistics) {
		statisticsMap.insert({statisticParameterIDs[idIndex++], statistic});
	}
}
