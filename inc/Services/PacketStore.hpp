#ifndef ECSS_SERVICES_PACKETSTORE_HPP
#define ECSS_SERVICES_PACKETSTORE_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/String.hpp"

class PacketStore {
public:
	uint8_t id;
	uint8_t virtualChannel;
	uint16_t retrievalPriority;
	uint32_t openRetrievalStartTimeTag; //What type is it?? (absolute time)
	uint32_t retrievalStartTime;
	uint32_t retrievalEndTime;
	etl::string<8> managementType;      //circular or bounded

	enum PacketStoreOpenRetrievalStatus {
		InProgress = 0,
		Suspended = 1
	};
	bool selfStorageStatus;
	bool selfByTimeRangeRetrievalStatus;
	PacketStoreOpenRetrievalStatus selfOpenRetrievalStatus;

};

#endif