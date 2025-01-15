#ifndef ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP
#define ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/ForwardControlConfiguration.hpp"
#include "Helpers/PacketStore.hpp"
#include "Service.hpp"
#include "etl/map.h"

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
	inline String <ECSSPacketStoreIdSize> readPacketStoreId(Message& message);

	/**
	 * Returns true if the specified packet store is present in packet stores of the main service.
	 */
	bool packetStoreExists(const String <ECSSPacketStoreIdSize>& packetStoreId);

	/**
     * Adds all report types of the specified application process definition, to the application process configuration.
     */
	void addAllReportsOfApplication(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

	/**
     * Adds all report types of the specified service type, to the application process configuration.
     */
	void addAllReportsOfService(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
		uint8_t serviceType);

	/**
     * Counts the number of service types, stored for the specified packet store ID and application process.
     */
	uint8_t countServicesOfApplication(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

	/**
     * Counts the number of report types, stored for the specified service type.
     */
	uint8_t countReportsOfService(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
		uint8_t serviceType);

	/**
     * Checks whether the specified message type already exists in the specified packet store ID, application process and service
     * type definition.
     */
	bool reportExistsInAppProcessConfiguration(const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

	/**
     * Performs the necessary error checking/logging for a specific packet store ID and application process ID. Also, skips the necessary
     * bytes from the request message, in case of an invalid request.
     *
     * @return True: if the application is valid and passes all the necessary error checking.
     */
	bool checkApplicationOfAppProcessConfig(Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID, uint8_t numOfServices);

	/**
     * Checks if the specified application process is controlled by the Service and returns true if it does.
     */
	bool isAppControlled(const Message& request, uint8_t applicationId);

	/**
     * Checks if all service types are allowed already, i.e. if the application process contains no service type
     * definitions.
     */
	bool allServiceTypesAllowed(const Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID);

	/**
     * Checks if the maximum number of service type definitions per application process is reached.
     */
	bool maxServiceTypesReached(const Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID);

	/**
     * Performs the necessary error checking/logging for a specific service type. Also, skips the necessary bytes
     * from the request message, in case of an invalid request.
     *
     * @return True: if the service type is valid and passes all the necessary error checking.
     */
	bool checkService(Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
		uint8_t numOfMessages);

	/**
     * Checks if the maximum number of report type definitions per service type definition is reached.
     */
	bool maxReportTypesReached(const Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID, uint8_t serviceType);

	/**
     * Checks if the maximum number of message types that can be contained inside a service type definition, is
     * already reached.
     *
     * @return True: if the message type is valid and passes all the necessary error checking.
     */
	bool checkMessage(const Message& request, const String <ECSSPacketStoreIdSize>& packetStoreID,
		uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

	typedef String <ECSSPacketStoreIdSize> PacketStoreId;

	/**
	 * Reference to the packet stores of the storage and retrieval service.
	 */
	etl::map <PacketStoreId, PacketStore, ECSSMaxPacketStores>& packetStores;

public:
	/**
	 * Constructor of the Packet Selection Subservice.
	 * @param mainServicePacketStores: reference to the packet stores of the storage and retrieval service.
	 */
	explicit PacketSelectionSubservice(etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& mainServicePacketStores) : packetStores(mainServicePacketStores) {}

	/**
     * Vector containing the IDs of the application processes controlled by the packet selection subservice.
     */
	etl::vector <uint8_t, ECSSMaxControlledApplicationProcesses> controlledApplications;

	typedef String <ECSSPacketStoreIdSize> PacketStoreID;
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
	etl::map <PacketStoreID, ApplicationProcessConfiguration, ECSSMaxPacketStores> applicationProcessConfiguration;

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
	void deleteAllReportsOfService(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID,
		uint8_t serviceType);

	/**
   	 * Deletes all report types of the specified application process definition, to the application process configuration.
   	 */
	void deleteAllReportsOfApplication(const String <ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

	/**
     * This function takes a TC[15,5] request 'report the content of the application process storage control
     * configuration' as an argument and respond with a TM[15,6] 'application process storage-control configuration
     * content report'
     */
	void reportApplicationProcess(Message& request);
};

#endif
