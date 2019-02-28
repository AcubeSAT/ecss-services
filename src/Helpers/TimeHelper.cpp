#include "Helpers/TimeHelper.hpp"

uint64_t TimeHelper::implementCDSTimeFormat(struct tm* timeInfo) {
	/**
	 * Define the T-field. The total number of octets for the implementation of T-field is 6(2 for
	 * the `DAY` and 4 for the `ms of day`
	 */

	/**
	 * Elapsed seconds between a given date from `timeInfo`(UTC time) and epoch 1 January 1970
	 * 00:00:00(hours:minutes:seconds)
	 *
	 * @todo WARNING: evaluate the mktime() (Is it computer efficient and embedded
	 * systems-compliant?)
	 */
	time_t seconds = mktime(timeInfo);

	/**
	 * The `DAY` segment, 16 bits as defined from standard. Actually the days passed from an
	 * Agency-defined epoch,that it will be 1 January 1970(1/1/1970) 00:00:00(hours:minutes:seconds)
	 * This epoch is configured from the current implementation, using mktime() function
	 */
	uint16_t elapsedDays = static_cast<uint16_t>(seconds/86400);

	/**
	 * The `ms of day` segment, 32 bits as defined in standard. The `ms of the day` and DAY`
	 * should give the time passed from the defined epoch (1/1/1970)
	 */
	uint32_t msOfDay = static_cast<uint32_t >((seconds%86400)*1000);

	/**
	 * Define CDS time format
	 *
	 * Notes:
	 * Only the 48 bits of the 64 will be used for the timeFormat
	 *
	 * Shift operators have high priority. That's why we should do a type-casting first so we
	 * don't lose valuable bits
	*/
	uint64_t timeFormat = (static_cast<uint64_t>(elapsedDays) << 32 | msOfDay);


	return timeFormat;
}
