#include <iostream>
#include "Services/StorageAndRetrievalService.hpp"

/******************************************************************************
 ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 	  Storage and Retrieval     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
 ******************************************************************************/

String<ECSSMaxPacketStoreIdSize> StorageAndRetrievalService::readPacketStoreId(Message& message) {
	uint8_t packetStoreData[ECSSMaxPacketStoreIdSize];
	message.readOctetString(packetStoreData);
	String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);
	return packetStoreId;
}

void StorageAndRetrievalService::deleteContentUntil(const String<ECSSMaxPacketStoreIdSize>& packetStoreId,
                                                    uint32_t timeLimit) {
	for (auto& tmPacket : packetStores[packetStoreId].storedTelemetryPackets) {
		/**
		 * @todo: actually compare the real time formats.
		 */
		if (tmPacket.first > timeLimit) {
			break;
		}
		packetStores[packetStoreId].storedTelemetryPackets.pop_front();
	}
}

void StorageAndRetrievalService::copyFromTagToTag(PacketStore& source, PacketStore& target, uint32_t startTime,
                                                  uint32_t endTime) {
	for (auto& packet : source.storedTelemetryPackets) {
		if (packet.first < startTime) {
			continue;
		}
		if (packet.first > endTime) {
			break;
		}
		target.storedTelemetryPackets.push_back(packet);
	}
}

void StorageAndRetrievalService::copyAfterTimeTag(PacketStore& source, PacketStore& target, uint32_t startTime) {
	for (auto& packet : source.storedTelemetryPackets) {
		if (packet.first < startTime) {
			continue;
		}
		target.storedTelemetryPackets.push_back(packet);
	}
}

void StorageAndRetrievalService::copyBeforeTimeTag(PacketStore& source, PacketStore& target, uint32_t endTime) {
	for (auto& packet : source.storedTelemetryPackets) {
		if (packet.first > endTime) {
			break;
		}
		target.storedTelemetryPackets.push_back(packet);
	}
}

bool StorageAndRetrievalService::copyPacketsFrom(PacketStore& source, PacketStore& target, uint32_t startTime,
                                                 uint32_t endTime, TimeWindowType timeWindow) {
	switch (timeWindow) {
		case 0:
			if (endTime < source.storedTelemetryPackets.front().first) {
				return false;
			}
			copyFromTagToTag(source, target, startTime, endTime);
			break;
		case 1:
			copyAfterTimeTag(source, target, startTime);
			break;
		case 2:
			copyBeforeTimeTag(source, target, endTime);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::InvalidTimeWindowType);
			return false;
	}
	return true;
}

void StorageAndRetrievalService::createContentSummary(Message& report,
                                                      String<ECSSMaxPacketStoreIdSize>& packetStoreId) {
	uint32_t oldestStoredPacketTime = packetStores[packetStoreId].storedTelemetryPackets.front().first;
	report.appendUint32(oldestStoredPacketTime);

	uint32_t newestStoredPacketTime = packetStores[packetStoreId].storedTelemetryPackets.back().first;
	report.appendUint32(newestStoredPacketTime);

	report.appendUint32(packetStores[packetStoreId].openRetrievalStartTimeTag);

	auto fillingPercentage =
	    static_cast<uint16_t>(packetStores[packetStoreId].storedTelemetryPackets.size() * 100 / ECSSMaxPacketStoreSize);
	report.appendUint16(fillingPercentage);

	uint16_t numOfPacketsToBeTransferred = 0;
	for (auto& packet : packetStores[packetStoreId].storedTelemetryPackets) {
		if (packet.first >= packetStores[packetStoreId].openRetrievalStartTimeTag) {
			numOfPacketsToBeTransferred++;
		}
	}
	fillingPercentage = static_cast<uint16_t>(numOfPacketsToBeTransferred * 100 / ECSSMaxPacketStoreSize);
	report.appendUint16(fillingPercentage);
}

void StorageAndRetrievalService::enableStorageFunction(Message& request) {
	request.assertTC(ServiceType, MessageType::EnableStorageInPacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore : packetStores) {
			packetStore.second.storageStatus = true;
		}
		return;
	}

	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].storageStatus = true;
	}
}

void StorageAndRetrievalService::disableStorageFunction(Message& request) {
	request.assertTC(ServiceType, MessageType::DisableStorageInPacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore : packetStores) {
			packetStore.second.storageStatus = false;
		}
		return;
	}

	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].storageStatus = false;
	}
}

