#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <etl/array.h>
#include "Time.hpp"
#include "UTCTimestamp.hpp"
#include "macros.hpp"

/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * This class is compatible with the CUC (Unsegmented Time Code) format defined in CCSDS 301.0-B-4. It allows specifying:
 * - Different amount of bytes for the basic time unit
 * - Different amount of bytes for the fractional time unit
 * - Different basic time units
 *
 * The timestamp is defined in relation to a user-defined epoch, set in @ref Time::Epoch.
 *
 * @section baseunit Setting the base time unit
 * By default, this class measures time in the order of **seconds**. Binary fractions of a second can be specified by increasing the FractionBytes.
 * However, the user can change the base time unit by setting the @p Num and @p Denom template parameters.
 *
 * The base time unit (or period) is then represented by the following:
 * \f[
 * \text{time unit} = \frac{Num}{Denom} \cdot \text{second}
 * \f]
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 *
 * @tparam BaseBytes The number of bytes used for the basic time units. This essentially defines the maximum duration from Epoch that this timestamp can represent.
 * @tparam FractionBytes The number of bytes used for the fraction of one basic time unit. This essentially defines the precision of the timestamp.
 * @tparam Num The numerator of the base type ratio (see @ref baseunit)
 * @tparam Denom The numerator of the base type ratio (see @ref baseunit)
 *
 * @ingroup Time
 * @author Baptiste Fournier
 * @author Konstantinos Kanavouras
 * @see [CCSDS 301.0-B-4](https://public.ccsds.org/Pubs/301x0b4e1.pdf)
 */
template <uint8_t BaseBytes = 4, uint8_t FractionBytes = 0, int Num = 1, int Denom = 1>
class TimeStamp {
public:
	/**
	 * The period of the base type, in relation to the second
	 *
	 * This type represents the base type of the timestamp.
	 *
	 * A ratio of `<1, 1>` (or 1/1) means that this timestamp represents seconds. A ratio of `<60, 1>` (or 60/1) means
	 * that this class represents 60s of seconds, or minutes. A ratio of `<1, 1000>` (or 1/1000) means that this class
	 * represents 1000ths of seconds, or milliseconds.
	 *
	 * This type has essentially the same meaning of `Rep` in [std::chrono::duration](https://en.cppreference.com/w/cpp/chrono/duration).
	 *
	 * @note std::ratio will simplify the fractions numerator and denominator
	 */
	using Ratio = std::ratio<Num, Denom>;

private:
	static_assert(BaseBytes + FractionBytes <= 8,
	              "Currently, this class is not suitable for storage on internal counter larger than uint64_t");
	using CUCHeader_t = typename std::conditional<(BaseBytes < 4 && FractionBytes < 3), uint8_t, uint16_t>::type;
	using TAICounter_t = typename std::conditional<(BaseBytes + FractionBytes <= 4), uint32_t, uint64_t>::type;

	/**
	 * The period of the internal counter
	 *
	 * Same as @ref Ratio, but instead of representing the Base bytes, it represents the entire value held by @ref taiCounter.
	 */
	using RawRatio = std::ratio<Num, Denom * 1UL << (8 * FractionBytes)>;

	/**
	 * An std::chrono::duration representation of the base type (without the fractional part)
	 */
	using BaseDuration = std::chrono::duration<TAICounter_t, Ratio>;

	/**
	 * An std::chrono::duration representation of the complete @ref taiCounter (including the fractional part)
	 */
	using RawDuration = std::chrono::duration<TAICounter_t, RawRatio>;

	template <uint8_t, uint8_t, int, int>
	friend class TimeStamp;

	/**
	 * Integer counter of time units since the @ref Time::Epoch. This number essentially represents the timestamp.
	 *
	 * The unit represented by this variable depends on `BaseBytes` and `FractionBytes`. The fractional
	 * part is included as the least significant bits of this variable, and the base part follows.
	 */
	TAICounter_t taiCounter;

	/**
	 * The constant header ("P-value") of the timestamp, if needed to be attached to any message
	 */
	static constexpr CUCHeader_t CUCHeader = Time::buildCUCHeader<CUCHeader_t, BaseBytes, FractionBytes>();

	/**
	 * The maximum value of the base type (seconds, larger or smaller) that can fit in @ref taiCounter
	 */
	static constexpr uint64_t MaxBase =
	    (BaseBytes == 8)
	        ? std::numeric_limits<uint64_t>::max()
	        : (uint64_t{1} << 8 * BaseBytes) - 1;

	/**
	 * The maximum number of seconds since epoch that can be represented in this class
	 */
	static constexpr uint64_t MaxSeconds = std::chrono::duration_cast<std::chrono::duration<uint64_t>>(BaseDuration(MaxBase)).count();

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
	 * @param timestamp A complete CUC timestamp including header, of the maximum possible size, zero padded to the right
	 */
	explicit TimeStamp(etl::array<uint8_t, Time::CUCTimestampMaximumSize> timestamp);

	/**
	 * Initialize the Timestamp from a UTC timestamp struct
	 *
	 * @param timestamp a UTC timestamp, from Unix Epoch
	 */
	explicit TimeStamp(const UTCTimestamp& timestamp);

	/**
	 * Convert a TimeStamp to a TimeStamp with different parameters
	 *
	 * This constructor will convert based on the number of bytes, and base units
	 *
	 * @note Internally uses double-precision floating point to allow for arbitrary ratios
	 */
	template <uint8_t BaseBytesIn, uint8_t FractionBytesIn, int NumIn = 1, int DenomIn = 1>
	explicit TimeStamp(TimeStamp<BaseBytesIn, FractionBytesIn, NumIn, DenomIn> input);

	/**
	 * Convert an [std::chrono::duration](https://en.cppreference.com/w/cpp/chrono/duration) representing seconds from @ref Time::Epoch
	 * to a timestamp
	 *
	 * @warning This function does not perform overflow calculations. It is up to the user to ensure that the types are
	 * compatible so that no overflow occurs.
	 */
	template <class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	explicit TimeStamp(Duration duration);

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
	 * @tparam T The return type of the seconds (float or double).
	 * @return The seconds elapsed in TAI since @ref Time::Epoch
	 */
	template <typename T>
	T asTAIseconds();

	/**
	 * Converts a TimeStamp to a duration of seconds since the @ref Time::Epoch.
	 *
	 * @warning This function does not perform overflow calculations. It is up to the user to ensure that the types are compatible so that no overflow occurs.
	 */
	template <class Duration = std::chrono::seconds>
	Duration asDuration() const;

	/**
	 * Get the representation as CUC formatted bytes, including the header (P-field and T-field)
	 */
	etl::array<uint8_t, Time::CUCTimestampMaximumSize> formatAsCUC();

	/**
	 * Get the representation as CUC formatted bytes, without the header (T-field only)
	 */
	TAICounter_t formatAsBytes() const {
		return taiCounter;
	}

	/**
	 * Get the representation as a UTC timestamp
	 *
	 * @return The TimeStamp, represented in the structure that holds UTC timestamps
	 */
	UTCTimestamp toUTCtimestamp();

	/**
	 * Get the maximum timestamp that can be represented by this class
	 *
	 * Can be used to represent null or infinite amounts of time
	 */
	static TimeStamp<BaseBytes, FractionBytes, Num, Denom> max() {
		TimeStamp<BaseBytes, FractionBytes, Num, Denom> timestamp;
		timestamp.taiCounter = std::numeric_limits<TAICounter_t>::max();
		return timestamp;
	}

	/**
	 * Adds any arbitrary duration to a timestamp.
	 *
	 * You can play with default C++ durations with this function:
	 * ```cpp
	 * using namespace std::literals;
	 *
	 * timestamp + std::chrono::seconds(5);        // adds 5 seconds
	 * timestamp + std::chrono::milliseconds(500); // adds 5 milliseconds
	 * timestamp + 60s; // adds 60 seconds
	 */
	template <class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	TimeStamp<BaseBytes, FractionBytes, Num, Denom> operator+(const Duration& duration) const {
		auto output = *this;
		output += duration;
		return output;
	}

	template <class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	TimeStamp<BaseBytes, FractionBytes, Num, Denom>& operator+=(const Duration& duration) {
		if (duration < Duration::zero()) {
			taiCounter -= std::chrono::duration_cast<RawDuration>(-duration).count();
		} else {
			taiCounter += std::chrono::duration_cast<RawDuration>(duration).count();
		}

		return *this;
	}

	template <class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	TimeStamp<BaseBytes, FractionBytes, Num, Denom> operator-(const Duration& duration) const {
		auto output = *this;
		output -= duration;
		return output;
	}

	template <class Duration, typename = std::enable_if_t<Time::is_duration_v<Duration>>>
	TimeStamp<BaseBytes, FractionBytes, Num, Denom>& operator-=(const Duration& duration) {
		if (duration < Duration::zero()) {
			taiCounter += std::chrono::duration_cast<RawDuration>(-duration).count();
		} else {
			taiCounter -= std::chrono::duration_cast<RawDuration>(duration).count();
		}

		return *this;
	}

	/**
	 * Subtraction between two timestamps.
	 *
	 * Given 2 absolute moments in time, returns the relative duration between them.
	 * @tparam Duration The duration returned is equal to the RawDuration of the first timestamp,
	 * 					but it's signed instead of unsigned, so that negative results can be represented.
	 */
	template <
	    uint8_t BaseBytesIn, uint8_t FractionBytesIn, int NumIn = 1, int DenomIn = 1, // Template parameters of the 2nd timestamp
	    class Duration = std::chrono::duration<                                       // Create a new Duration based on our RawDuration...
	        typename std::make_signed<typename RawDuration::rep>::type,               // the Duration base type is equal to the RawDuration, but converted to signed from unsigned
	        typename RawDuration::period>>
	Duration operator-(const TimeStamp<BaseBytesIn, FractionBytesIn, NumIn, DenomIn>& operand) const {
		Duration myDuration = asDuration<Duration>();
		Duration operandDuration = operand.template asDuration<Duration>();

		return myDuration - operandDuration;
	}

	/**
	 * @name Comparison operators between timestamps
	 * @{
	 */
	template <class OtherTimestamp>
	bool operator<(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) < typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}

	template <class OtherTimestamp>
	bool operator>(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) > typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}

	template <class OtherTimestamp>
	bool operator==(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) == typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}

	template <class OtherTimestamp>
	bool operator!=(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) != typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}

	template <class OtherTimestamp>
	bool operator<=(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) <= typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}

	template <class OtherTimestamp>
	bool operator>=(const OtherTimestamp& timestamp) const {
		return RawDuration(taiCounter) >= typename OtherTimestamp::RawDuration(timestamp.taiCounter);
	}
	/**
	 * @}
	 */
};

namespace Time {
	using DefaultCUC = TimeStamp<4, 0, 1, 10>;

	/**
	 * Creates a custom literal to specify timestamp ticks.
	 *
	 * For example, this code:
	 * ```cpp
	 * Time::DefaultCUC timestamp(1000_t);
	 * ```
	 * will define a timestamp 1000 ticks from the epoch.
	 *
	 * The time amount of a "tick" is the period defined by the DefaultCUC::Ratio
	 */
	constexpr std::chrono::duration<uint32_t, DefaultCUC::Ratio> operator""_t(unsigned long long s) {
		return std::chrono::duration<uint32_t, DefaultCUC::Ratio>(s);
	}
} // namespace Time

#include "TimeStamp.tpp"
