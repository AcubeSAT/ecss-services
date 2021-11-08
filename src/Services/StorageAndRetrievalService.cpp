#include "Services/StorageAndRetrievalService.hpp"

/********************************    Storage And Retrieval Subservice    *********************************/

void StorageAndRetrievalService::enableStorageFunction(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::EnableStorageFunction, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto &packetStore : packetStores) {
			packetStore.second.selfStorageStatus = true;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[currPacketStoreId].selfStorageStatus = true;
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
		for (auto &packetStore : packetStores) {
			packetStore.second.selfStorageStatus = false;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[currPacketStoreId].selfStorageStatus = false;
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
	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto &packetStore : packetStores) {
			if (packetStore.second.selfOpenRetrievalStatus == PacketStore::Suspended) {
				packetStore.second.openRetrievalStartTimeTag = newStartTimeTag;
			} else {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
			}
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if (packetStores[currPacketStoreId].selfOpenRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		packetStores[currPacketStoreId].openRetrievalStartTimeTag = newStartTimeTag;
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
		for (auto &packetStore : packetStores) {
			if ((not supportsConcurrentRetrievalRequests) and packetStore.second.selfByTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
				continue;
			}
			packetStore.second.selfOpenRetrievalStatus = PacketStore::InProgress;
			if (supportsPrioritizingRetrievals) {
				uint16_t newRetrievalPriority = request.readUint16();
				packetStore.second.retrievalPriority = newRetrievalPriority;
			}
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if ((not supportsConcurrentRetrievalRequests) and packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStores[currPacketStoreId].selfOpenRetrievalStatus = PacketStore::InProgress;
		if (supportsPrioritizingRetrievals) {
			uint16_t newRetrievalPriority = request.readUint16();
			packetStores[currPacketStoreId].retrievalPriority = newRetrievalPriority;
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
		for (auto &packetStore : packetStores) {
			packetStore.second.selfOpenRetrievalStatus = PacketStore::Suspended;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[currPacketStoreId].selfOpenRetrievalStatus = PacketStore::Suspended;
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
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if ((not supportsConcurrentRetrievalRequests) and packetStores[currPacketStoreId].selfOpenRetrievalStatus ==
		                                                      PacketStore::InProgress) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		if (packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::ByTimeRangeRetrievalAlreadyEnabled);
			continue;
		}
		if (supportsPrioritizingRetrievals) {
			uint16_t priority = request.readUint16();
			packetStores[currPacketStoreId].retrievalPriority = priority;
		}
		uint32_t retrievalStartTime = request.readUint32();
		uint32_t retrievalEndTime = request.readUint32();
		if (retrievalStartTime >= retrievalEndTime) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::InvalidRetrievalTimesRelation);
			continue;
		}
		/**
		 * @todo: 6.15.3.5.2.d(4)
		 */
		packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus = true;
		packetStores[currPacketStoreId].retrievalStartTime = retrievalStartTime;
		packetStores[currPacketStoreId].retrievalEndTime = retrievalEndTime;
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
		for (auto &packetStore : packetStores) {
			packetStore.second.selfByTimeRangeRetrievalStatus = false;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus = false;
	}
}

void StorageAndRetrievalService::packetStoresStatusReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportStatusOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message report(ServiceType,MessageType::PacketStoresStatusReport,Message::TM,1);
	report.appendUint16(packetStores.size());
	for (auto &packetStore : packetStores) {
		report.appendUint16(packetStore.first);
		report.appendBoolean(packetStore.second.selfStorageStatus);
		uint16_t statusCode = (packetStore.second.selfOpenRetrievalStatus == PacketStore::InProgress) ? 0 : 1;
		report.appendUint16(statusCode);
		report.appendBoolean(packetStore.second.selfByTimeRangeRetrievalStatus);
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

	uint32_t timeLimit = request.readUint32();
	uint16_t numOfPacketStores = request.readUint16();
	if (!numOfPacketStores) {
		for (auto &packetStore : packetStores) {
			for (auto &tmPacket : packetStore.second.storedTmPackets) {
				if (tmPacket.first <= timeLimit) {
					packetStore.second.storedTmPackets.pop_front();
				} else {
					break;
				}
			}
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		if (packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		if (packetStores[currPacketStoreId].selfOpenRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		for (int j = 0; j < packetStores[currPacketStoreId].storedTmPackets.size(); j++) {
			/**
			 * @todo: actually compare the real time structures.
			 */
			if (packetStores[currPacketStoreId].storedTmPackets[j].first <= timeLimit) {
				packetStores[currPacketStoreId].storedTmPackets.pop_front();
			} else {
				break;
			}
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
		if (packetStores.size() >= maxPacketStores) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxNumberOfPacketStoresReached);
			return;
		}
		uint16_t idToCreate = request.readUint16();
//		uint16_t packetStoreSize = request.readUint16();
		uint16_t typeCode = request.readUint16();
		PacketStore::PacketStoreType packetStoreType = (!typeCode) ? PacketStore::Circular : PacketStore::Bounded;

		/**
		 * @todo: actually check if the available memory can handle the new creation
		 *
		 * @todo: read the virtual channel. Don't know the data-type yet
		 *
		 * @todo: check if the virtual channel is valid
		 */

		 if (packetStores.find(idToCreate) != packetStores.end()) {
			 ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingPacketStore);
			 continue;
		 }
		PacketStore newPacketStore;
		newPacketStore.packetStoreType = packetStoreType;
		newPacketStore.selfStorageStatus = false;
		newPacketStore.selfByTimeRangeRetrievalStatus = false;
		newPacketStore.selfOpenRetrievalStatus = PacketStore::Suspended;
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
		for (int i = 0; i < packetStores.size(); i++) {
			if (packetStores[i].selfStorageStatus) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
				continue;
			}
			if (packetStores[i].selfByTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithByTimeRangeRetrieval);
				continue;
			}
			if(packetStores[i].selfOpenRetrievalStatus == PacketStore::InProgress) {
				ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithOpenRetrievalInProgress);
				continue;
			}
			packetStores.erase(i);
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t idToDelete = request.readUint16();
		if (packetStores.find(idToDelete) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		if (packetStores[idToDelete].selfStorageStatus) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStores[idToDelete].selfByTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithByTimeRangeRetrieval);
			continue;
		}
		if(packetStores[idToDelete].selfOpenRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithOpenRetrievalInProgress);
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

	Message report(ServiceType,MessageType::PacketStoreConfigurationReport,Message::TM,1);
	report.appendUint16(packetStores.size());
	for (auto &packetStore : packetStores) {
		uint16_t currPacketStoreId = packetStore.first;
		request.appendUint16(currPacketStoreId);
		/**
		 * @todo: append packet store size in bytes
		 */
		PacketStore::PacketStoreType currPacketStoreType = packetStore.second.packetStoreType;
		uint16_t typeCode = (currPacketStoreType == PacketStore::Circular) ? 0 : 1;
		request.appendUint16(typeCode);
		/**
		 * @todo: append virtual channel. Don't know the data-type yet.
		 */
	}
	storeMessage(report);
}

void StorageAndRetrievalService::copyPacketsInTimeWindow(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::CopyPacketsInTimeWindow, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

//	uint16_t timeTagsTypeCode = request.readUint16();
//	TimeStamping timeTagsType = (!timeTagsTypeCode) ? StorageBased : PacketBased; // How do I use that??
	uint32_t timeTag1 = request.readUint32();
	uint32_t timeTag2 = request.readUint32();
	uint16_t fromPacketStoreId = request.readUint16();
	uint16_t toPacketStoreId = request.readUint16();
	if (fromPacketStoreId >= maxPacketStores) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (toPacketStoreId >= maxPacketStores) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (timeTag1 >= timeTag2) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
		return;
	}
	if (!packetStores[toPacketStoreId].storedTmPackets.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DestinationPacketStoreNotEmtpy);
		return;
	}
	packetStores[fromPacketStoreId].copyPacketsTo(packetStores[toPacketStoreId], timeTag1, timeTag2);
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
		uint16_t currPacketStoreId = request.readUint16();
		uint16_t currPacketStoreSize = request.readUint16();    //In bytes
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		/**
		 * @todo: check if the current memory availability can handle the new size requested
		 */
		if (packetStores[currPacketStoreId].selfStorageStatus) {
			ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStores[currPacketStoreId].selfOpenRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		if (packetStores[currPacketStoreId].selfByTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStores[currPacketStoreId].sizeInBytes = currPacketStoreSize;
	}
}

void StorageAndRetrievalService::changeTypeToCircular(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ChangeTypeToCircular, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t idToChange = request.readUint16();
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (packetStores[idToChange].selfStorageStatus) {
		ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStores[idToChange].selfByTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].selfOpenRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
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

	uint16_t idToChange = request.readUint16();
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (packetStores[idToChange].selfStorageStatus) {
		ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStores[idToChange].selfByTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].selfOpenRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
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

	uint16_t idToChange = request.readUint16();
	/**
	 * @todo: read the requested virtual channel and check if its valid
	 */
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (packetStores[idToChange].selfByTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStores[idToChange].selfOpenRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	/**
	 * @todo: actually change the virtual channel utilized by the packet store
	 */
}

void StorageAndRetrievalService::packetStoreContentSummaryReport(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::ReportContentSummaryOfPacketStores, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	Message report(ServiceType,MessageType::PacketStoreContentSummaryReport,Message::TM,1);
	uint16_t numOfPacketStores = request.readUint16();
	//For all packet stores
	if (!numOfPacketStores) {
		report.appendUint16(packetStores.size());
		for (auto &packetStore : packetStores) {
			report.appendUint16(packetStore.first);
			uint32_t oldestStoredPacketTime = packetStore.second.storedTmPackets.front().first;
			report.appendUint32(oldestStoredPacketTime);
			uint32_t newestStoredPacketTime = packetStore.second.storedTmPackets.back().first;
			report.appendUint32(newestStoredPacketTime);
			auto fillingPercentage = static_cast <uint16_t> (packetStore.second.storedTmPackets.size() * 100
			                                               / ECSS_MAX_PACKETS_IN_PACKET_STORE);
			report.appendUint16(fillingPercentage);
			uint16_t packetCounter = 0;
			for (auto &packet : packetStore.second.storedTmPackets) {
				if (packetStore.second.openRetrievalStartTimeTag <= packet.first) {
					packetCounter++;
				}
			}
			fillingPercentage = static_cast <uint16_t> (packetCounter * 100 / ECSS_MAX_PACKETS_IN_PACKET_STORE);
			report.appendUint16(fillingPercentage);
		}
		return;
	}
	//For specified packet stores
	uint16_t numOfValidPacketStores = 0;
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) != packetStores.end()) {
			numOfValidPacketStores++;;
		}
	}
	report.appendUint16(numOfValidPacketStores);
	request.resetRead();
	numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		uint16_t currPacketStoreId = request.readUint16();
		if (packetStores.find(currPacketStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		report.appendUint16(currPacketStoreId);
		uint32_t oldestStoredPacketTime = packetStores[currPacketStoreId].storedTmPackets.front().first;
		report.appendUint32(oldestStoredPacketTime);
		uint32_t newestStoredPacketTime = packetStores[currPacketStoreId].storedTmPackets.back().first;
		report.appendUint32(newestStoredPacketTime);
		report.appendUint32(packetStores[currPacketStoreId].openRetrievalStartTimeTag);
		auto fillingPercentage = static_cast <uint16_t> (packetStores[currPacketStoreId].storedTmPackets.size() * 100
		                                               / ECSS_MAX_PACKETS_IN_PACKET_STORE);
		report.appendUint16(fillingPercentage);
		uint16_t packetCounter = 0;
		for (auto &packet : packetStores[currPacketStoreId].storedTmPackets) {
			if (packetStores[currPacketStoreId].openRetrievalStartTimeTag <= packet.first) {
				packetCounter++;
			}
		}
		fillingPercentage = static_cast <uint16_t> (packetCounter * 100 / ECSS_MAX_PACKETS_IN_PACKET_STORE);
		report.appendUint16(fillingPercentage);
	}
}

