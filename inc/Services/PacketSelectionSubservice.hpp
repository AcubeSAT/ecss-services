#pragma once

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/PacketStore.hpp"
#include "Helpers/AppProcessConfiguration.hpp"
#include "Service.hpp"
#include "etl/map.h"
#include "etl/vector.h"

class StorageAndRetrievalService;

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
public:
	/**
	 * The service type of the parent Storage and Retrieval Service (ST[15]).
	 *
	 * Kept as a separate constant (identical to StorageAndRetrievalService::ServiceType) because this header
	 * cannot include StorageAndRetrievalService.hpp without creating a circular dependency.
	 */
	inline static constexpr ServiceTypeNum ServiceType = 15;

	/**
	 * The type of the packet store ID, coming from the TC/TM[15,x] messages.
	 */
	using PacketStoreId = String<ECSSPacketStoreIdSize>;

private:
	/**
	 * Helper function that reads the packet store ID string from a TC/TM[15,x] message
	 */
	PacketStoreId readPacketStoreId(Message& message);

	/**
	 * Returns true if the specified packet store is present in packet stores of the main service.
	 */
	bool packetStoreExists(const PacketStoreId& packetStoreId);

	/**
	 * Reference to the parent Storage and Retrieval Service, used to query the existing packet stores.
	 */
	StorageAndRetrievalService& mainService;

	/**
	 * Initializes the packetStoreAppProcessConfig map and the controlledApplications vector with the initial
	 * values needed at system startup. Implemented by each platform.
	 */
	void initializePacketSelectionSubServiceStructures();

public:
	/**
	 * Constructor of the Packet Selection Subservice.
	 * @param parent: reference to the parent Storage and Retrieval Service.
	 */
	explicit PacketSelectionSubservice(StorageAndRetrievalService& parent) : mainService(parent) {
		serviceType = ServiceType;
		initializePacketSelectionSubServiceStructures();
	}

	/**
	 * Vector containing the IDs of the application processes controlled by the packet selection subservice.
	 *
	 * @note
	 * The subservice can only add report types to the storage-control configuration for application processes that
	 * are contained in this vector. It is the responsibility of the platform initialization code (or the tests) to
	 * populate it with the application process IDs that the subsystem controls, similarly to the
	 * controlledApplications vector of the RealTimeForwardingControlService (ST[14]).
	 */
	etl::vector<ApplicationProcessId, ECSSMaxControlledApplicationProcesses> controlledApplications;

	/**
	 * The map containing the application process configuration. The packet store ID is used as key, to access the application
	 * process definitions, the service type definitions and the message type definitions.
	 *
	 * @note
	 * The size of this object is known at compile time, but it is significant: each ApplicationProcessConfiguration
	 * holds up to ECSSMaxApplicationsServicesCombinations vectors of ECSSMaxReportTypeDefinitions message types, and
	 * this map holds one configuration per packet store (ECSSMaxPacketStores). With the current x86 definitions
	 * (4 packet stores, 50 app-service combinations, 20 report types), this map measures ~16.6 kB on x86-64
	 * (sizeof(ApplicationProcessConfiguration) = 4072 bytes). Keep this in mind when adjusting those definitions
	 * for the embedded platform.
	 *
	 * @note
	 * The report type definitions are the message types of each service. For example a message type for the
	 * 'ParameterStatisticsService' (ST04) is 'ParameterStatisticsService::MessageType::ParameterStatisticsReport'. The
	 * Packet Selection Subservice of the Storage and Retrieval Service (ST15) uses this map as a lookup table, to identify
	 * whether a requested quadruplet (packetStoreID->app->service->message type) is allowed to be stored in the packet stores
	 * of the Storage and Retrieval Service. The requested message type is only stored, if the requested packet store ID, the
	 * application process ID and service type already exist in the map, and the requested report type is located in the vector
	 * of report types, which corresponds to the packet store ID, appID and service type.
	 */
	etl::map<PacketStoreId, ApplicationProcessConfiguration, ECSSMaxPacketStores> packetStoreAppProcessConfig;

	/**
	 * TC[15,3] 'add report types to an application process storage control configuration'.
	 */
	void addReportTypesToAppProcessConfiguration(Message& request);

	/**
	 * TC[15,4] 'delete report types from the application process storage control configuration'
	 */
	void deleteReportTypesFromAppProcessConfiguration(Message& request);

	/**
	 * This function takes a TC[15,5] request 'report the content of the application process storage control
	 * configuration' as an argument and respond with a TM[15,6] 'application process storage-control configuration
	 * content report'
	 */
	void reportApplicationProcess(Message& request);
};
