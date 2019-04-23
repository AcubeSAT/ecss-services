#ifndef ECSS_SERVICES_TIMEHELPER_HPP
#define ECSS_SERVICES_TIMEHELPER_HPP

#include <cstdint>
#include <Message.hpp>
#include "TimeAndDate.hpp"

#define SECONDS_PER_MINUTE 60u
#define SECONDS_PER_HOUR 3600u
#define SECONDS_PER_DAY 86400u

/**
 * This class formats the spacecraft time and cooperates closely with the ST[09] time management.
 *
 * The ECSS standard supports two time formats: the CUC and CSD that are described in CCSDS
 * 301.0-B-4 standard.
 *
 * The CDS is UTC-based (UTC: Coordinated Universal Time). It consists of two main fields: the
 * time code preamble field (P-field) and the time specification field (T-field). The P-Field is the metadata for the
 * T-Field. The T-Field is consisted of two segments: 1) the `DAY` and the 2) `ms of day` segments. The P-field won't
 * be included in the code, because as the ECSS standards claims, it can be just implicitly declared.
 *
 * The CUC time format consists of two main fields: the time code preamble field (P-field) and the time specification
 * field(T-field). The T-Field contains the value of the time unit and the designer decides what the time unit will
 * be, so this is a subject for discussion. The recommended time unit from the standard is the second and it is
 * probably the best solution for accuracy.
 *
 * @notes
 * The defined epoch for both time formats is 1 January 1958 00:00:00
 *
 * Since CDS format is UTC-based, the leap second correction must be made. The leap seconds that
 * have been occurred between timestamps should be considered if a critical time-difference is
 * needed
 *
 */
class TimeHelper {
public:
	static constexpr uint8_t DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	TimeHelper() = default;

	/**
	 * @param year The year that will be examined if it is a leap year (366 days)
	 * @return if the \p year is a leap year returns true and if it isn't returns false
	 */
	static bool IsLeapYear(uint16_t year);

	/**
	 * Convert UTC date to elapsed seconds since Unix epoch (1/1/1970 00:00:00).
	 *
	 * This is a reimplemented mktime() of <ctime> library in an embedded systems way
	 *
	 * @note
	 * This function can convert UTC dates after 1 January 2019 to elapsed seconds since Unix epoch
	 *
	 * @param TimeInfo the time information/data from the RTC (UTC format)
	 * @return the elapsed seconds between a given UTC date (after the Unix epoch) and Unix epoch
	 * @todo check if we need to change the epoch to the recommended one from the standard, 1
	 * January 1958
	 */
	static uint32_t utcToSeconds(TimeAndDate& TimeInfo);

	/**
	 * Convert elapsed seconds since Unix epoch to UTC date.
	 *
	 * This is a reimplemented gmtime() of <ctime> library in an embedded systems way
	 *
	 * @note
	 * This function can convert elapsed seconds since Unix epoch to UTC dates after 1 January 2019
	 *
	 * @param seconds elapsed seconds since Unix epoch
	 * @return the UTC date based on the \p seconds
	 * @todo check if we need to change the epoch to the recommended one from the standard, 1
	 * January 1958
	 */
	static TimeAndDate secondsToUTC(uint32_t seconds);

	/**
	 * Generate the CDS time format (3.3 in CCSDS 301.0-B-4 standard).
	 *
	 * Converts a UTC date to CDS time format.
	 *
	 * @param TimeInfo is the data provided from RTC (UTC)
	 * @return TimeFormat the CDS time format. More specific, 48 bits are used for the T-field
	 * (16 for the `DAY` and 32 for the `ms of day`)
	 * @todo time security for critical time operations
	 * @todo declare the implicit P-field
	 */
	static uint64_t generateCDStimeFormat(struct TimeAndDate& TimeInfo);

	/**
	 * Parse the CDS time format (3.3 in CCSDS 301.0-B-4 standard)
	 *
	 * @param data time information provided from the ground segment. The length of the data is a
	 * fixed size of 48 bits
	 * @return the UTC date
	 */
	static TimeAndDate parseCDStimeFormat(const uint8_t* data);

	/**
	 * Generate the CUC time format (3.3 in CCSDS 301.0-B-4 standard).
	 *
	 * Converts a UTC date to CUC time format.
	 *
	 * @note
	 * The T-field is specified for the seconds passed from the defined epoch 1 January 1958. We use 4 octets(32
	 * bits) for the time unit(seconds) because 32 bits for the seconds are enough to count 136 years! But if we use 24
	 * bits for the seconds then it will count 0,5 years and this isn't enough. Remember we can use only integers
	 * numbers of octets for the time unit(second)
	 *
	 * @param TimeInfo is the data provided from RTC (UTC)
	 * @return TimeFormat the CUC time format. More specific, 32 bits are used for the T-field (seconds since 1/1/1958)
	 * @todo time security for critical time operations
	 * @todo declare the implicit P-field
	 */
	static uint32_t generateCUCtimeFormat(struct TimeAndDate& TimeInfo);

	/**
	 * Parse the CUC time format (3.3 in CCSDS 301.0-B-4 standard)
	 *
	 * @param data time information provided from the ground segment. The length of the data is a
	 * fixed size of 32 bits
	 * @return the UTC date
	 */
	static TimeAndDate parseCUCtimeFormat(const uint8_t* data);
};

#endif // ECSS_SERVICES_TIMEHELPER_HPP
