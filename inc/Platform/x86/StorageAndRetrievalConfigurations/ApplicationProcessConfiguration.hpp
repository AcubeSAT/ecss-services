#ifndef ECSS_SERVICES_APPLICATIONPROCESSCONFIGURATION_HPP
#define ECSS_SERVICES_APPLICATIONPROCESSCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "PacketSelectionDefinitions.hpp"
#include <etl/String.hpp>
#include "etl/map.h"
#include "etl/vector.h"

/**
 * This is the Application Process configuration as defined in ST[15] of the standard.
 *
 * @brief it contains all the necessary application process definitions, followed by the service type definitions,
 * and finally the message type definitions. This configuration is meant to decide which packets are going to be
 * stores into the packet stores, depending on the existing definitions in the configuration.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class ApplicationProcessConfiguration {
public:
	/**
	 * Map containing the application process definitions (key), each followed by the list of service type definitions.
	 */
	typedef etl::map<uint8_t, ApplicationProcessDefinition, ECSS_MAX_CONTROLLED_APPLICATION_PROCESSES>
	    applicationProcessDefinitions;

	/**
	 * Map containing the packet store IDs (key), each followed by the application process definition.
	 */
	etl::map<String<ECSS_MAX_PACKET_STORE_ID_SIZE>, applicationProcessDefinitions, ECSS_MAX_PACKET_STORES> definitions;
};

#endif
