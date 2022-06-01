#include "Helpers/TimeGetter.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	time_t timeInSeconds = static_cast<time_t>(time(nullptr));
	tm* UTCTimeStruct = gmtime(&timeInSeconds);
	UTCTimestamp currentTime(UTCTimeStruct->tm_year, UTCTimeStruct->tm_mon,
	                         UTCTimeStruct->tm_mday, UTCTimeStruct->tm_hour,
	                         UTCTimeStruct->tm_min, UTCTimeStruct->tm_sec);
	return currentTime;
}

Time::CustomCUC_t TimeGetter::getCurrentTimeCustomCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
	Time::CustomCUC_t CUCtime = timeCUC.asCustomCUCTimestamp();
	return CUCtime;
}
