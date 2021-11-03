#ifndef ECSS_SERVICES_PACKETSTORE_HPP
#define ECSS_SERVICES_PACKETSTORE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/deque.h"

class PacketStore {
public:
	uint8_t id;
	uint8_t virtualChannel;
	uint16_t retrievalPriority;
	uint32_t openRetrievalStartTimeTag; //What type is it?? (absolute time)
	uint32_t retrievalStartTime;
	uint32_t retrievalEndTime;

	enum PacketStoreType {
		Circular = 0,
		Bounded = 1
	};

	enum PacketStoreOpenRetrievalStatus {
		InProgress = 0,
		Suspended = 1
	};

	bool selfStorageStatus;
	bool selfByTimeRangeRetrievalStatus;
	PacketStoreOpenRetrievalStatus selfOpenRetrievalStatus;
	PacketStoreType packetStoreType;

	/**
	 * A queue containing the TM messages stored by the packet store. Every TM is followed by its timestamp.
	 *
	 * @note I made a convention that this should be filled out using 'push_back' and NOT 'push_front', dictating that
	 * earlier packets are placed in the front position. So popping the earlier packets is done with 'pop_front'.
	 */
	etl::deque <std::pair <uint32_t, Message>, ECSS_MAX_PACKETS_IN_PACKET_STORE> storedTmPackets;

};

#endif