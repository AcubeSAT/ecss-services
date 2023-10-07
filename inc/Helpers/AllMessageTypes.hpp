#ifndef ECSS_SERVICES_ALLMESSAGETYPES_HPP
#define ECSS_SERVICES_ALLMESSAGETYPES_HPP

#include "ECSS_Definitions.hpp"
#include <etl/map.h>
#include <etl/vector.h>

/**
 * Namespace containing all the message types for every service type.
 */
namespace AllMessageTypes {
	/**
	 * Map containing all the message types, per service. The key is the ServiceType and the value,
	 * an etl vector containing the message types.
	 */
	extern const etl::map<uint8_t, etl::vector<uint8_t, ECSSMaxReportTypeDefinitions>, ECSSMaxServiceTypeDefinitions> MessagesOfService;

} // namespace AllMessageTypes

#endif
