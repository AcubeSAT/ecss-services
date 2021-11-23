#include <iostream>
#include "Services/StorageAndRetrievalService.hpp"

/******************************************************************************
 ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 	  Storage and Retrieval     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
 ******************************************************************************/

void StorageAndRetrievalService::enableStorageFunction(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnableStorageFunction, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			packetStore.second.storageStatus = true;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].storageStatus = true;
	}
}

void StorageAndRetrievalService::disableStorageFunction(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DisableStorageFunction, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			packetStore.second.storageStatus = false;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].storageStatus = false;
	}
}

void StorageAndRetrievalService::changeOpenRetrievalStartTimeTag(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ChangeOpenRetrievalStartTimeTag, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint32_t newStartTimeTag = request.readUint32();
	/**
	 * @todo: check if newStartTimeTag is in the future
	 */
	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			if (packetStore.second.openRetrievalStatus == PacketStore::InProgress) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
				continue;
			}
			packetStore.second.openRetrievalStartTimeTag = newStartTimeTag;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if (packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		packetStores[packetStoreId].openRetrievalStartTimeTag = newStartTimeTag;
	}
}

void StorageAndRetrievalService::resumeOpenRetrievalOfPacketStores(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ResumeOpenRetrievalOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			if ((not supportsConcurrentRetrievalRequests) and packetStore.second.byTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
				continue;
			}
			packetStore.second.openRetrievalStatus = PacketStore::InProgress;
			if (supportsPrioritizingRetrievals) {
				uint16_t newRetrievalPriority = request.readUint16();
				packetStore.second.retrievalPriority = newRetrievalPriority;
			}
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if ((not supportsConcurrentRetrievalRequests) and packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStores[packetStoreId].openRetrievalStatus = PacketStore::InProgress;
		if (supportsPrioritizingRetrievals) {
			uint16_t newRetrievalPriority = request.readUint16();
			packetStores[packetStoreId].retrievalPriority = newRetrievalPriority;
		}
	}
}

void StorageAndRetrievalService::suspendOpenRetrievalOfPacketStores(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::SuspendOpenRetrievalOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			packetStore.second.openRetrievalStatus = PacketStore::Suspended;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
	}
}

void StorageAndRetrievalService::startByTimeRangeRetrieval(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::StartByTimeRangeRetrieval, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if ((not supportsConcurrentRetrievalRequests) and
		    packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		if (packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::ByTimeRangeRetrievalAlreadyEnabled);
			continue;
		}
		if (supportsPrioritizingRetrievals) {
			uint16_t priority = request.readUint16();
			packetStores[packetStoreId].retrievalPriority = priority;
		}
		uint32_t retrievalStartTime = request.readUint32();
		uint32_t retrievalEndTime = request.readUint32();
		if (retrievalStartTime >= retrievalEndTime) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
			continue;
		}
		//		if (retrievalEndTime <= packetStores[packetStoreId].storedTmPackets.end()->first and retrievalEndTime <=
		//		                                                                                         timeNow) {
		//
		//		}
		/**
		 * @todo: 6.15.3.5.2.d(4), actually count the current time
		 */
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = true;
		packetStores[packetStoreId].retrievalStartTime = retrievalStartTime;
		packetStores[packetStoreId].retrievalEndTime = retrievalEndTime;
		/**
		 * @todo: start the by-time-range retrieval process according to the priority policy
		 */
	}
}

void StorageAndRetrievalService::abortByTimeRangeRetrieval(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AbortByTimeRangeRetrieval, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			packetStore.second.byTimeRangeRetrievalStatus = false;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
	}
}

void StorageAndRetrievalService::packetStoresStatusReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportStatusOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message report(ServiceType, MessageType::PacketStoresStatusReport, Message::TM, 1);
	report.appendUint16(packetStores.size());
	for (auto& packetStore : packetStores) {
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId = packetStore.first;
		report.appendOctetString(packetStoreId);
		report.appendBoolean(packetStore.second.storageStatus);
		uint16_t code = (packetStore.second.openRetrievalStatus == PacketStore::InProgress) ? 0 : 1;
		report.appendUint16(code);
		if (supportsByTimeRangeRetrieval) {
			report.appendBoolean(packetStore.second.byTimeRangeRetrievalStatus);
		}
	}
	storeMessage(report);
}

