#ifndef ECSS_SERVICES_PACKETSTORE_HPP
#define ECSS_SERVICES_PACKETSTORE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/deque.h"
#include "Message.hpp"

/**
 * @todo: add a way of measuring each packets store's size in bytes
 */

/**
 * This is the Packet Store class, needed for the Storage-Retrieval Service. The purpose of the packet-store is to
 * store all the TM packets transmitted by the other Services.
 */
class PacketStore {
public:
	uint8_t virtualChannel;
	uint16_t retrievalPriority;
	uint32_t openRetrievalStartTimeTag = 0; // What type is it?? (absolute time)
	uint32_t retrievalStartTime = 0;
	uint32_t retrievalEndTime = 0;
	uint64_t sizeInBytes;

	/**
	 * Whether the insertion of packets stores in the packet-store should cyclically overwrite older packets, or be
	 * suspended when the packet-store is full.
	 */
	enum PacketStoreType : uint8_t { Circular = 0, Bounded = 1 };

	/**
	 * Whether the open retrieval status of the packet-store is in progress or not.
	 */
	enum PacketStoreOpenRetrievalStatus : uint8_t { InProgress = 0, Suspended = 1 };

	/**
	 * @brief defines whether the storage of TM packets is enabled for this packet store
	 */
	bool storageStatus;

	/**
	 * Whether the by-time-range retrieval of packet stores is enabled for this packet-store.
	 */
	bool byTimeRangeRetrievalStatus;
	PacketStoreType packetStoreType;
	PacketStoreOpenRetrievalStatus openRetrievalStatus;

	PacketStore() = default;

	/**
	 * A queue containing the TM messages stored by the packet store. Every TM is followed by its timestamp.
	 *
	 * @note I made a convention that this should be filled out using 'push_back' and NOT 'push_front', dictating that
	 * earlier packets are placed in the front position. So popping the earlier packets is done with 'pop_front'.
	 *
	 * 				old packets  <---------->  new packets
	 * 				[][][][][][][][][][][][][][][][][][][]	<--- deque
	 */
	etl::deque<std::pair<uint32_t, Message>, ECSSMaxPacketStoreSize> storedTelemetryPackets;
};

#endif