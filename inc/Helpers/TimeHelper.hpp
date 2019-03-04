#ifndef ECSS_SERVICES_TIMEHELPER_HPP
#define ECSS_SERVICES_TIMEHELPER_HPP

#include <cstdint>
#include <Message.hpp>

/**
 * The time and date provided from Real Time Clock(Real Time Clock)
 *
 * Note:
 * This struct is similar to the `struct tm` of <ctime> library but it is more embedded-friendly
 *
 * For the current implementation this struct takes dummy values, because RTC hasn't been
 * implemented
 */
struct TimeAndDate {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

/**
 * This class formats the spacecraft time and cooperates closely with the ST[09] time management.
 * The ECSS standard supports two time formats: the CUC and CSD that are described in
 * CCSDS 301.0-B-4 standard. The chosen time format is CDS and it is UTC-based(UTC: Coordinated
 * Universal Time)
 *
 * Note:
 * Since this code is UTC-based, the leap second correction must be made. The leap seconds that 
 * have been occured between timestamps should be considered if a critical time-difference is needed
 *
 */
class TimeHelper {
private:
	int SecondsPerMinute = 60;
	int SecondsPerHour = 3600;
	int SecondsPerDay = 86400;
	int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	/**
	 * @param year The year that will be examined if it is a leap year(366 days)
	 * @return if the /p is a leap year returns true and if it isn't returns false
	 */
	bool IsLeapYear(uint16_t year);

	/**
     * Make GMT(UTC) time. This is a reimplemented mktime() of <ctime> library in an
     * embedded systems way
     *
     * @param timeInfo the time information/data from the RTC(UTC format)
     * @return the elapsed seconds between a given UTC date(after the Unix epoch) and Unix epoch
     * (1/1/1970 00:00:00)
     * @todo change the epoch for computer-efficiency
     */
	uint32_t mkgmtime(struct TimeAndDate &timeInfo);

public:

	/**
	 * Generate the CDS time format(3.3 in CCSDS 301.0-B-4 standard)
	 *
	 * @details The CDS time format consists of two main fields: the time code preamble field
	 * (P-field) and the time specification field(T-field). The P-Field is the metadata for the
	 * T-Field. The T-Field is consisted of two segments: 1)the `DAY` and the 2)`ms of
	 * day` segments. The P-field won't be included in the code, because as the ECSS standards
	 * claims, it can be just implicitly declared.
	 * @param timeInfo is the data provided from RTC(Real Time Clock)
 	 * @todo check if we need to define other epoch than the 1 January 1970
 	 * @todo time security for critical time operations
 	 * @todo declare the implicit P-field
 	 * @todo check if we need milliseconds
	 */
	static uint64_t implementCDSTimeFormat(struct TimeAndDate &timeInfo);

	/**
	 * Dummy function created only to access mkgmtime for testing
	 *
	 * @todo Delete this function
	 */
	uint32_t get_mkgmtime(struct TimeAndDate &timeInfo) {
		return mkgmtime(timeInfo);
	}

};

// used to access `mkgmtime` function in the static `implementCDSTimeFormat` function
static TimeHelper mkgmtimeAccess;

#endif //ECSS_SERVICES_TIMEHELPER_HPP
