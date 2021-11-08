#ifndef ECSS_SERVICES_HOUSEKEEPINGREPORTCONFIGURATION_HPP
#define ECSS_SERVICES_HOUSEKEEPINGREPORTCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * This is the housekeeping parameter report configuration as defined in ST[15] of the standard.
 *
 * @brief it contains all the necessary housekeeping parameter report definitions, which contain the application
 * process ID, each followed by a list of the housekeeping structure IDs. These IDs define which housekeeping TM
 * packets are going to be allowed in the packet store.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class HousekeepingReportConfiguration {
public:
	typedef std::pair <uint16_t, uint16_t> appKey;
	typedef etl::vector <uint16_t, ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL> housekeepingStructIds;

	/**
	 * This map contains the necessary definitions' hierarchy (application process -> housekeeping struct ids). It
	 * stores the definitions in 2 layers.
	 *
	 * Layer-1:
	 * Map containing pairs (first = packetStoreId, second = AppProcessId), each as a key to a vector. Every pair
	 * contains the application process type definitions.
	 *
	 * Layer-2:
	 * Vector containing the housekeeping structure IDs.
	 */
	etl::map <appKey, housekeepingStructIds, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> housekeepingReportDefinitions;
};

#endif