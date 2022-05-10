#ifndef ECSS_SERVICES_PACKETSELECTIONCONFIGURATION_HPP
#define ECSS_SERVICES_PACKETSELECTIONCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/Parameter.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * The Application Process configuration. It's basically a map, storing a vector of report type definitions for each
 * pair of (applicationID, serviceType). It contains definitions, which indicate whether a telemetry message, produced
 * by a service, inside an application process (subsystem), should be forwarded to the ground station.
 */
class ApplicationProcessConfiguration {
public:
	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ReportTypeDefinitions;

	/**
	 * This is the key for the application process configuration map. It contains a pair with the applicationID and
	 * the serviceType.
	 */
	typedef std::pair<uint8_t, uint8_t> AppServiceKey;

	/**
	 * Map containing the report type definitions. Each application process has its own ID. The combination of the
	 * application ID and the service type is used as a key to provide access to the list of report type definitions.
	 */
	typedef etl::map<AppServiceKey, ReportTypeDefinitions, ECSSMaxApplicationsServicesCombinations> AppServiceDefinitions;

	typedef String<ECSSPacketStoreIdSize> PacketStoreID;

	/**
	 * The map containing the application process configuration. The packet store ID is used as key, to access the application
	 * process definitions, the service type definitions and the message type definitions.
	 *
	 * @note
	 * The report type definitions are basically the message types of each service. For example a message type for the
	 * 'ParameterStatisticsService' (ST04) is 'ParameterStatisticsService::MessageType::ParameterStatisticsReport'. The
	 * Packet Selection Subservice of the Storage and Retrieval Service (ST15) uses this map as a lookup table, to identify
	 * whether a requested quadruplet (packetStoreID->app->service->message type) is allowed to be stored in the packet stores
	 * of the Storage and Retrieval Service. The requested message type is only stored, if the requested packet store ID, the
	 * application process ID and service type already exist in the map, and the requested report type is located in the vector
	 * of report types, which corresponds to the packet store ID, appID and service type.
	 */
	etl::map<PacketStoreID, AppServiceDefinitions, ECSSMaxPacketStores> definitions;

	ApplicationProcessConfiguration() = default;
};

#endif
