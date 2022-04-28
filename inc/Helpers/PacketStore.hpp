#ifndef ECSS_SERVICES_PACKETSTORE_HPP
#define ECSS_SERVICES_PACKETSTORE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/deque.h"
#include "Message.hpp"

/**
 * This is the Packet Store class, needed for the Storage-Retrieval Service. The purpose of the packet-store is to
 * store all the TM packets transmitted by the other Services.
 */
class PacketStore {
public:
	/**
	 * The virtual channel used to transmit the packet store to the ground station. There is an upper and a lower
	 * bound for the virtual channels, defined in 'ECSSDefinitions' file.
	 */
	uint8_t virtualChannel;
	/**
	 * The time-tag that defines the starting point of the open retrieval process, meaning that we retrieve packets,
	 * starting from the open-retrieval-start-time-tag until the latest packet.
	 */
	uint32_t openRetrievalStartTimeTag = 0;
	/**
	 * The start time of a by-time-range retrieval process, i.e. retrieval of packets between two specified time-tags.
	 */
	uint32_t retrievalStartTime = 0;
	/**
	 * The end time of a by-time-range retrieval process, i.e. retrieval of packets between two specified time-tags.
	 */
	uint32_t retrievalEndTime = 0;
	/**
	 * The maximum size of the packet store, in bytes.
	 *
	 * @todo: add a way of defining each packets store's size in bytes
	 */
	uint64_t sizeInBytes;

	/**
	 * Whether the insertion of packets stores in the packet-store should cyclically overwrite older packets, or be
	 * suspended when the packet-store is full.
	 */
	enum PacketStoreType : uint8_t { Circular = 0, Bounded = 1 };

	/**
	 * Whether the open retrieval status of the packet-store is in progress or not.
	 */
	enum PacketStoreOpenRetrievalStatus : bool { Suspended = false, InProgress = true };

	/**
+	 * Whether the storage of TM packets is enabled for this packet store
	 */
	bool storageStatus = false;

	/**
	 * Whether the by-time-range retrieval of packet stores is enabled for this packet-store.
	 */
	bool byTimeRangeRetrievalStatus = false;
	PacketStoreType packetStoreType;
	PacketStoreOpenRetrievalStatus openRetrievalStatus;

	PacketStore() = default;

	/**
	 * A queue containing the TM messages stored by the packet store. Every TM is accompanied by its timestamp.
	 *
	 * @note A convention is made that this should be filled out using `push_back` and NOT `push_front`, dictating that
	 * earlier packets are placed in the front position. So removing the earlier packets is done with `pop_front`.
	 *
	 * 				old packets  <---------->  new packets
	 * 				[][][][][][][][][][][][][][][][][][][]	<--- deque
	 */
	etl::deque<std::pair<uint32_t, Message>, ECSSMaxPacketStoreSize> storedTelemetryPackets;

	/**
	 * Returns the sum of the sizes of the packets stored in this PacketStore, in bytes.
	 */
	uint16_t calculateSizeInBytes();
};

#endif
