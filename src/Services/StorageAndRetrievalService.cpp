#include "Services/StorageAndRetrievalService.hpp"

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
				ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketStoreWithStorageStatusEnabled);
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
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketStoreWithStorageStatusEnabled);
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
