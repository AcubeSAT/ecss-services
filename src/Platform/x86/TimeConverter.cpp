#include "Platform/x86/TimeConverter.hpp"

UTCTimestamp TimeConverter::currentTimeUTC() {
	// todo: in the obc software repo, use the appropriate parameters to get the current UTC time and test it.
	UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0);
	return currentTime;
}

uint64_t TimeConverter::currentTimeCustomCUC() {
	UTCTimestamp timeUTC = currentTimeUTC();
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
	return timeCUC.asCustomCUCTimestamp().elapsed100msTicks;
}
