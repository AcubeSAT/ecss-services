#ifndef ECSS_SERVICES_TIMEGETTER_HPP
#define ECSS_SERVICES_TIMEGETTER_HPP

#include <cstdint>
#include <ctime>
#include <Time/Time.hpp>
#include <Time/TimeStamp.hpp>
#include <Time/UTCTimestamp.hpp>

/**
 * @brief Get the current time
 */
class TimeGetter {
public:
	/**
	 * @brief Gets the current time in UNIX epoch
	 * @return Current UNIX epoch time, in elapsed seconds
	 */
	static inline uint32_t getSeconds() {
		return static_cast<uint32_t>(time(nullptr));
	}

	/**
	 * Returns the current UTC time.
	 * @note
	 * The information needed to compute the UTC time is implementation-specific. This function should
	 * be reimplemented to work for every format of the time-related parameters.
	 */
	static inline UTCTimestamp currentTimeUTC() {
		// todo: in the obc software repo, use the appropriate parameters to get the current UTC time and test it.
		UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0);
		return currentTime;
	}

	/**
	 * Converts the current UTC time, to a CUC formatted timestamp.
	 * @note
	 * The original format of the CUC (etl array of bits), is not used here, because it's easier to append
	 * a type uint64_t to a message object, rather than a whole array. Thus, we use the custom CUC format.
	 *
	 * @return CUC timestamp, formatted as elapsed 100ms ticks.
	 * @see Time.hpp
	 */
	static inline uint64_t currentTimeCustomCUC() {
		UTCTimestamp timeUTC = currentTimeUTC();
		TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
		return timeCUC.asCustomCUCTimestamp().elapsed100msTicks;
	}
};

#endif // ECSS_SERVICES_TIMEGETTER_HPP
