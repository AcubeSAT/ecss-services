#ifndef ECSS_SERVICES_TIMECONVERTER_HPP
#define ECSS_SERVICES_TIMECONVERTER_HPP

#include <Time/Time.hpp>
#include <Time/TimeStamp.hpp>
#include <Time/UTCTimestamp.hpp>

/**
 * Tool for getting the current time and converting it to CUC format. The reverse operation
 * is also supported.
 */
class TimeConverter {
public:
	/**
	 * Returns the current UTC time.
	 * @note
	 * The information needed to compute the UTC time is implementation-specific. This function should
	 * be reimplemented to work for every format of the time-related parameters.
	 */
	static UTCTimestamp currentTimeUTC();

	/**
	 * Converts the current UTC time, to a CUC formatted timestamp.
	 * @note
	 * The original format of the CUC (etl array of bits), is not used here, because it's easier to append
	 * a type uint64_t to a message object, rather than a whole array. Thus, we use the custom CUC format.
	 *
	 * @return CUC timestamp, formatted as elapsed 100ms ticks.
	 * @see Time.hpp
	 */
	static uint64_t currentTimeCustomCUC();
};

#endif
