#ifndef ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP
#define ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/PacketStore.hpp"
#include "Helpers/AppProcessConfiguration.hpp"
#include "Service.hpp"
#include "etl/map.h"
#include "etl/vector.h"

/**
 * Implementation of the Packet Selection Subservice of the ST[15] Storage and Retrieval Service.
 *
 * This subservice provides the capability to control the storage of TM messages to the packet stores of the
 * Storage and Retrieval Service. It contains definitions each indicating whether a specific TM message
 * should or should not be stored into the packet stores. For a specific TM message, if the packet selection
 * includes a definition related to that message, it means that it can be stored into the packet stores.
 *
 * @authors Konstantinos Petridis <petridkon@gmail.com>, Xhulio Luli <lulixhulio@gmail.com>
 */
class PacketSelectionSubservice : public Service {
private:
	/**
	 * Helper function that reads the packet store ID string from a TC/TM[15,x] message
	 */
	inline PacketStoreId readPacketStoreId(Message& message);

	/**
	 * Returns true if the specified packet store is present in packet stores of the main service.
	 */
	bool packetStoreExists(const PacketStoreId& packetStoreId);

	/**
	 * Reference to the packet stores of the storage and retrieval service.
	 */
	etl::map <PacketStoreId, PacketStore, ECSSMaxPacketStores>& packetStores;

	/**
	 * Function to be implemented by each platform that initializes the packetStoreAppProcessConfig map
	 */
	void initializePacketSelectionSubServiceStructures();

public:

	/**
	 * Constructor of the Packet Selection Subservice.
	 * @param mainServicePacketStores: reference to the packet stores of the storage and retrieval service.
	 */
	explicit PacketSelectionSubservice(etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& mainServicePacketStores) : packetStores(mainServicePacketStores) {
		initializePacketSelectionSubServiceStructures();
	}

	/**
     * Vector containing the IDs of the application processes controlled by the packet selection subservice.
     */
	etl::vector <ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications;

	/**
	 * The map containing the application process configuration. The packet store ID is used as key, to access the application
	 * process definitions, the service type definitions and the message type definitions.
	 *
	 * @note
	 * The report type definitions are  the message types of each service. For example a message type for the
	 * 'ParameterStatisticsService' (ST04) is 'ParameterStatisticsService::MessageType::ParameterStatisticsReport'. The
	 * Packet Selection Subservice of the Storage and Retrieval Service (ST15) uses this map as a lookup table, to identify
	 * whether a requested quadruplet (packetStoreID->app->service->message type) is allowed to be stored in the packet stores
	 * of the Storage and Retrieval Service. The requested message type is only stored, if the requested packet store ID, the
	 * application process ID and service type already exist in the map, and the requested report type is located in the vector
	 * of report types, which corresponds to the packet store ID, appID and service type.
	 */
	etl::map <PacketStoreId, ApplicationProcessConfiguration, ECSSMaxPacketStores> packetStoreAppProcessConfig;

	/**
     * TC[15,3] 'add report types to an application process storage control configuration'.
     */
	void addReportTypesToAppProcessConfiguration(Message& request);

	/**
     * TC[15,4] 'delete report types from the application process storage control configuration'
     */
	void deleteReportTypesFromAppProcessConfiguration(Message& request);

	/**
   	 * Deletes all report types of the specified service type, to the application process configuration.
   	 */
	void deleteAllReportsOfService(const PacketStoreId& packetStoreID, ApplicationProcessId applicationID,
		ServiceTypeNum serviceType);

	/**
   	 * Deletes all report types of the specified application process definition, to the application process configuration.
   	 */
	void deleteAllReportsOfApplication(const PacketStoreId& packetStoreID, ApplicationProcessId applicationID);

	/**
     * This function takes a TC[15,5] request 'report the content of the application process storage control
     * configuration' as an argument and respond with a TM[15,6] 'application process storage-control configuration
     * content report'
     */
	void reportApplicationProcess(Message& request);
};

#endif
