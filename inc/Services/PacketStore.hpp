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
	uint32_t openRetrievalStartTimeTag; //What type is it?? (absolute time)
	uint32_t retrievalStartTime;
	uint32_t retrievalEndTime;
	uint64_t sizeInBytes;

	enum PacketStoreType : uint8_t {
		Circular = 0,
		Bounded = 1
	};

	enum PacketStoreOpenRetrievalStatus : uint8_t {
		InProgress = 0,
		Suspended = 1
	};

	/**
	 * @brief defines whether the storage of TM packets is enabled for this packet store
	 */
	bool storageStatus;
	PacketStoreType packetStoreType;
	PacketStoreOpenRetrievalStatus openRetrievalStatus;
	bool byTimeRangeRetrievalStatus;

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
	etl::deque <std::pair <uint32_t, Message>, ECSS_MAX_PACKETS_IN_PACKET_STORE> storedTmPackets;

	/**
	 * This function copies all the packets within the start-time -> end-time window to the destination packet store.
	 *
	 * @note may needs to be template, depending on the timestamping type.
	 */
	bool copyPacketsTo(PacketStore &target,
	                   uint32_t startTime,
	                   uint32_t endTime,
	                   bool beforeTimeTag,
	                   bool afterTimeTag,
	                   bool fromTagToTag);
};

#endif