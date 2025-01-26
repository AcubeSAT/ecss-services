#include "Helpers/TimeGetter.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	time_t timeInSeconds = static_cast<time_t>(time(nullptr));
	tm* UTCTimeStruct = gmtime(&timeInSeconds);
	UTCTimestamp currentTime(UTCTimeStruct->tm_year + 1900, UTCTimeStruct->tm_mon + 1,
	                         UTCTimeStruct->tm_mday, UTCTimeStruct->tm_hour,
	                         UTCTimeStruct->tm_min, UTCTimeStruct->tm_sec);
	return currentTime;
}

Time::DefaultCUC TimeGetter::getCurrentTimeDefaultCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	return Time::DefaultCUC(timeUTC);
}
