#ifndef ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP
#define ECSS_SERVICES_STORAGEANDRETRIEVALSERVICE_HPP

#include "ECSS_Definitions.hpp"
#include "Service.hpp"
#include "ErrorHandler.hpp"
#include "Helpers/PacketStore.hpp"
#include "etl/map.h"

/**
 * Implementation of ST[15] Storage and Retrieval Service, as defined in ECSS-E-ST-70-41C.
 *
 * @brief This Service:
 * 1) provides the capability to select reports generated by other services and store them into packet stores.
 * 2) allows the ground system to manage the reports in the packet stores and request their downlink.
 *
 * @author Konstantinos Petridis <petridkon@gmail.com>
 */
class StorageAndRetrievalService : public Service {
	/**
	 * @todo: add prioritization policy for retrievals if prioritization is supported
	 */
public:
	/**
	 * Limits noting the minimum and maximum valid Virtual Channels used by the Storage and Retrieval subservice
	 */
	enum VirtualChannels : uint8_t { MIN = 1, MAX = 10 };

	/**
	 * The type of timestamps that the Storage and Retrieval Subservice assigns to each incoming packet.
	 */
	enum TimeStamping : uint8_t { StorageBased = 0, PacketBased = 1 };

	/**
	 * @brief Different types of packet retrieval from a packet store, relative to a specified time-tag.
	 */
	enum TimeWindowType : uint8_t { FromTagToTag = 0, AfterTimeTag = 1, BeforeTimeTag = 2 };

private:
	/**
	 * Helper function that reads the packet store ID string from a TM[15] message
	 */
	String<ECSSMaxPacketStoreIdSize> readPacketStoreId(Message& message);

	/**
	 * Helper function that, given a time-limit, deletes every packet stored in the specified packet-store, up to the
	 * requested time.
	 *
	 * @note Its functionality is indirectly tested in the test-case named 'Deleting packet store content'.
	 */
	void deleteContentUntil(const String<ECSSMaxPacketStoreIdSize>& packetStoreId, uint32_t timeLimit);

	/**
	 * Helper function that copies all the packets within the start-time -> end-time window to the destination packet
	 * store.
	 *
	 * @todo: may needs to be template, depending on the timestamping type.
	 */
	bool copyPacketsFrom(PacketStore& source, PacketStore& target, uint32_t startTime, uint32_t endTime,
	                     TimeWindowType timeWindow);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, that fall between the two specified
	 * time-tags as per 6.15.3.8.4.d(1) of the standard.
	 *
	 * @note Its functionality is indirectly tested in the test case named 'Copying packets in time window, from tag to
	 * tag'.
	 */
	void copyFromTagToTag(PacketStore& source, PacketStore& target, uint32_t startTime, uint32_t endTime);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is after the
	 * specified time-tag as per 6.15.3.8.4.d(2) of the standard.
	 *
	 * @note Its functionality is indirectly tested in the test case named 'Copying packets in time window, after
	 * time-tag'.
	 */
	void copyAfterTimeTag(PacketStore& source, PacketStore& target, uint32_t startTime);

	/**
	 * Copies all TM packets from source packet store to the target packet-store, whose time-stamp is before the
	 * specified time-tag as per 6.15.3.8.4.d(3) of the standard.
	 *
	 * @note Its functionality is indirectly tested in the test case named 'Copying packets in time window, before
	 * time-tag'.
	 */
	void copyBeforeTimeTag(PacketStore& source, PacketStore& target, uint32_t endTime);

	/**
	 * Forms the content summary of the specified packet-store and appends it to a report message.
	 *
	 * @note Its functionality is indirectly tested in the test case named 'Reporting the content summary of packet
	 * stores'.
	 */
	void createContentSummary(Message& report, const String<ECSSMaxPacketStoreIdSize>& packetStoreId);

public:
	inline static const uint8_t ServiceType = 15;

	enum MessageType : uint8_t {
		EnableStorageInPacketStores = 1,
		DisableStorageInPacketStores = 2,
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

	typedef String<ECSSMaxPacketStoreIdSize> packetStoreKey;

	/**
	 * @brief All packet stores, held by the Storage and Retrieval Service. Each packet store has its ID as key.
	 */
	etl::map<packetStoreKey, PacketStore, ECSSMaxPacketStores> packetStores;

	/**
	 * @brief Whether the Storage and Retrieval Subservice supports the cyclical update of packets (Circular type).
	 */
	const bool supportsOverwritingOldPackets = true;

	/**
	 * @brief Whether the Storage and Retrieval Subservice supports rejecting new packets while the packet store is
	 * full (Bounded type)
	 */
	const bool supportsRejectingNewPackets = true;

	/**
	 * @brief Support for the capability to handle multiple retrieval requests in parallel as per 6.15.3.1(i)
	 */
	const bool supportsConcurrentRetrievalRequests = false;

	/**
	 * @brief The max number of concurrent retrieval requests supported by the subservice.
	 */
	const uint16_t maxConcurrentRetrievalRequests = 5;

	/**
	 * @brief The type of the time window in which the retrieval shall occur.
	 */
	TimeWindowType timeWindowType = FromTagToTag;

	/**
	 * @brief Support for the capability to queue requests pending their execution as per 6.15.3.1(k)
	 */
	const bool supportsQueuingRetrievalRequests = true;

	/**
	 * @brief Support for the capability to prioritize packet retrieval as per 6.15.3.1(m)
	 */
	const bool supportsPrioritizingRetrievals = false;

	/**
	 * @brief Support for the by-time-range retrieval of packets.
	 */
	const bool supportsByTimeRangeRetrieval = true;

	/**
	 * @brief The type of timestamps that the subservice sets to each incoming telemetry packet.
	 */
	const TimeStamping timeStamping = PacketBased;

	/**
	 * TC[15,1] request to enable the packet stores' storage function
	 */
	void enableStorageFunction(Message& request);

	/**
	 * TC[15,2] request to disable the packet stores' storage function
	 */
	void disableStorageFunction(Message& request);

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
	 * TC[15,9] start the by-time-range retrieval of packet stores
	 */
	void startByTimeRangeRetrieval(Message& request);

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
	 * TC[15,11] delete the packet store content up to the specified time
	 */
	void deletePacketStoreContent(Message& request);

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
	void copyPacketsInTimeWindow(Message& request, TimeWindowType timeWindow);

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
	 * This function takes a TC[15,12] 'report the packet store content summary' as argument and responds with a TM[15,
	 * 13] 'packet store content summary report' report message.
	 */
	void packetStoreContentSummaryReport(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param param Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& request);
};

#endif