/********************************    Packet Selection Subservice    *********************************/

StorageAndRetrievalService::PacketSelectionSubservice::PacketSelectionSubservice(StorageAndRetrievalService& parent,
                                                                                 uint16_t numOfControlledAppProcs,
                                                                                 uint16_t maxEventDefIds,
                                                                                 uint16_t maxHousekeepingStructIds,
                                                                                 uint16_t maxReportTypeDefs,
                                                                                 uint16_t maxServiceTypeDefs)
    : mainService(parent), numOfControlledAppProcesses(numOfControlledAppProcs), maxEventDefinitionIds(maxEventDefIds),
      maxHousekeepingStructureIds(maxHousekeepingStructIds), maxReportTypeDefinitions(maxReportTypeDefs),
      maxServiceTypeDefinitions(maxServiceTypeDefs) {}

bool StorageAndRetrievalService::PacketSelectionSubservice::appIsControlled(uint16_t applicationId, Message& request) {
	if (std::find(controlledAppProcesses.begin(), controlledAppProcesses.end(), applicationId) ==
	    controlledAppProcesses.end()) {
		ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::UnControlledApplicationProcessId);
		return false;
	}
	return true;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxReportDefinitions(uint16_t applicationId,
                                                                                        uint16_t serviceId,
                                                                                        Message& request) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].size() >=
	    maxReportTypeDefinitions) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxReportTypeDefinitionsReached);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::exceedsMaxServiceDefinitions(uint16_t applicationId,Message& request) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId].size() >=maxServiceTypeDefinitions) {
		ErrorHandler::reportError(request,ErrorHandler::ExecutionStartErrorType::MaxServiceTypeDefinitionsReached);
		return true;
	}
	return false;

}

