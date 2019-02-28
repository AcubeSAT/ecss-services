#ifndef ECSS_SERVICES_TIMEHELPER_HPP
#define ECSS_SERVICES_TIMEHELPER_HPP

#include <cstdint>
#include <ctime>
#include <Message.hpp>

/**
 * This class formats the spacecraft time and cooperates closely with the ST[09] time management.
 * The ECSS standard supports two time formats: the CUC and CSD that are described in
 * CCSDS 301.0-B-4 standard. The chosen time format is CDS and it is UTC-based(UTC: Coordinated
 * Universal Time)
 *
 * Note:
 * Since this code is UTC-based, the leap second correction must be made and leap seconds should
 * be considered in the difference between timestamps if a critical time-difference is needed
 *
 */
class TimeHelper {
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
	static uint64_t implementCDSTimeFormat(struct tm* timeInfo);
};

#endif //ECSS_SERVICES_TIMEHELPER_HPP
