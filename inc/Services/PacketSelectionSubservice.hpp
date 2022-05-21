#ifndef ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP
#define ECSS_SERVICES_PACKETSELECTIONSUBSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/AllMessageTypes.hpp"
#include "Helpers/PacketSelectionConfiguration.hpp"
#include "Helpers/PacketStore.hpp"
#include "Service.hpp"
#include "etl/map.h"

/**
* Implementation of the Packet Selection Subservice of the ST[15] Storage and Retrieval Service.
*
* This service provides the capability to control the storage of TM messages to the packet stores of the
* Storage and Retrieval Service. It contains definitions each one indicating whether a specific TM message
* should or should not be stored into the packet stores. For a specific TM message, if the packet selection
* includes a definition related to that message, it means that it can be stored into the packet stores.
*
* @author Konstantinos Petridis <petridkon@gmail.com>
*/
class PacketSelectionSubservice {
private:
   /**
	* Helper function that reads the packet store ID string from a TM[15] message
	*/
   inline String<ECSSPacketStoreIdSize> readPacketStoreId(Message& message);

   /**
	* Returns true if the specified packet store is present in packet stores of the main service.
	*/
   bool packetStoreExists(const String<ECSSPacketStoreIdSize>& packetStoreId);

   /**
   * Adds all report types of the specified application process definition, to the application process configuration.
   */
   void addAllReportsOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

   /**
   * Adds all report types of the specified service type, to the application process configuration.
   */
   void addAllReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType);

   /**
   * Counts the number of service types, stored for the specified packet store ID and application process.
   */
   uint8_t countServicesOfApplication(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

   /**
   * Counts the number of report types, stored for the specified service type.
   */
   uint8_t countReportsOfService(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType);

   /**
   * Checks whether the specified message type already exists in the specified packet store ID, application process and service
   * type definition.
   */
   bool reportExistsInAppProcessConfiguration(const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

   /**
   * Performs the necessary error checking/logging for a specific packet store ID and application process ID. Also, skips the necessary
   * bytes from the request message, in case of an invalid request.
   *
   * @return True: if the application is valid and passes all the necessary error checking.
   */
   bool checkApplicationOfAppProcessConfig(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t numOfServices);

   /**
   * Checks if the specified application process is controlled by the Service and returns true if it does.
   */
   bool isAppControlled(Message& request, uint8_t applicationId);

   /**
   * Checks if all service types are allowed already, i.e. if the application process contains no service type
   * definitions.
   */
   bool allServiceTypesAllowed(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

   /**
   * Checks if the maximum number of service type definitions per application process is reached.
   */
   bool maxServiceTypesReached(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID);

   /**
   * Performs the necessary error checking/logging for a specific service type. Also, skips the necessary bytes
   * from the request message, in case of an invalid request.
   *
   * @return True: if the service type is valid and passes all the necessary error checking.
   */
   bool checkService(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t numOfMessages);

   /**
   * Checks if the maximum number of report type definitions per service type definition is reached.
   */
   bool maxReportTypesReached(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType);

   /**
   * Checks if the maximum number of message types that can be contained inside a service type definition, is
   * already reached.
   *
   * @return True: if the message type is valid and passes all the necessary error checking.
   */
   bool checkMessage(Message& request, const String<ECSSPacketStoreIdSize>& packetStoreID, uint8_t applicationID, uint8_t serviceType, uint8_t messageType);

public:
   typedef String<ECSSPacketStoreIdSize> PacketStoreId;

   /**
	* Reference to the packet stores of the storage and retrieval service.
	*/
   etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& packetStores;

   /**
	* Constructor of the Packet Selection Subservice.
	* @param mainServicePacketStores: reference to the packet stores of the storage and retrieval service.
	*/
   PacketSelectionSubservice(etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& mainServicePacketStores);

   /**
   * Vector containing the IDs of the application processes controlled by the packet selection subservice.
   */
   etl::vector<uint8_t, ECSSMaxControlledApplicationProcesses> controlledApplications;

   /**
   * Contains the definitions that the packet selection subservice holds, regarding TM packets coming from
   * application processes.
   */
   ApplicationProcessConfiguration applicationProcessConfiguration;

   /**
   * TC[15,3] 'add report types to an application process storage control configuration'.
   */
   void addReportTypesToAppProcessConfiguration(Message& request);
};

#endif
