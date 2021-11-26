#ifndef ECSS_SERVICES_TIME_HPP
#define ECSS_SERVICES_TIME_HPP

#include <cstdint>
#include <algorithm>
#include "macros.hpp"
#include <etl/array.h>
#include "Helpers/TimeFormats.hpp"

// SEE CCSDS 301.0-B-4
// CUSTOM EPOCH FOR ALL ACUBESAT TIMESTAMPS IS 01 JAN 2020, EXCEPT UTC (UNIX)

///////////// CLASS DECLARATION ////////////////
/**
 * A class that represents an instant in time, with convenient conversion
 * to and from usual time and date representations
 *
 * @note
 * This class uses internally TAI time, and handles UTC leap seconds at conversion to and
 * from UTC time system.
 */
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
class TimeStamp {
private:
	static_assert(seconds_counter_bytes + fractional_counter_bytes <= 8,
	              "Currently, this class is not suitable for storage on internal counter larger than uint64_t");
	typedef typename std::conditional < seconds_counter_bytes < 4 &&
	    fractional_counter_bytes<3, uint8_t, uint16_t>::type CUC_header_t;
	typedef typename std::conditional<(seconds_counter_bytes + fractional_counter_bytes) < 4, uint32_t, uint64_t>::type
	    tai_counter_t;
	tai_counter_t tai_counter;
	CUC_header_t CUC_header = build_CUC_header<CUC_header_t, seconds_counter_bytes, fractional_counter_bytes>();

public:
	/**
	 * Initialize the instant at epoch
	 *
	 */
	TimeStamp() : tai_counter(0){};

	/**
	 * Initialize the instant from a duration from epoch in TAI
	 *
	 * @param seconds an integer number of seconds from AcubeSAT custom epoch
	 */
	TimeStamp(int tai_seconds_from_AcubeSAT_epoch)
	    : tai_counter(static_cast<tai_counter_t>(tai_seconds_from_AcubeSAT_epoch) << 8 * fractional_counter_bytes){};

	/**
	 * Initialize the instant from the bytes of a CUC time stamp
	 *
	 * @param timestamp a complete CUC time stamp including header, of the maximum possible size, zero padded to the
	 * right
	 */
	TimeStamp(etl::array<uint8_t, MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP> timestamp);

	/**
	 * Initialize the instant from a UTC timestamp struct
	 *
	 * @param timestamp a UTC timestamp, from Unix Epoch
	 */
	TimeStamp(UTC_Timestamp timestamp);

	/**
	 * Initialize the instant from an AcubeSAT CDS timestamp
	 *
	 * @param timestamp a CDS timestamp, in the format from DDJF_TTC, from epoch 2020.01.01
	 */
	TimeStamp(AcubeSAT_CDS_timestamp timestamp);

	/**
	 * Get the representation as seconds from epoch in TAI
	 *
	 * @return the seconds elapsed in TAI since 1 Jan 1958, cut to the integer part
	 */
	const int as_TAI_seconds();

	/**
	 * Get the representation as CUC formatted bytes
	 *
	 * @return the instant, represented in the CCSDS CUC format
	 */
	const etl::array<uint8_t, MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP> as_CUC_timestamp();

	/**
	 * Get the representation as a UTC timestamp
	 *
	 * @return the instant, represented in the structure that holds UTC timestamps
	 */
	const UTC_Timestamp as_UTC_timestamp();

	/**
	 * Compare two instants.
	 *
	 * @param TimeStamp the date that will be compared with the pointer `this`
	 * @return true if the condition is satisfied
	 */
	bool operator<(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
	bool operator>(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
	bool operator==(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
	bool operator!=(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
	bool operator<=(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
	bool operator>=(const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const;
};
////////////////////////////////////////////////

////// TEMPLATED METHODS INSTANTIATION /////////
#include "TimeStamp.tpp"
////////////////////////////////////////////////

#endif
