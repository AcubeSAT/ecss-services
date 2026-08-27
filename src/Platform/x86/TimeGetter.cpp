#include "Helpers/TimeGetter.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	// struct tm counts years since 1900 and months from 0, while UTCTimestamp expects absolute values
	constexpr int TmYearOffset = 1900;
	constexpr int TmMonthOffset = 1;

	time_t timeInSeconds = static_cast<time_t>(time(nullptr));
	tm* UTCTimeStruct = gmtime(&timeInSeconds);
	UTCTimestamp currentTime(UTCTimeStruct->tm_year + TmYearOffset, UTCTimeStruct->tm_mon + TmMonthOffset,
	                         UTCTimeStruct->tm_mday, UTCTimeStruct->tm_hour,
	                         UTCTimeStruct->tm_min, UTCTimeStruct->tm_sec);
	return currentTime;
}

Time::DefaultCUC TimeGetter::getCurrentTimeDefaultCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	return Time::DefaultCUC(timeUTC);
}