bool StorageAndRetrievalService::PacketSelectionSubservice::noReportDefinitionInService(uint16_t applicationId,
                                                                                        uint16_t serviceId,
                                                                                        Message& request) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NoExistingReportTypeDefinitionInService);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::noServiceDefinitionInApplication(uint16_t applicationId,
                                                                                             Message& request) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId].empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NoExistingReportTypeDefinitionInService);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appExistsInDefinition(uint16_t applicationId) {
	if (applicationProcessConfiguration.applicationProcessDefinitions.find(applicationId) !=
	    applicationProcessConfiguration.applicationProcessDefinitions.end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createAppDefinition(uint16_t applicationId){
	typedef etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> vecType;
	etl::map <uint16_t, vecType, ECSS_MAX_SERVICE_TYPE_DEFINITIONS> tempMap;
	applicationProcessConfiguration.applicationProcessDefinitions.insert({applicationId, tempMap});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::serviceExistsInApp(uint16_t applicationId,
                                                                               uint16_t serviceId) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId].find(serviceId) !=
	    applicationProcessConfiguration.applicationProcessDefinitions[applicationId].end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createServiceDefinition(uint16_t applicationId,uint16_t serviceId){
	etl::vector <uint16_t, ECSS_MAX_MESSAGE_TYPE_DEFINITIONS> tempVec;
	applicationProcessConfiguration.applicationProcessDefinitions[applicationId].insert({serviceId, tempVec});
}

bool StorageAndRetrievalService::PacketSelectionSubservice::reportExistsInService(uint16_t applicationId,
                                                                                  uint16_t serviceId,
                                                                                  uint16_t reportId) {
	if (std::find(applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].begin(),
	              applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].end(),
	              reportId) != applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].end()) {
		return true;
	}
	return false;
}

