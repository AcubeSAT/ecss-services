#ifndef ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP
#define ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP

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
* @brief
* This service provides the capability to control the storage of TM messages to the packet stores of the
* Storage and Retrieval Service. It contains definitions each one indicating whether a specific TM message
* should or should not be stored into the packet stores. For a specific TM message, if the packet selection
* includes a definition related to that message, it means that it can be stored into the packet stores.
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
	explicit PacketSelectionSubservice(etl::map<PacketStoreId, PacketStore, ECSSMaxPacketStores>& mainServicePacketStores);

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

/**
 * Implementation of ST[15] Storage and Retrieval Service, as defined in ECSS-E-ST-70-41C.
 *
 * This Service:
 * - provides the capability to select reports generated by other services and store them into packet stores.
 * - allows the ground system to manage the reports in the packet stores and request their downlink.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class StorageAndRetrievalService : public Service {
public:
	/**
	 * The type of timestamps that the Storage and Retrieval Subservice assigns to each incoming packet.
	 */
	enum TimeStampType : uint8_t { StorageBased = 0,
		                           PacketBased = 1 };

	/**
	 * Different types of packet retrieval from a packet store, relative to a specified time-tag.
	 */
	enum TimeWindowType : uint8_t { FromTagToTag = 0,
		                            AfterTimeTag = 1,
		                            BeforeTimeTag = 2 };

	/**
	 * The type of timestamps that the subservice sets to each incoming telemetry packet.
	 */
	const TimeStampType timeStamping = PacketBased;

	typedef String<ECSSPacketStoreIdSize> packetStoreId;

	/**
	 * All packet stores, held by the Storage and Retrieval Service. Each packet store has its ID as key.
	 */
	etl::map<packetStoreId, PacketStore, ECSSMaxPacketStores> packetStores;

	/**
	 * Helper function that reads the packet store ID string from a TM[15] message
	 */
	inline String<ECSSPacketStoreIdSize> readPacketStoreId(Message& message);

private:
	/**
	 * Helper function that, given a time-limit, deletes every packet stored in the specified packet-store, up to the
	 * requested time.
	 *
	 * @param packetStoreId required to access the correct packet store.
	 * @param timeLimit the limit until which, packets are deleted.
	 */
	void deleteContentUntil(const String<ECSSPacketStoreIdSize>& packetStoreId, uint32_t timeLimit);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, that fall between the two specified
	 * time-tags as per 6.15.3.8.4.d(1) of the standard.
	 *
	 * @param request used to read the time-tags, the packet store IDs and to raise errors.
	 */
	void copyFromTagToTag(Message& request);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is after the
	 * specified time-tag as per 6.15.3.8.4.d(2) of the standard.
	 *
	 * @param request used to read the time-tag, the packet store IDs and to raise errors.
	 */
	void copyAfterTimeTag(Message& request);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is before the
	 * specified time-tag as per 6.15.3.8.4.d(3) of the standard.
	 *
	 * @param request used to raise errors.
	 */
	void copyBeforeTimeTag(Message& request);

	/**
	 * Checks if the two requested packet stores exist.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool checkPacketStores(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
	                       const String<ECSSPacketStoreIdSize>& toPacketStoreId, Message& request);

	/**
	 * Checks whether the time window makes logical sense (end time should be after the start time)
	 *
	 * @param request used to raise errors.
	 */
	static bool checkTimeWindow(uint32_t startTime, uint32_t endTime, Message& request);

	/**
	 * Checks if the destination packet store is empty, in order to proceed with the copying of packets.
	 *
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content. Needed for error
	 * checking.
	 * @param request used to raise errors.
	 */
	bool checkDestinationPacketStore(const String<ECSSPacketStoreIdSize>& toPacketStoreId, Message& request);

	/**
	 * Checks if there are no stored timestamps that fall between the two specified time-tags.
	 *
	 * @param fromPacketStoreId  the source packet store, whose content is to be copied. Needed for error checking.
	 * @param request used to raise errors.
	 *
	 * @note
	 * This function assumes that `startTime` and `endTime` are valid at this point, so any necessary error checking
	 * regarding these variables, should have already occurred.
	 */
	bool noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId, uint32_t startTime,
	                             uint32_t endTime, Message& request);

	/**
	 * Checks if there are no stored timestamps that fall between the two specified time-tags.
	 *
	 * @param isAfterTimeTag true indicates that we are examining the case of AfterTimeTag. Otherwise, we are referring
	 * to the case of BeforeTimeTag.
	 * @param request used to raise errors.
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 */
	bool noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId, uint32_t timeTag,
	                             Message& request, bool isAfterTimeTag);

	/**
	 * Performs all the necessary error checking for the case of FromTagToTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedFromTagToTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
	                        const String<ECSSPacketStoreIdSize>& toPacketStoreId, uint32_t startTime,
	                        uint32_t endTime, Message& request);

	/**
	 * Performs all the necessary error checking for the case of AfterTimeTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedAfterTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
	                        const String<ECSSPacketStoreIdSize>& toPacketStoreId, uint32_t startTime,
	                        Message& request);

	/**
	 * Performs all the necessary error checking for the case of BeforeTimeTag copying of packets.
	 *
	 * @param fromPacketStoreId the source packet store, whose content is to be copied.
	 * @param toPacketStoreId  the target packet store, which is going to receive the new content.
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedBeforeTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
	                         const String<ECSSPacketStoreIdSize>& toPacketStoreId, uint32_t endTime,
	                         Message& request);

	/**
	 * Performs the necessary error checking for a request to start the by-time-range retrieval process.
	 *
	 * @param request used to raise errors.
	 * @return true if an error has occurred.
	 */
	bool failedStartOfByTimeRangeRetrieval(const String<ECSSPacketStoreIdSize>& packetStoreId, Message& request);

	/**
	 * Forms the content summary of the specified packet-store and appends it to a report message.
	 */
	void createContentSummary(Message& report, const String<ECSSPacketStoreIdSize>& packetStoreId);

