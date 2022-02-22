#ifndef ECSS_SERVICES_TIME_HPP
#define ECSS_SERVICES_TIME_HPP

#include <cstdint>
#include <algorithm>
#include "macros.hpp"
#include <etl/array.h>
#include "Time.hpp"
#include "UTCTimestamp.hpp"

/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 *
 * @ingroup Time
 * @author Baptiste Fournier
 * @see [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf)
 */
template <uint8_t secondsBytes, uint8_t fractionalBytes>
class TimeStamp {
private:
	static_assert(secondsBytes + fractionalBytes <= 8,
	              "Currently, this class is not suitable for storage on internal counter larger than uint64_t");
	typedef typename std::conditional<(secondsBytes < 4 && fractionalBytes < 3), uint8_t, uint16_t>::type CUCHeader_t;
	typedef typename std::conditional<(secondsBytes + fractionalBytes < 4), uint32_t, uint64_t>::type TAICounter_t;

	/**
	 * Integer counter of time units since the @ref Time::Epoch. This number essentially represents the timestamp.
	 *
	 * The unit represented by this variable depends on `secondsBytes` and `fractionalBytes`. The fractional
	 * part is included as the least significant bits of this variable, and the base part follows.
	 */
	TAICounter_t taiCounter;

	/**
	 * The constant header ("P-value") of the timestamp, if needed to be attached to any message
	 */
	static constexpr CUCHeader_t CUCHeader = Time::buildCUCHeader<CUCHeader_t, secondsBytes, fractionalBytes>();

	/**
	 * The maximum value that can fit in @ref taiCounter, or the maximum number of seconds since epoch that can be
	 * represented in this base class
	 */
	static constexpr uint64_t maxSecondCounterValue = (1U << (8U * secondsBytes)) - 1;

	/**
	 * Returns whether the amount of `seconds` can be represented by this TimeStamp.
	 * If `seconds` is too large, the number of `secondsByte` may not be enough to represent this timestamp.
	 *
	 * @param seconds The amount of seconds from @ref Time::Epoch
	 */
	static constexpr bool areSecondsValid(TAICounter_t seconds);
public:
	/**
	 * Initialize the TimeStamp at @ref Time::Epoch
	 */
	TimeStamp() : taiCounter(0){};

	/**
	 * Initialize the TimeStamp from a duration from epoch in TAI (leap seconds not accounted)
	 *
	 * @param taiSecondsFromEpoch An integer number of seconds from the custom @ref Time::Epoch
	 */
	explicit TimeStamp(uint64_t taiSecondsFromEpoch);

	/**
	 * Initialize the TimeStamp from the bytes of a CUC time stamp
	 *
	 * @param timestamp A complete CUC timestamp including header, of the maximum possible size, zero padded to the
	 * right
	 */
	explicit TimeStamp(etl::array<uint8_t, Time::CUCTimestampMaximumSize> timestamp);

	/**
	 * Initialize the Timestamp from a UTC timestamp struct
	 *
	 * @param timestamp a UTC timestamp, from Unix Epoch
	 */
	explicit TimeStamp(const UTCTimestamp& timestamp);

	/**
	 * Get the representation as seconds from epoch in TAI
	 *
	 * @return The seconds elapsed in TAI since @ref Time::Epoch. This function is explicitly defined
	 */
	TAICounter_t asTAIseconds();

	/**
	 * Get the representation as seconds from epoch in TAI, for a floating-point representation.
	 * For an integer result, see the overloaded @ref asTAIseconds function.
	 *
	 * @todo Implement integer seconds in this function
	 * @tparam T The return type of the seconds (float or double).
	 * @return The seconds elapsed in TAI since @ref Time::Epoch
	 */
	template<typename T> T asTAIseconds();

	/**
	 * Get the representation as CUC formatted bytes
	 *
	 * @return The TimeStamp, represented in the CCSDS CUC format
	 */
	etl::array<uint8_t, Time::CUCTimestampMaximumSize> toCUCtimestamp();

	/**
	 * Get the representation as a UTC timestamp
	 *
	 * @return The TimeStamp, represented in the structure that holds UTC timestamps
	 */
	UTCTimestamp toUTCtimestamp();

	/**
	 * Compare two timestamps.
	 *
	 * @param timestamp the date that will be compared with the pointer `this`
	 * @return true if the condition is satisfied
	 */
	bool operator<(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter < timestamp.taiCounter;
	}

	bool operator>(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter > timestamp.taiCounter;
	}

	bool operator==(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter == timestamp.taiCounter;
	}

	bool operator!=(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter != timestamp.taiCounter;
	}

	bool operator<=(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter <= timestamp.taiCounter;
	}

	bool operator>=(const TimeStamp<secondsBytes, fractionalBytes>& timestamp) const {
		return taiCounter >= timestamp.taiCounter;
	}
};

#include "TimeStamp.tpp"
typedef TimeStamp<Time::CUCSecondsBytes,Time::CUCFractionalBytes> AcubesatTimestamp_t;

#endif
