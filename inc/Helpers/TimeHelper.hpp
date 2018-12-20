#ifndef ECSS_SERVICES_TIMEHELPER_HPP
#define ECSS_SERVICES_TIMEHELPER_HPP

#include <cstdint>
#include <Message.hpp>

/**
 * This class formats the spacecraft time and cooperates closely with the ST[09] time management.
 * The ECSS standard supports two time formats: the CUC and CSD that are described in
 * CCSDS 301.0-B-4 standard
 * The chosen time format is CUC. The reasons for this selection are the followings:
 * 1)It is more flexible from the CSD. The designer is free to decide how much memory will use
 * for the time unit and what that time unit will be(seconds, minutes, hours etc.).
 * 2)It can use TAI(international atomic time) as reference time scale. So there is no need
 * to worry about leap seconds(code UTC-based)
 *
 * Note: The implementation of the time formats are in general RTC-dependent. First, we need to
 * get the time data from the RTC, so we know what time is it and then format it!
 */
class TimeHelper {
public:
	/**
	 * Generate the CUC time format
	 *
	 * @details The CUC time format consists of two main fields: the time code preamble field
	 * (P-field) and the time specification field(T-field). The P-Field is the metadata for the
	 * T-Field. The T-Field contains the value of the time unit and the designer decides what the
	 * time unit will be, so this is a subject for discussion. The recommended time unit from the
	 * standard is the second and it is probably the best solution for accuracy.
	 * @param seconds the seconds provided from the RTC. This function in general should have
	 * parameters corresponding with the RTC. For the time being we assume that the RTC has a
	 * 32-bit counter that counts seconds(the RTC in Nucleo F103RB!)
 	 * @todo check if we need milliseconds(fractions of the time unit)
 	 * @todo the time unit should be declared in the metadata. But how?
 	 * @todo check if we need to define other epoch than the 1 January 1958
 	 * @todo time security for critical time operations
	 */
	static uint64_t implementCUCTimeFormat(uint32_t seconds);
};

#endif //ECSS_SERVICES_TIMEHELPER_HPP