void StorageAndRetrievalService::PacketSelectionSubservice::createReportDefinition(uint16_t applicationId,
                                                                                   uint16_t serviceId,
                                                                                   uint16_t reportId) {
	applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].push_back(reportId);
}

bool StorageAndRetrievalService::PacketSelectionSubservice::serviceHasReportDefinitions(uint16_t applicationId,
                                                                                        uint16_t serviceId) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].empty()) {
		return false;
	}
	return true;
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteReportDefinitionsOfService(uint16_t applicationId,
                                                                                             uint16_t serviceId) {
	applicationProcessConfiguration.applicationProcessDefinitions[applicationId][serviceId].clear();
}

bool StorageAndRetrievalService::PacketSelectionSubservice::appHasServiceDefinitions(uint16_t applicationId) {
	if (applicationProcessConfiguration.applicationProcessDefinitions[applicationId].empty()) {
		return false;
	}
	return true;
}

void StorageAndRetrievalService::PacketSelectionSubservice::deleteServiceDefinitionsOfApp(uint16_t applicationId) {
	applicationProcessConfiguration.applicationProcessDefinitions[applicationId].clear();
}

void StorageAndRetrievalService::PacketSelectionSubservice::addReportTypesToAppProcessConfiguration(Message& request) {
	ErrorHandler::assertRequest(request.packetType == Message::TC, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.messageType == MessageType::AddReportTypesToAppProcessConfiguration, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(request.serviceType == ServiceType, request,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t packetStoreId = request.readUint16();
	if (mainService.packetStores.find(packetStoreId) == mainService.packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	uint16_t numOfApplicationIds = request.readUint16();
	//Per application process
	for (int i = 0; i < numOfApplicationIds; i++) {
		uint16_t currentAppId = request.readUint16();
		if (not appIsControlled(currentAppId, request)) {
			continue;
		}
		if (not appExistsInDefinition(currentAppId)) {
			createAppDefinition(currentAppId);
		}
//		if (noServiceDefinitionInApplication(currentAppId, request)) {
//			continue;
//		}
		uint16_t numOfCurrAppServices = request.readUint16();
		if (!numOfCurrAppServices and (appHasServiceDefinitions(currentAppId))) {
			deleteServiceDefinitionsOfApp(currentAppId);
			continue;
		}
		if (exceedsMaxServiceDefinitions(currentAppId, request)) {
			continue;
		}
		//Per service type in application process
		for (int j = 0; j < numOfCurrAppServices; j++) {
			uint16_t currentServiceId = request.readUint16();
			if (exceedsMaxReportDefinitions(currentAppId, currentServiceId, request)) {
				continue;
			}
//			if (noReportDefinitionInService(currentAppId, currentServiceId, request)) {
//				continue;
//			}
			if (not serviceExistsInApp(currentAppId, currentServiceId)) {
				createServiceDefinition(currentAppId, currentServiceId);
			}
			uint16_t numOfCurrServiceMessageTypes = request.readUint16();
			if ((!numOfCurrServiceMessageTypes) and (serviceHasReportDefinitions(currentAppId, currentServiceId))) {
				deleteReportDefinitionsOfService(currentAppId, currentServiceId);
				continue;
			}
			//Per message type per service type
			for (int k = 0; k < numOfCurrServiceMessageTypes; k++) {
				uint16_t currentMessageType = request.readUint16();
				if (not reportExistsInService(currentAppId, currentServiceId, currentMessageType)) {
					createReportDefinition(currentAppId, currentServiceId, currentMessageType);
				}

			}
		}
	}
}