public:
	inline static const uint8_t ServiceType = 15;

	enum MessageType : uint8_t {
		EnableStorageInPacketStores = 1,
		DisableStorageInPacketStores = 2,
		AddReportTypesToAppProcessConfiguration = 3,
		StartByTimeRangeRetrieval = 9,
		DeletePacketStoreContent = 11,
		ReportContentSummaryOfPacketStores = 12,
		PacketStoreContentSummaryReport = 13,
		ChangeOpenRetrievalStartingTime = 14,
		ResumeOpenRetrievalOfPacketStores = 15,
		SuspendOpenRetrievalOfPacketStores = 16,
		AbortByTimeRangeRetrieval = 17,
		ReportStatusOfPacketStores = 18,
		PacketStoresStatusReport = 19,
		CreatePacketStores = 20,
		DeletePacketStores = 21,
		ReportConfigurationOfPacketStores = 22,
		PacketStoreConfigurationReport = 23,
		CopyPacketsInTimeWindow = 24,
		ResizePacketStores = 25,
		ChangeTypeToCircular = 26,
		ChangeTypeToBounded = 27,
		ChangeVirtualChannel = 28
	};

	StorageAndRetrievalService() = default;

	/**
	 * The packet selection sub-service of the Storage and Retrieval service.
	 */
	PacketSelectionSubservice packetSelection = PacketSelectionSubservice(packetStores);

	/**
	 * Adds new packet store into packet stores.
	 */
	void addPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId, const PacketStore& packetStore);

	/**
	 * Adds telemetry to the specified packet store and timestamps it.
	 */
	void addTelemetryToPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId, uint32_t timestamp);

	/**
	 * Deletes the content from all the packet stores.
	 */
	void resetPacketStores();

	/**
	 * Returns the number of existing packet stores.
	 */
	uint16_t currentNumberOfPacketStores();

	/**
	 * Returns the packet store with the specified packet store ID.
	 */
	PacketStore& getPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId);

	/**
	 * Returns true if the specified packet store is present in packet stores.
	 */
	bool packetStoreExists(const String<ECSSPacketStoreIdSize>& packetStoreId);

	/**
	 * Given a request that contains a number N, followed by N packet store IDs, this method calls function on every
	 * packet store. Implemented to reduce duplication. If N = 0, then function is applied to all packet stores.
	 * Incorrect packet store IDs are ignored and generate an error.

	 * @param function the job to be done after the error checking.
	 */
	void executeOnPacketStores(Message& request, const std::function<void(PacketStore&)>& function);

	/**
	 * TC[15,1] request to enable the packet stores' storage function
	 */
	void enableStorageFunction(Message& request);

	/**
	 * TC[15,2] request to disable the packet stores' storage function
	 */
	void disableStorageFunction(Message& request);

	/**
	 * TC[15,9] start the by-time-range retrieval of packet stores
	 */
	void startByTimeRangeRetrieval(Message& request);

	/**
	 * TC[15,11] delete the packet store content up to the specified time
	 */
	void deletePacketStoreContent(Message& request);

	/**
	 * This function takes a TC[15,12] 'report the packet store content summary' as argument and responds with a TM[15,
	 * 13] 'packet store content summary report' report message.
	 */
	void packetStoreContentSummaryReport(Message& request);

	/**
	 * TC[15,14] change the open retrieval start time tag
	 */
	void changeOpenRetrievalStartTimeTag(Message& request);

	/**
	 * TC[15,15] resume the open retrieval of packet stores
	 */
	void resumeOpenRetrievalOfPacketStores(Message& request);

	/**
	 * TC[15,16] suspend the open retrieval of packet stores
	 */
	void suspendOpenRetrievalOfPacketStores(Message& request);

	/**
	 * TC[15,17] abort the by-time-range retrieval of packet stores
	 */
	void abortByTimeRangeRetrieval(Message& request);

	/**
	 * This function takes a TC[15,18] 'report the status of packet stores' request as argument and responds with a
	 * TM[15,19] 'packet stores status' report message.
	 */
	void packetStoresStatusReport(Message& request);

	/**
	 * TC[15,20] create packet stores
	 */
	void createPacketStores(Message& request);

	/**
	 * TC[15,21] delete packet stores
	 */
	void deletePacketStores(Message& request);

	/**
	 * This function takes a TC[15,22] 'report the packet store configuration' as argument and responds with a TM[15,
	 * 23] 'packet store configuration report' report message.
	 */
	void packetStoreConfigurationReport(Message& request);

	/**
	 * TC[15,24] copy the packets contained into a packet store, selected by the time window
	 */
	void copyPacketsInTimeWindow(Message& request);

	/**
	 * TC[15,25] resize packet stores
	 */
	void resizePacketStores(Message& request);

	/**
	 * TC[15,26] change the packet store type to circular
	 */
	void changeTypeToCircular(Message& request);

	/**
	 * TC[15,27] change the packet store type to bounded
	 */
	void changeTypeToBounded(Message& request);

	/**
	 * TC[15,28] change the virtual channel used by a packet store
	 */
	void changeVirtualChannel(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param request Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& request);
};

#endif
