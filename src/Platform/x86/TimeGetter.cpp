#include "Platform/x86/Helpers/TimeGetter.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0);
	return currentTime;
}

uint64_t TimeGetter::getCurrentTimeCustomCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
	TimeGetter::CUCTimeFormat CUCtime = timeCUC.asCustomCUCTimestamp().elapsed100msTicks;
	return CUCtime;
}