void StorageAndRetrievalService::deletePacketStoreContent(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeletePacketStoreContent, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint32_t timeLimit = request.readUint32(); // todo: decide the time-format
	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto& packetStore : packetStores) {
			for (auto& tmPacket : packetStore.second.storedTmPackets) {
				if (tmPacket.first > timeLimit) {
					break;
				}
				packetStore.second.storedTmPackets.pop_front();
			}
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if (packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		if (packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		for (auto& tmPacket : packetStores[packetStoreId].storedTmPackets) {
			/**
			 * @todo: actually compare the real time formats.
			 */
			if (tmPacket.first > timeLimit) {
				break;
			}
			packetStores[packetStoreId].storedTmPackets.pop_front();
		}
	}
}

void StorageAndRetrievalService::createPacketStores(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::CreatePacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		if (packetStores.size() >= ECSS_MAX_PACKET_STORES) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxNumberOfPacketStoresReached);
			return;
		}
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> idToCreate(packetStoreData);
		uint16_t packetStoreSize = request.readUint16();
		uint16_t typeCode = request.readUint16();
		PacketStore::PacketStoreType packetStoreType = (!typeCode) ? PacketStore::Circular : PacketStore::Bounded;
		uint8_t virtualChannel = request.readUint8();

		if (virtualChannel < MIN or virtualChannel > MAX) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel);
			continue;
		}
		/**
		 * @todo: actually check if the available memory can handle the new creation
		 */
		if (packetStores.find(idToCreate) != packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingPacketStore);
			continue;
		}
		PacketStore newPacketStore;
		newPacketStore.sizeInBytes = packetStoreSize;
		newPacketStore.packetStoreType = packetStoreType;
		newPacketStore.storageStatus = false;
		newPacketStore.byTimeRangeRetrievalStatus = false;
		newPacketStore.openRetrievalStatus = PacketStore::Suspended;
		newPacketStore.virtualChannel = virtualChannel;
		packetStores.insert({idToCreate, newPacketStore});
	}
}

void StorageAndRetrievalService::deletePacketStores(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeletePacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		int size = 0;
		etl::string <ECSS_MAX_PACKET_STORE_ID_SIZE> keysToDelete[packetStores.size()];
		for (auto& packetStore : packetStores) {
			if (packetStore.second.storageStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
				continue;
			}
			if (packetStore.second.byTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithByTimeRangeRetrieval);
				continue;
			}
			if (packetStore.second.openRetrievalStatus == PacketStore::InProgress) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithOpenRetrievalInProgress);
				continue;
			}
			keysToDelete[size++] = packetStore.first;
		}
		for (int l = 0; l < size; l++) {
			uint8_t data[ECSS_MAX_PACKET_STORE_ID_SIZE];
			etl::string<ECSS_MAX_PACKET_STORE_ID_SIZE> keyToDelete = keysToDelete[l];
			std::copy(keyToDelete.begin(), keyToDelete.end(), data);
			String<ECSS_MAX_PACKET_STORE_ID_SIZE> key(data);
			packetStores.erase(key);
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> idToDelete(packetStoreData);
		if (packetStores.find(idToDelete) == packetStores.end()) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		if (packetStores[idToDelete].storageStatus) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStores[idToDelete].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithByTimeRangeRetrieval);
			continue;
		}
		if (packetStores[idToDelete].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithOpenRetrievalInProgress);
			continue;
		}
		packetStores.erase(idToDelete);
	}
}

void StorageAndRetrievalService::packetStoreConfigurationReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportConfigurationOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message report(ServiceType, MessageType::PacketStoreConfigurationReport, Message::TM, 1);
	report.appendUint16(packetStores.size());
	for (auto& packetStore : packetStores) {
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId = packetStore.first;
		report.appendOctetString(packetStoreId);
		report.appendUint16(packetStore.second.sizeInBytes);
		uint16_t typeCode = (packetStore.second.packetStoreType == PacketStore::Circular) ? 0 : 1;
		report.appendUint16(typeCode);
		report.appendUint16(packetStore.second.virtualChannel);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::copyPacketsInTimeWindow(Message& request, bool beforeTimeTag, bool afterTimeTag,
                                                         bool fromTagToTag) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::CopyPacketsInTimeWindow, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t timeTagsTypeCode = request.readUint16();
	//	TimeStamping timeTagsType = (!timeTagsTypeCode) ? StorageBased : PacketBased; //todo: actually figure out
	uint32_t timeTag1 = request.readUint32();
	uint32_t timeTag2 = request.readUint32();
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> fromPacketStoreId(packetStoreData);
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> toPacketStoreId(packetStoreData);
	if (packetStores.find(fromPacketStoreId) == packetStores.end() or
	    packetStores.find(toPacketStoreId) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (timeTag1 >= timeTag2) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
		return;
	}
	if (not packetStores[toPacketStoreId].storedTmPackets.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DestinationPacketStoreNotEmtpy);
		return;
	}
	if (not packetStores[fromPacketStoreId].copyPacketsTo(packetStores[toPacketStoreId], timeTag1, timeTag2,
	                                                      beforeTimeTag, afterTimeTag, fromTagToTag)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
	}
}