void StorageAndRetrievalService::changeOpenRetrievalStartTimeTag(Message& request) {
	request.assertTC(ServiceType, MessageType::ChangeOpenRetrievalStartingTime);

	uint32_t newStartTimeTag = request.readUint32();
	/**
	 * @todo: check if newStartTimeTag is in the future
	 */
	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
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
		auto packetStoreId = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::ResumeOpenRetrievalOfPacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
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
		auto packetStoreId = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::SuspendOpenRetrievalOfPacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore : packetStores) {
			packetStore.second.openRetrievalStatus = PacketStore::Suspended;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
	}
}

void StorageAndRetrievalService::startByTimeRangeRetrieval(Message& request) {
	request.assertTC(ServiceType, MessageType::StartByTimeRangeRetrieval);

	uint16_t numOfPacketStores = request.readUint16();
	bool errorFlag = false;
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			errorFlag = true;
		} else if ((not supportsConcurrentRetrievalRequests) and
		           packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			errorFlag = true;
		} else if (packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::ByTimeRangeRetrievalAlreadyEnabled);
			errorFlag = true;
		}
		if (errorFlag) {
			uint16_t numberOfBytesToSkip = (supportsPrioritizingRetrievals) ? 10 : 8;
			request.skipBytes(numberOfBytesToSkip);
			errorFlag = false;
			continue;
		}
		uint16_t priority = 0;
		if (supportsPrioritizingRetrievals) {
			priority = request.readUint16();
		}
		uint32_t retrievalStartTime = request.readUint32();
		uint32_t retrievalEndTime = request.readUint32();

		if (retrievalStartTime >= retrievalEndTime) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
			continue;
		}
		//		if (retrievalEndTime <= packetStores[packetStoreId].storedTelemetryPackets.end()->first and
		//		retrievalEndTime <=
		//		                                                                                         timeNow) {
		//
		//		}
		/**
		 * @todo: 6.15.3.5.2.d(4), actually count the current time
		 */
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = true;
		packetStores[packetStoreId].retrievalStartTime = retrievalStartTime;
		packetStores[packetStoreId].retrievalEndTime = retrievalEndTime;
		if (supportsPrioritizingRetrievals) {
			packetStores[packetStoreId].retrievalPriority = priority;
		}
		/**
		 * @todo: start the by-time-range retrieval process according to the priority policy
		 */
	}
}

void StorageAndRetrievalService::abortByTimeRangeRetrieval(Message& request) {
	request.assertTC(ServiceType, MessageType::AbortByTimeRangeRetrieval);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore : packetStores) {
			packetStore.second.byTimeRangeRetrievalStatus = false;
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::SetNonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
	}
}

void StorageAndRetrievalService::packetStoresStatusReport(Message& request) {
	request.assertTC(ServiceType, MessageType::ReportStatusOfPacketStores);

	Message report(ServiceType, MessageType::PacketStoresStatusReport, Message::TM, 1);
	report.appendUint16(packetStores.size());
	for (auto& packetStore : packetStores) {
		auto packetStoreId = packetStore.first;
		report.appendOctetString(packetStoreId);
		report.appendBoolean(packetStore.second.storageStatus);
		uint8_t code = (packetStore.second.openRetrievalStatus == PacketStore::InProgress) ? 0 : 1;
		report.appendUint8(code);
		if (supportsByTimeRangeRetrieval) {
			report.appendBoolean(packetStore.second.byTimeRangeRetrievalStatus);
		}
	}
	storeMessage(report);
}

void StorageAndRetrievalService::deletePacketStoreContent(Message& request) {
	request.assertTC(ServiceType, MessageType::DeletePacketStoreContent);

	uint32_t timeLimit = request.readUint32(); // todo: decide the time-format
	uint16_t numOfPacketStores = request.readUint16();

	if (numOfPacketStores == 0) {
		for (auto& packetStore : packetStores) {
			if (packetStore.second.byTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
				continue;
			}
			if (packetStore.second.openRetrievalStatus == PacketStore::InProgress) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
				continue;
			}
			deleteContentUntil(packetStore.first, timeLimit);
		}
		return;
	}
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
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
		deleteContentUntil(packetStoreId, timeLimit);
	}
}

