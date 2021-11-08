#ifndef ECSS_SERVICES_APPLICATIONPROCESSCONFIGURATION_HPP
#define ECSS_SERVICES_APPLICATIONPROCESSCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * This is the application process configuration as defined in ST[15] of the standard.
 *
 * @brief it contains all the necessary application process definitions, followed by the service type definitions,
 * and finally the message type definitions. This configuration is meant to decide which packets are going to be
 * stores into the packet stores, depending on the existing definitions in the configuration.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class ApplicationProcessConfiguration {
public:
	typedef std::pair <uint16_t, uint16_t> appKey;
	typedef etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> messageTypeDefinitions;
	typedef etl::map <uint16_t, messageTypeDefinitions, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> serviceTypeDefinitions;

	/**
	 * This map contains the necessary definitions' hierarchy (application process -> service -> message). It stores
	 * the definitions in 3 layers.
	 *
	 * Layer-1:
	 * Map containing a pair (first = packetStoreId, second = AppProcessId) as a key to the 2nd map. This pair
	 * contains the application process type definitions.
	 *
	 * Layer-2:
	 * Map containing service type Ids as keys, each pointing to a vector.
	 *
	 * Layer-3:
	 * Vector containing the message type definitions (message type ids).
	 */
	etl::map<appKey, serviceTypeDefinitions, ECSS_MAX_APPLICATION_PROCESS_DEFINITIONS> applicationProcessDefinitions;
};

#endif