void StorageAndRetrievalService::resizePacketStores(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ResizePacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		uint16_t packetStoreSize = request.readUint16(); // In bytes
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		/**
		 * @todo: check if the current memory availability can handle the new size requested
		 */
		if (packetStores[packetStoreId].storageStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		if (packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStores[packetStoreId].sizeInBytes = packetStoreSize;
	}
}

void StorageAndRetrievalService::changeTypeToCircular(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ChangeTypeToCircular, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> idToChange(packetStoreData);
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (packetStores[idToChange].storageStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStores[idToChange].byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStores[idToChange].packetStoreType = PacketStore::Circular;
}

void StorageAndRetrievalService::changeTypeToBounded(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ChangeTypeToBounded, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String <ECSS_MAX_PACKET_STORE_ID_SIZE> idToChange(packetStoreData);
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (packetStores[idToChange].storageStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStores[idToChange].byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStores[idToChange].packetStoreType = PacketStore::Bounded;
}

void StorageAndRetrievalService::changeVirtualChannel(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ChangeVirtualChannel, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String <ECSS_MAX_PACKET_STORE_ID_SIZE> idToChange(packetStoreData);
	uint8_t virtualChannel = request.readUint8();
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (virtualChannel < MIN or virtualChannel > MAX) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel);
		return;
	}
	if (packetStores[idToChange].byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStores[idToChange].virtualChannel = virtualChannel;
}

void StorageAndRetrievalService::packetStoreContentSummaryReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportContentSummaryOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message report(ServiceType, MessageType::PacketStoreContentSummaryReport, Message::TM, 1);
	uint16_t numOfPacketStores = request.readUint16();
	// For all packet stores
	if (!numOfPacketStores) {
		report.appendUint16(packetStores.size());
		for (auto &packetStore : packetStores) {
			report.appendOctetString(packetStore.first);
			uint32_t oldestStoredPacketTime = packetStore.second.storedTmPackets.front().first;
			report.appendUint32(oldestStoredPacketTime);
			uint32_t newestStoredPacketTime = packetStore.second.storedTmPackets.back().first;
			report.appendUint32(newestStoredPacketTime);
			report.appendUint32(packetStore.second.openRetrievalStartTimeTag);
			auto fillingPercentage = static_cast <uint16_t> (packetStore.second.storedTmPackets.size() * 100 /
			                                               ECSS_MAX_PACKETS_IN_PACKET_STORE);
			report.appendUint16(fillingPercentage);
			uint16_t packetCounter = 0;
			for (auto &packet : packetStore.second.storedTmPackets) {
				if (packet.first >= packetStore.second.openRetrievalStartTimeTag) {
					packetCounter++;
				}
			}
			fillingPercentage = static_cast <uint16_t> (packetCounter * 100 / ECSS_MAX_PACKETS_IN_PACKET_STORE);
			report.appendUint16(fillingPercentage);
		}
		storeMessage(report);
		return;
	}
	// For specified packet stores
	uint16_t numOfValidPacketStores = 0;
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) != packetStores.end()) {
			numOfValidPacketStores++;
		}
	}
	report.appendUint16(numOfValidPacketStores);
	request.resetRead();
	numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		request.readOctetString(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		report.appendOctetString(packetStoreId);
		uint32_t oldestStoredPacketTime = packetStores[packetStoreId].storedTmPackets.front().first;
		report.appendUint32(oldestStoredPacketTime);
		uint32_t newestStoredPacketTime = packetStores[packetStoreId].storedTmPackets.back().first;
		report.appendUint32(newestStoredPacketTime);
		report.appendUint32(packetStores[packetStoreId].openRetrievalStartTimeTag);
		auto fillingPercentage = static_cast <uint16_t> (packetStores[packetStoreId].storedTmPackets.size() * 100 /
		                                               ECSS_MAX_PACKETS_IN_PACKET_STORE);
		report.appendUint16(fillingPercentage);
		uint16_t packetCounter = 0;
		for (auto &packet : packetStores[packetStoreId].storedTmPackets) {
			if (packet.first >= packetStores[packetStoreId].openRetrievalStartTimeTag) {
				packetCounter++;
			}
		}
		fillingPercentage = static_cast <uint16_t> (packetCounter * 100 / ECSS_MAX_PACKETS_IN_PACKET_STORE);
		report.appendUint16(fillingPercentage);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::execute(Message& request) {
	switch (request.messageType) {
		case 1:
			enableStorageFunction(request);
			break;
		case 2:
			disableStorageFunction(request);
			break;
		case 9:
			startByTimeRangeRetrieval(request);
			break;
		case 11:
			deletePacketStoreContent(request);
			break;
		case 12:
			packetStoreContentSummaryReport(request);
			break;
		case 14:
			changeOpenRetrievalStartTimeTag(request);
			break;
		case 15:
			resumeOpenRetrievalOfPacketStores(request);
			break;
		case 16:
			suspendOpenRetrievalOfPacketStores(request);
			break;
		case 17:
			abortByTimeRangeRetrieval(request);
			break;
		case 18:
			packetStoresStatusReport(request);
			break;
		case 20:
			createPacketStores(request);
			break;
		case 21:
			deletePacketStores(request);
			break;
		case 22:
			packetStoreConfigurationReport(request);
			break;
		case 24:
			copyPacketsInTimeWindow(request, beforeTimeTag, afterTimeTag, fromTagToTag);
			break;
		case 25:
			resizePacketStores(request);
			break;
		case 26:
			changeTypeToCircular(request);
			break;
		case 27:
			changeTypeToBounded(request);
			break;
		case 28:
			changeVirtualChannel(request);
			break;
	}
}

StorageAndRetrievalService::StorageAndRetrievalService()
    : packetSelectionSubservice(*this, 5, ECSS_MAX_EVENT_DEFINITION_IDS,
                                ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS,
                                ECSS_MAX_SERVICE_TYPE_DEFINITIONS) {
	serviceType = StorageAndRetrievalService::ServiceType;
}

/******************************************************************************
 ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~	 Packet Selection     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
 ******************************************************************************/

StorageAndRetrievalService::PacketSelectionSubservice::PacketSelectionSubservice(
    StorageAndRetrievalService& parent, uint16_t numOfControlledAppProcs, uint16_t maxEventDefIds,
    uint16_t maxHousekeepingStructIds, uint16_t maxReportTypeDefs, uint16_t maxServiceTypeDefs)
    : mainService(parent), numOfControlledAppProcesses(numOfControlledAppProcs), maxEventDefinitionIds(maxEventDefIds),
      maxHousekeepingStructureIds(maxHousekeepingStructIds), maxReportTypeDefinitions(maxReportTypeDefs),
      maxServiceTypeDefinitions(maxServiceTypeDefs), supportsSubsamplingRate(true) {}

bool StorageAndRetrievalService::PacketSelectionSubservice::appIsControlled(uint16_t applicationId, Message& request) {
	if (std::find(controlledAppProcesses.begin(), controlledAppProcesses.end(), applicationId) ==
	    controlledAppProcesses.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnControlledApplicationProcessId);
		return false;
	}
	return true;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxReportDefinitions(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	        .serviceTypeDefinitions[serviceId]
	        .size() >= maxReportTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxServiceDefinitions(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.size() >=
	    maxServiceTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxServiceTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noReportDefinitionInService(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	        .serviceTypeDefinitions[serviceId]
	        .empty()) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinitionInService);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noServiceDefinitionInApplication(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinitionInService);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appExistsInDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	if (applicationProcessConfiguration.definitions[packetStoreId].find(applicationId) !=
	    applicationProcessConfiguration.definitions[packetStoreId].end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createAppDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	typedef etl::vector<uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> vecType;
	etl::map<uint16_t, vecType, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> tempMap;
	ApplicationProcessDefinition newAppDefinition;
	newAppDefinition.serviceTypeDefinitions = tempMap;
	applicationProcessConfiguration.definitions[packetStoreId].insert({applicationId, newAppDefinition});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::serviceExistsInApp(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.find(
	        serviceId) !=
	    applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createServiceDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId) {
	etl::vector<uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> tempVec;
	applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.insert(
	    {serviceId, tempVec});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::reportExistsInService(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId, uint16_t reportId,
    uint16_t& index) {
	uint16_t position = 0;
	for (auto& id :
	     applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions[serviceId]) {
		if (id == reportId) {
			index = position;
			return true;
		}
		position++;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createReportDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId,
    uint16_t reportId) {
	applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	    .serviceTypeDefinitions[serviceId]
	    .push_back(reportId);
}

bool StorageAndRetrievalService::PacketSelectionSubservice::serviceHasReportDefinitions(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
	        .serviceTypeDefinitions[serviceId]
	        .empty()) {
		return false;
	}
	return true;
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteReportDefinitionsOfService(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t serviceId, bool deleteAll,
    uint16_t index) {
	if (deleteAll) {
		applicationProcessConfiguration.definitions[packetStoreId][applicationId]
		    .serviceTypeDefinitions[serviceId]
		    .clear();
	} else {
		auto iterator = applicationProcessConfiguration.definitions[packetStoreId][applicationId]
		                    .serviceTypeDefinitions[serviceId]
		                    .begin() +
		                index;
		applicationProcessConfiguration.definitions[packetStoreId][applicationId]
		    .serviceTypeDefinitions[serviceId]
		    .erase(iterator);
		if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
		        .serviceTypeDefinitions[serviceId]
		        .empty()) {
			deleteServiceDefinitionsOfApp(packetStoreId, applicationId, false, serviceId);
			if (applicationProcessConfiguration.definitions[packetStoreId][applicationId]
			        .serviceTypeDefinitions.empty()) {
				applicationProcessConfiguration.definitions[packetStoreId].erase(applicationId);
			}
		}
	}
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appHasServiceDefinitions(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
		return false;
	}
	return true;
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteServiceDefinitionsOfApp(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, bool deleteAll, uint16_t serviceId) {
	if (deleteAll) {
		applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.clear();
	} else {
		applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.erase(
		    serviceId);
		if (applicationProcessConfiguration.definitions[packetStoreId][applicationId].serviceTypeDefinitions.empty()) {
			applicationProcessConfiguration.definitions[packetStoreId].erase(applicationId);
		}
	}
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxStructureIds(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.size() >=
	    maxHousekeepingStructureIds) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxHousekeepingStructureIdsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noStructureInDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.empty()) {
		ErrorHandler::reportError(
		    request, ErrorHandler::ExecutionStartErrorType::NonExistingHousekeepingStructureIdInDefinition);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::housekeepingDefinitionExists(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	if (housekeepingReportConfiguration.definitions[packetStoreId].find(applicationId) !=
	    housekeepingReportConfiguration.definitions[packetStoreId].end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createHousekeepingDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	etl::vector<std::pair<uint16_t, uint16_t>, ECSS_MAX_HOUSEKEEPING_STRUCTS_PER_STORAGE_CONTROL> housekeepingStructIds;
	HousekeepingDefinition newDefinition;
	newDefinition.housekeepingStructIds = housekeepingStructIds;
	housekeepingReportConfiguration.definitions[packetStoreId].insert({applicationId, newDefinition});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::structureExists(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t structureId,
    uint16_t& index) {
	uint16_t position = 0;
	for (auto& id : housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds) {
		if (id.first == structureId) {
			index = position;
			return true;
		}
		position++;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteStructureIds(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, bool deleteAll, uint16_t index) {
	if (deleteAll and
	    (not housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.empty())) {
		housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.clear();
	} else {
		auto iterator =
		    housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.begin() +
		    index;
		housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.erase(iterator);
		if (housekeepingReportConfiguration.definitions[packetStoreId][applicationId].housekeepingStructIds.empty()) {
			housekeepingReportConfiguration.definitions[packetStoreId].erase(applicationId);
		}
	}
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxEventDefinitionIds(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.size() >=
	    maxEventDefinitionIds) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxEventDefinitionIdsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noEventInDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, Message& request) {
	if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NoEventDefinitionExistsInApp);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::eventBlockingDefinitionExists(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	if (eventReportConfiguration.definitions[packetStoreId].find(applicationId) !=
	    eventReportConfiguration.definitions[packetStoreId].end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createEventReportBlockingDefinition(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId) {
	etl::vector<uint16_t, ECSS_MAX_EVENT_DEFINITION_IDS> eventDefinitionIds;
	EventDefinition newDefinition;
	newDefinition.eventDefinitionIds = eventDefinitionIds;
	eventReportConfiguration.definitions[packetStoreId].insert({applicationId, newDefinition});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::eventDefinitionIdExists(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t eventId, uint16_t& index) {
	uint16_t position = 0;
	for (auto& id : eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds) {
		if (id == eventId) {
			index = position;
			return true;
		}
		position++;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createEventDefinitionId(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, uint16_t eventId) {
	eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.push_back(eventId);
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteEventDefinitionIds(
    String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId, uint16_t applicationId, bool deleteAll, uint16_t index) {
	if (deleteAll and
	    (not eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty())) {
		eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.clear();
	} else {
		auto iterator =
		    eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.begin() + index;
		eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.erase(iterator);
		if (eventReportConfiguration.definitions[packetStoreId][applicationId].eventDefinitionIds.empty()) {
			eventReportConfiguration.definitions[packetStoreId].erase(applicationId);
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::addReportTypesToAppProcessConfiguration(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddReportTypesToAppProcessConfiguration, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String <ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	// Per application process
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t currentAppId = request.readUint16();
		if (not appIsControlled(currentAppId, request)) {
			continue;
		}
		if (noServiceDefinitionInApplication(packetStoreId, currentAppId, request)) {
			continue;
		}
		uint16_t numOfCurrAppServices = request.readUint16();
		// Add all reports in application
		if (!numOfCurrAppServices) {
			if (not appExistsInDefinition(packetStoreId, currentAppId)) {
				createAppDefinition(packetStoreId, currentAppId);
			}
			if (appHasServiceDefinitions(packetStoreId, currentAppId)) {
				deleteServiceDefinitionsOfApp(packetStoreId, currentAppId, true, 0);
			}
			continue;
		}
		if (exceedsMaxServiceDefinitions(packetStoreId, currentAppId, request)) {
			continue;
		}
		// Per service type in application process
		for (int j = 0; j < numOfCurrAppServices; j++) {
			uint16_t currentServiceId = request.readUint16();
			if (noReportDefinitionInService(packetStoreId, currentAppId, currentServiceId, request)) {
				continue;
			}
			uint16_t numOfCurrServiceMessageTypes = request.readUint16();
			// Add all reports of Service
			if ((!numOfCurrServiceMessageTypes)) {
				if (not appExistsInDefinition(packetStoreId, currentAppId)) {
					createAppDefinition(packetStoreId, currentAppId);
				}
				if (not serviceExistsInApp(packetStoreId, currentAppId, currentServiceId)) {
					createServiceDefinition(packetStoreId, currentAppId, currentServiceId);
				}
				if (serviceHasReportDefinitions(packetStoreId, currentAppId, currentServiceId)) {
					deleteReportDefinitionsOfService(packetStoreId, currentAppId, currentServiceId, true, 0);
				}
				continue;
			}
			if (exceedsMaxReportDefinitions(packetStoreId, currentAppId, currentServiceId, request)) {
				continue;
			}
			// Per report type
			for (int k = 0; k < numOfCurrServiceMessageTypes; k++) {
				uint16_t currentReportType = request.readUint16();
				if (not appExistsInDefinition(packetStoreId, currentAppId)) {
					createAppDefinition(packetStoreId, currentAppId);
				}
				if (not serviceExistsInApp(packetStoreId, currentAppId, currentServiceId)) {
					createServiceDefinition(packetStoreId, currentAppId, currentServiceId);
				}
				uint16_t garbage = 0;
				if (not reportExistsInService(packetStoreId, currentAppId, currentServiceId, currentReportType,
				                              garbage)) {
					createReportDefinition(packetStoreId, currentAppId, currentServiceId, currentReportType);
				}
			}
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteReportTypesFromAppProcessConfiguration(
    Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteReportTypesFromAppProcessConfiguration,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	if (!numOfApplicationIds) {
		applicationProcessConfiguration.definitions[packetStoreId].clear();
		return;
	}
	// Per application process
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t currentAppId = request.readUint16();
		if (not appExistsInDefinition(packetStoreId, currentAppId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::NonExistingApplicationInDefinition);
			continue;
		}
		uint16_t numOfCurrAppServices = request.readUint16();
		// Add all reports in application
		if (!numOfCurrAppServices) {
			applicationProcessConfiguration.definitions[packetStoreId].erase(currentAppId);
			continue;
		}
		// Per service type in application process
		for (int j = 0; j < numOfCurrAppServices; j++) {
			uint16_t currentServiceId = request.readUint16();
			if (not serviceExistsInApp(packetStoreId, currentAppId, currentServiceId)) {
				ErrorHandler::reportError(request,
				                          ErrorHandler::ExecutionStartErrorType::NonExistingServiceTypeDefinitionInApp);
				continue;
			}
			uint16_t numOfCurrServiceReportTypes = request.readUint16();
			// Delete whole service definition
			if ((!numOfCurrServiceReportTypes)) {
				deleteServiceDefinitionsOfApp(packetStoreId, currentAppId, false, currentServiceId);
				continue;
			}
			// Per report type
			for (int k = 0; k < numOfCurrServiceReportTypes; k++) {
				uint16_t currentReportType = request.readUint16();
				uint16_t index = 0;
				if (not reportExistsInService(packetStoreId, currentAppId, currentServiceId, currentReportType,
				                              index)) {
					ErrorHandler::reportError(
					    request, ErrorHandler::ExecutionStartErrorType::NonExistingReportTypeDefinitionInService);
					continue;
				}
				deleteReportDefinitionsOfService(packetStoreId, currentAppId, currentServiceId, false, index);
			}
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::appConfigurationContentReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportAppConfigurationContent, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message contentReport(ServiceType, MessageType::AppConfigurationContentReport, Message::TM, 1);
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	contentReport.appendString(packetStoreId);
	uint16_t numOfApplications = applicationProcessConfiguration.definitions[packetStoreId].size();
	contentReport.appendUint16(numOfApplications);
	for (auto& app : applicationProcessConfiguration.definitions[packetStoreId]) {
		contentReport.appendUint16(app.first);
		uint16_t numOfAppServices = app.second.serviceTypeDefinitions.size();
		contentReport.appendUint16(numOfAppServices);
		for (auto& service : app.second.serviceTypeDefinitions) {
			contentReport.appendUint16(service.first);
			uint16_t numOfServiceReports = service.second.size();
			contentReport.appendUint16(numOfServiceReports);
			for (auto& report : service.second) {
				contentReport.appendUint16(report);
			}
		}
	}
	mainService.storeMessage(contentReport);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addStructuresToHousekeepingConfiguration(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddStructuresToHousekeepingConfiguration, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t currentAppId = request.readUint16();
		if (not appIsControlled(currentAppId, request)) {
			continue;
		}
		if (noStructureInDefinition(packetStoreId, currentAppId, request)) {
			continue;
		}
		uint16_t numOfHousekeepingStructs = request.readUint16();

		if (!numOfHousekeepingStructs) {
			if (not housekeepingDefinitionExists(packetStoreId, currentAppId)) {
				createHousekeepingDefinition(packetStoreId, currentAppId);
			}
			deleteStructureIds(packetStoreId, currentAppId, false, 0);
			continue;
		}

		for (int j = 0; j < numOfHousekeepingStructs; j++) {
			uint16_t currentStructId = request.readUint16();
			uint16_t subsamplingRate = 0;
			if (supportsSubsamplingRate) {
				subsamplingRate = request.readUint16();
			}
			if (exceedsMaxStructureIds(packetStoreId, currentAppId, request)) {
				continue;
			}
			/**
			 * @todo: check if this needs to be in the outer loop
			 */
			if (not housekeepingDefinitionExists(packetStoreId, currentAppId)) {
				createHousekeepingDefinition(packetStoreId, currentAppId);
			}
			uint16_t garbage = 0;
			if (not structureExists(packetStoreId, currentAppId, currentStructId, garbage)) {
				housekeepingReportConfiguration.definitions[packetStoreId][currentAppId]
				    .housekeepingStructIds.push_back(std::make_pair(currentStructId, subsamplingRate));
			}
			/**
			 * @todo: set the subsampling rate (pg.303)
			 */
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteStructuresFromHousekeepingConfiguration(
    Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteStructuresFromHousekeepingConfiguration,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	if (!numOfApplicationIds) {
		housekeepingReportConfiguration.definitions[packetStoreId].clear();
		return;
	}
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t currentAppId = request.readUint16();
		if (not housekeepingDefinitionExists(packetStoreId, currentAppId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::NonExistingApplicationInDefinition);
			continue;
		}
		uint16_t numOfHousekeepingStructs = request.readUint16();
		if (!numOfHousekeepingStructs) {
			housekeepingReportConfiguration.definitions[packetStoreId].erase(currentAppId);
			continue;
		}
		for (int j = 0; j < numOfHousekeepingStructs; j++) {
			uint16_t currentStructId = request.readUint16();
			uint16_t index = 0;
			if (not structureExists(packetStoreId, currentAppId, currentStructId, index)) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::NonExistingHousekeepingStructureIdInDefinition);
				continue;
			}
			deleteStructureIds(packetStoreId, currentAppId, false, index);
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::housekeepingConfigurationContentReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportHousekeepingConfigurationContent, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message contentReport(ServiceType, MessageType::HousekeepingConfigurationContentReport, Message::TM, 1);
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	contentReport.appendString(packetStoreId);
	contentReport.appendUint16(housekeepingReportConfiguration.definitions[packetStoreId].size());
	for (auto& app : housekeepingReportConfiguration.definitions[packetStoreId]) {
		contentReport.appendUint16(app.first);
		contentReport.appendUint16(app.second.housekeepingStructIds.size());
		for (auto& structId : app.second.housekeepingStructIds) {
			contentReport.appendUint16(structId.first);
			if (supportsSubsamplingRate) {
				contentReport.appendUint16(structId.second);
			}
		}
	}
	mainService.storeMessage(contentReport);
}

void StorageAndRetrievalService::PacketSelectionSubservice::addEventDefinitionsToEventReportConfiguration(
    Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddEventDefinitionsToEventReportConfiguration,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t applicationId = request.readUint16();
		if (not appIsControlled(applicationId, request)) {
			continue;
		}
		if (exceedsMaxEventDefinitionIds(packetStoreId, applicationId, request)) {
			continue;
		}
		if (noEventInDefinition(packetStoreId, applicationId, request)) {
			continue;
		}
		if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
			createEventReportBlockingDefinition(packetStoreId, applicationId);
		}
		uint16_t numOfEventIds = request.readUint16();
		if (!numOfEventIds) {
			if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
				createEventReportBlockingDefinition(packetStoreId, applicationId);
			}
			deleteEventDefinitionIds(packetStoreId, applicationId, true, 0);
			return;
		}
		for (int j = 0; j < numOfEventIds; j++) {
			uint16_t eventId = request.readUint16();
			uint16_t garbage = 0;
			if (not eventDefinitionIdExists(packetStoreId, applicationId, eventId, garbage)) {
				createEventDefinitionId(packetStoreId, applicationId, eventId);
			}
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteEventDefinitionsFromEventReportConfiguration(
    Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::DeleteEventDefinitionsFromEventReportConfiguration,
	                            request, ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	if (!numOfApplicationIds) {
		eventReportConfiguration.definitions[packetStoreId].clear();
		return;
	}
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t applicationId = request.readUint16();
		if (not eventBlockingDefinitionExists(packetStoreId, applicationId)) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::NonExistingEventReportBlockingDefinition);
			continue;
		}
		uint16_t numOfEventIds = request.readUint16();
		if (!numOfEventIds) {
			eventReportConfiguration.definitions[packetStoreId].erase(applicationId);
			return;
		}
		for (int j = 0; j < numOfEventIds; j++) {
			uint16_t eventId = request.readUint16();
			uint16_t index = 0;
			if (not eventDefinitionIdExists(packetStoreId, applicationId, eventId, index)) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingEventDefinitionId);
				continue;
			}
			deleteEventDefinitionIds(packetStoreId, applicationId, false, index);
		}
	}
}

void StorageAndRetrievalService::PacketSelectionSubservice::eventConfigurationContentReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportEventConfigurationContent, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
	request.readOctetString(packetStoreData);
	String<ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	Message contentReport(ServiceType, MessageType::EventConfigurationContentReport, Message::TM, 1);
	contentReport.appendString(packetStoreId);
	uint16_t numOfApplications = eventReportConfiguration.definitions[packetStoreId].size();
	contentReport.appendUint16(numOfApplications);
	for (auto& app : eventReportConfiguration.definitions[packetStoreId]) {
		contentReport.appendUint16(app.first);
		contentReport.appendUint16(app.second.eventDefinitionIds.size());
		for (auto& id : app.second.eventDefinitionIds) {
			contentReport.appendUint16(id);
		}
	}
	mainService.storeMessage(contentReport);
}