void StorageAndRetrievalService::createPacketStores(Message& request) {
	request.assertTC(ServiceType, MessageType::CreatePacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		if (packetStores.size() >= ECSSMaxPacketStores) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxNumberOfPacketStoresReached);
			return;
		}
		auto idToCreate = readPacketStoreId(request);

		if (packetStores.find(idToCreate) != packetStores.end()) {
			uint16_t numberOfBytesToSkip = 4;
			request.skipBytes(numberOfBytesToSkip);
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingPacketStore);
			continue;
		}
		uint16_t packetStoreSize = request.readUint16();
		uint8_t typeCode = request.readUint8();
		PacketStore::PacketStoreType packetStoreType = (typeCode == 0) ? PacketStore::Circular : PacketStore::Bounded;
		uint8_t virtualChannel = request.readUint8();

		if (virtualChannel < MIN or virtualChannel > MAX) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel);
			continue;
		}
		/**
		 * @todo: actually check if the available memory can handle the new creation
		 */
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
	request.assertTC(ServiceType, MessageType::DeletePacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		int numOfPacketStoresToDelete = 0;
		etl::string<ECSSMaxPacketStoreIdSize> packetStoresToDelete[packetStores.size()];
		for (auto& packetStore : packetStores) {
			if (packetStore.second.storageStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketStoreWithStorageStatusEnabled);
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
			packetStoresToDelete[numOfPacketStoresToDelete] = packetStore.first;
			numOfPacketStoresToDelete++;
		}
		for (int l = 0; l < numOfPacketStoresToDelete; l++) {
			uint8_t data[ECSSMaxPacketStoreIdSize];
			etl::string<ECSSMaxPacketStoreIdSize> idToDelete = packetStoresToDelete[l];
			std::copy(idToDelete.begin(), idToDelete.end(), data);
			String<ECSSMaxPacketStoreIdSize> key(data);
			packetStores.erase(key);
		}
		return;
	}

	for (int i = 0; i < numOfPacketStores; i++) {
		auto idToDelete = readPacketStoreId(request);
		if (packetStores.find(idToDelete) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
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
	request.assertTC(ServiceType, MessageType::ReportConfigurationOfPacketStores);

	Message report(ServiceType, MessageType::PacketStoreConfigurationReport, Message::TM, 1);
	report.appendUint16(packetStores.size());
	for (auto& packetStore : packetStores) {
		auto packetStoreId = packetStore.first;
		report.appendOctetString(packetStoreId);
		report.appendUint16(packetStore.second.sizeInBytes);
		uint8_t typeCode = (packetStore.second.packetStoreType == PacketStore::Circular) ? 0 : 1;
		report.appendUint8(typeCode);
		report.appendUint8(packetStore.second.virtualChannel);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::copyPacketsInTimeWindow(Message& request, TimeWindowType timeWindow) {
	request.assertTC(ServiceType, MessageType::CopyPacketsInTimeWindow);

	request.skipBytes(2);
	//	uint16_t timeTagsTypeCode = request.readUint16();
	//	TimeStamping timeTagsType = (!timeTagsTypeCode) ? StorageBased : PacketBased; //todo: actually figure out
	uint32_t timeTag1 = request.readUint32();
	uint32_t timeTag2 = request.readUint32();
	auto fromPacketStoreId = readPacketStoreId(request);
	auto toPacketStoreId = readPacketStoreId(request);

	if (packetStores.find(fromPacketStoreId) == packetStores.end() or
	    packetStores.find(toPacketStoreId) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
		return;
	}
	if (timeTag1 >= timeTag2) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
		return;
	}
	if (not packetStores[toPacketStoreId].storedTelemetryPackets.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DestinationPacketStoreNotEmtpy);
		return;
	}
	if (not copyPacketsFrom(packetStores[fromPacketStoreId], packetStores[toPacketStoreId], timeTag1, timeTag2,
	                        timeWindow)) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
	}
}

void StorageAndRetrievalService::resizePacketStores(Message& request) {
	request.assertTC(ServiceType, MessageType::ResizePacketStores);

	uint16_t numOfPacketStores = request.readUint16();
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::ChangeTypeToCircular);

	auto idToChange = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::ChangeTypeToBounded);

	auto idToChange = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::ChangeVirtualChannel);

	auto idToChange = readPacketStoreId(request);
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
	request.assertTC(ServiceType, MessageType::ReportContentSummaryOfPacketStores);

	Message report(ServiceType, MessageType::PacketStoreContentSummaryReport, Message::TM, 1);
	uint16_t numOfPacketStores = request.readUint16();
	// For all packet stores
	if (numOfPacketStores == 0) {
		report.appendUint16(packetStores.size());
		for (auto& packetStore : packetStores) {
			auto packetStoreId = packetStore.first;
			report.appendOctetString(packetStoreId);
			createContentSummary(report, packetStoreId);
		}
		storeMessage(report);
		return;
	}
	// For specified packet stores
	uint16_t numOfValidPacketStores = 0;
	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) != packetStores.end()) {
			numOfValidPacketStores++;
		}
	}
	report.appendUint16(numOfValidPacketStores);
	request.resetRead();
	numOfPacketStores = request.readUint16();

	for (int i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::GetNonExistingPacketStore);
			continue;
		}
		report.appendOctetString(packetStoreId);
		createContentSummary(report, packetStoreId);
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
			copyPacketsInTimeWindow(request, FromTagToTag);
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
