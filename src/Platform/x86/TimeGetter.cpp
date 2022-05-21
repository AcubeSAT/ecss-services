#include "Helpers/TimeGetter.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0);
	return currentTime;
}

Time::CustomCUC_t TimeGetter::getCurrentTimeCustomCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
	Time::CustomCUC_t CUCtime = timeCUC.asCustomCUCTimestamp();
	return CUCtime;
}
