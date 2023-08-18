#include "Services/StorageAndRetrievalService.hpp"

String<ECSSPacketStoreIdSize> StorageAndRetrievalService::readPacketStoreId(Message& message) {
	uint8_t packetStoreId[ECSSPacketStoreIdSize];
	message.readString(packetStoreId, ECSSPacketStoreIdSize);
	return packetStoreId;
}

void StorageAndRetrievalService::deleteContentUntil(const String<ECSSPacketStoreIdSize>& packetStoreId,
                                                    TimeSize timeLimit) {
	auto& telemetryPackets = packetStores[packetStoreId].storedTelemetryPackets;
	while (not telemetryPackets.empty() and telemetryPackets.front().first <= timeLimit) {
		telemetryPackets.pop_front();
	}
}

void StorageAndRetrievalService::copyFromTagToTag(Message& request) {
	TimeSize startTime = request.readUint32();
	TimeSize endTime = request.readUint32();

	auto fromPacketStoreId = readPacketStoreId(request);
	auto toPacketStoreId = readPacketStoreId(request);

	if (failedFromTagToTag(fromPacketStoreId, toPacketStoreId, startTime, endTime, request)) {
		return;
	}

	for (auto& packet: packetStores[fromPacketStoreId].storedTelemetryPackets) {
		if (packet.first < startTime) {
			continue;
		}
		if (packet.first > endTime) {
			break;
		}
		packetStores[toPacketStoreId].storedTelemetryPackets.push_back(packet);
	}
}

void StorageAndRetrievalService::copyAfterTimeTag(Message& request) {
	TimeSize startTime = request.readUint32();

	auto fromPacketStoreId = readPacketStoreId(request);
	auto toPacketStoreId = readPacketStoreId(request);

	if (failedAfterTimeTag(fromPacketStoreId, toPacketStoreId, startTime, request)) {
		return;
	}

	for (auto& packet: packetStores[fromPacketStoreId].storedTelemetryPackets) {
		if (packet.first < startTime) {
			continue;
		}
		packetStores[toPacketStoreId].storedTelemetryPackets.push_back(packet);
	}
}

void StorageAndRetrievalService::copyBeforeTimeTag(Message& request) {
	TimeSize endTime = request.readUint32();

	auto fromPacketStoreId = readPacketStoreId(request);
	auto toPacketStoreId = readPacketStoreId(request);

	if (failedBeforeTimeTag(fromPacketStoreId, toPacketStoreId, endTime, request)) {
		return;
	}

	for (auto& packet: packetStores[fromPacketStoreId].storedTelemetryPackets) {
		if (packet.first > endTime) {
			break;
		}
		packetStores[toPacketStoreId].storedTelemetryPackets.push_back(packet);
	}
}

bool StorageAndRetrievalService::checkPacketStores(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                   const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                   Message& request) {
	if (packetStores.find(fromPacketStoreId) == packetStores.end() or
	    packetStores.find(toPacketStoreId) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return false;
	}
	return true;
}

bool StorageAndRetrievalService::checkTimeWindow(TimeSize startTime, TimeSize endTime, Message& request) {
	if (startTime >= endTime) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::checkDestinationPacketStore(const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                             Message& request) {
	if (not packetStores[toPacketStoreId].storedTelemetryPackets.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DestinationPacketStoreNotEmtpy);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                         TimeSize startTime, TimeSize endTime, Message& request) {
	if (endTime < packetStores[fromPacketStoreId].storedTelemetryPackets.front().first ||
	    startTime > packetStores[fromPacketStoreId].storedTelemetryPackets.back().first) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                         TimeSize timeTag, Message& request, bool isAfterTimeTag) {
	if (isAfterTimeTag) {
		if (timeTag > packetStores[fromPacketStoreId].storedTelemetryPackets.back().first) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
			return true;
		}
		return false;
	} else if (timeTag < packetStores[fromPacketStoreId].storedTelemetryPackets.front().first) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::failedFromTagToTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                    const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                    TimeSize startTime, TimeSize endTime, Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkTimeWindow(startTime, endTime, request) or checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, startTime, endTime, request));
}

bool StorageAndRetrievalService::failedAfterTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                    const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                    TimeSize startTime, Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, startTime, request, true));
}

bool StorageAndRetrievalService::failedBeforeTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                     const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                     TimeSize endTime, Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, endTime, request, false));
}

void StorageAndRetrievalService::createContentSummary(Message& report,
                                                      const String<ECSSPacketStoreIdSize>& packetStoreId) {
	TimeSize oldestStoredPacketTime = packetStores[packetStoreId].storedTelemetryPackets.front().first;
	report.appendUint32(oldestStoredPacketTime);

	TimeSize newestStoredPacketTime = packetStores[packetStoreId].storedTelemetryPackets.back().first;
	report.appendUint32(newestStoredPacketTime);

	report.appendUint32(packetStores[packetStoreId].openRetrievalStartTimeTag);

	auto filledPercentage1 = static_cast<uint16_t>(static_cast<float>(packetStores[packetStoreId].storedTelemetryPackets.size()) * 100 /
	                                               ECSSMaxPacketStoreSize);
	report.appendUint16(filledPercentage1);

	uint16_t numOfPacketsToBeTransferred = 0;
	numOfPacketsToBeTransferred = std::count_if(
	    std::begin(packetStores[packetStoreId].storedTelemetryPackets),
	    std::end(packetStores[packetStoreId].storedTelemetryPackets), [this, &packetStoreId](auto packet) {
		    return packet.first >= packetStores[packetStoreId].openRetrievalStartTimeTag;
	    });
	auto filledPercentage2 = static_cast<uint16_t>(static_cast<float>(numOfPacketsToBeTransferred) * 100 / ECSSMaxPacketStoreSize);
	report.appendUint16(filledPercentage2);
}

bool StorageAndRetrievalService::failedStartOfByTimeRangeRetrieval(
    const String<ECSSPacketStoreIdSize>& packetStoreId, Message& request) {
	bool errorFlag = false;

	if (packetStores.find(packetStoreId) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		errorFlag = true;
	} else if (packetStores[packetStoreId].openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		errorFlag = true;
	} else if (packetStores[packetStoreId].byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::ByTimeRangeRetrievalAlreadyEnabled);
		errorFlag = true;
	}
	if (errorFlag) {
		uint16_t numberOfBytesToSkip = 8;
		request.skipBytes(numberOfBytesToSkip);
		return true;
	}
	return false;
}

void StorageAndRetrievalService::addPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId,
                                                const PacketStore& packetStore) {
	packetStores.insert({packetStoreId, packetStore});
}

void StorageAndRetrievalService::addTelemetryToPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId,
                                                           TimeSize timestamp) {
	Message tmPacket;
	packetStores[packetStoreId].storedTelemetryPackets.push_back({timestamp, tmPacket});
}

void StorageAndRetrievalService::resetPacketStores() {
	packetStores.clear();
}

NumOfPacketStoresSize StorageAndRetrievalService::currentNumberOfPacketStores() {
	return packetStores.size();
}

PacketStore& StorageAndRetrievalService::getPacketStore(const String<ECSSPacketStoreIdSize>& packetStoreId) {
	auto packetStore = packetStores.find(packetStoreId);
	ASSERT_INTERNAL(packetStore != packetStores.end(), ErrorHandler::InternalErrorType::ElementNotInArray);
	return packetStore->second;
}

bool StorageAndRetrievalService::packetStoreExists(const String<ECSSPacketStoreIdSize>& packetStoreId) {
	return packetStores.find(packetStoreId) != packetStores.end();
}

void StorageAndRetrievalService::executeOnPacketStores(Message& request,
                                                       const std::function<void(PacketStore&)>& function) {
	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			function(packetStore.second);
		}
		return;
	}

	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		auto packetStore = packetStores.find(packetStoreId);
		if (packetStore == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		function(packetStores[packetStoreId]);
	}
}

void StorageAndRetrievalService::enableStorageFunction(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::EnableStorageInPacketStores)) {
		return;
	}

	executeOnPacketStores(request, [](PacketStore& p) { p.storageStatus = true; });
}

void StorageAndRetrievalService::disableStorageFunction(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DisableStorageInPacketStores)) {
		return;
	}

	executeOnPacketStores(request, [](PacketStore& p) { p.storageStatus = false; });
}

void StorageAndRetrievalService::startByTimeRangeRetrieval(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::StartByTimeRangeRetrieval)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	bool errorFlag = false;

	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (failedStartOfByTimeRangeRetrieval(packetStoreId, request)) {
			continue;
		}
		TimeSize retrievalStartTime = request.readUint32();
		TimeSize retrievalEndTime = request.readUint32();

		if (retrievalStartTime >= retrievalEndTime) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
			continue;
		}

		// todo: 6.15.3.5.2.d(4), actually count the current time

		auto& packetStore = packetStores[packetStoreId];
		packetStore.byTimeRangeRetrievalStatus = true;
		packetStore.retrievalStartTime = retrievalStartTime;
		packetStore.retrievalEndTime = retrievalEndTime;
		// todo: start the by-time-range retrieval process according to the priority policy
	}
}

void StorageAndRetrievalService::deletePacketStoreContent(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeletePacketStoreContent)) {
		return;
	}

	TimeSize timeLimit = request.readUint32(); // todo: decide the time-format
	NumOfPacketStoresSize numOfPacketStores = request.readUint16();

	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
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
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
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

void StorageAndRetrievalService::packetStoreContentSummaryReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportContentSummaryOfPacketStores)) {
		return;
	}

	Message report = createTM(PacketStoreContentSummaryReport);
	NumOfPacketStoresSize numOfPacketStores = request.readUint16();

	if (numOfPacketStores == 0) {
		report.appendUint16(packetStores.size());
		for (auto& packetStore: packetStores) {
			auto packetStoreId = packetStore.first;
			report.appendString(packetStoreId);
			createContentSummary(report, packetStoreId);
		}
		storeMessage(report);
		return;
	}
	NumOfPacketStoresSize numOfValidPacketStores = 0;
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) != packetStores.end()) {
			numOfValidPacketStores++;
		}
	}
	report.appendUint16(numOfValidPacketStores);
	request.resetRead();
	numOfPacketStores = request.readUint16();

	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		report.appendString(packetStoreId);
		createContentSummary(report, packetStoreId);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::changeOpenRetrievalStartTimeTag(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ChangeOpenRetrievalStartingTime)) {
		return;
	}

	TimeSize newStartTimeTag = request.readUint32();
	/**
	 * @todo: check if newStartTimeTag is in the future
	 */
	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			if (packetStore.second.openRetrievalStatus == PacketStore::InProgress) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithOpenRetrievalInProgress);
				continue;
			}
			packetStore.second.openRetrievalStartTimeTag = newStartTimeTag;
		}
		return;
	}

	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
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
	if (!request.assertTC(ServiceType, MessageType::ResumeOpenRetrievalOfPacketStores)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			if (packetStore.second.byTimeRangeRetrievalStatus) {
				ErrorHandler::reportError(
				    request, ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
				continue;
			}
			packetStore.second.openRetrievalStatus = PacketStore::InProgress;
		}
		return;
	}
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		auto& packetStore = packetStores[packetStoreId];
		if (packetStore.byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::SetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStore.openRetrievalStatus = PacketStore::InProgress;
	}
}

void StorageAndRetrievalService::suspendOpenRetrievalOfPacketStores(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::SuspendOpenRetrievalOfPacketStores)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			packetStore.second.openRetrievalStatus = PacketStore::Suspended;
		}
		return;
	}
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].openRetrievalStatus = PacketStore::Suspended;
	}
}

void StorageAndRetrievalService::abortByTimeRangeRetrieval(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::AbortByTimeRangeRetrieval)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			packetStore.second.byTimeRangeRetrievalStatus = false;
		}
		return;
	}
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
	}
}

void StorageAndRetrievalService::packetStoresStatusReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportStatusOfPacketStores)) {
		return;
	}

	Message report = createTM(PacketStoresStatusReport);
	report.appendUint16(packetStores.size());
	for (auto& packetStore: packetStores) {
		auto packetStoreId = packetStore.first;
		report.appendString(packetStoreId);
		report.appendBoolean(packetStore.second.storageStatus);
		report.appendEnum8(packetStore.second.openRetrievalStatus);
		report.appendBoolean(packetStore.second.byTimeRangeRetrievalStatus);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::createPacketStores(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CreatePacketStores)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
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
		NumOfPacketStoresSize packetStoreSize = request.readUint16();
		uint8_t typeCode = request.readUint8();
		PacketStore::PacketStoreType packetStoreType = (typeCode == 0) ? PacketStore::Circular : PacketStore::Bounded;
		uint8_t virtualChannel = request.readUint8();

		if (virtualChannel < VirtualChannelLimits.min or virtualChannel > VirtualChannelLimits.max) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel);
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
	if (!request.assertTC(ServiceType, MessageType::DeletePacketStores)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		NumOfPacketStoresSize numOfPacketStoresToDelete = 0;
		etl::string<ECSSPacketStoreIdSize> packetStoresToDelete[ECSSMaxPacketStores];
		for (auto& packetStore: packetStores) {
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
		for (NumOfPacketStoresSize l = 0; l < numOfPacketStoresToDelete; l++) {
			uint8_t data[ECSSPacketStoreIdSize];
			etl::string<ECSSPacketStoreIdSize> idToDelete = packetStoresToDelete[l];
			std::copy(idToDelete.begin(), idToDelete.end(), data);
			String<ECSSPacketStoreIdSize> key(data);
			packetStores.erase(key);
		}
		return;
	}

	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto idToDelete = readPacketStoreId(request);
		if (packetStores.find(idToDelete) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		auto& packetStore = packetStores[idToDelete];

		if (packetStore.storageStatus) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStore.byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithByTimeRangeRetrieval);
			continue;
		}
		if (packetStore.openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(
			    request, ErrorHandler::ExecutionStartErrorType::DeletionOfPacketWithOpenRetrievalInProgress);
			continue;
		}
		packetStores.erase(idToDelete);
	}
}

void StorageAndRetrievalService::packetStoreConfigurationReport(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportConfigurationOfPacketStores)) {
		return;
	}
	Message report = createTM(PacketStoreConfigurationReport);

	report.appendUint16(packetStores.size());
	for (auto& packetStore: packetStores) {
		auto packetStoreId = packetStore.first;
		report.appendString(packetStoreId);
		report.appendUint16(packetStore.second.sizeInBytes);
		uint8_t typeCode = (packetStore.second.packetStoreType == PacketStore::Circular) ? 0 : 1;
		report.appendUint8(typeCode);
		report.appendUint8(packetStore.second.virtualChannel);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::copyPacketsInTimeWindow(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CopyPacketsInTimeWindow)) {
		return;
	}

	uint8_t typeOfTimeWindow = request.readEnum8();
	switch (typeOfTimeWindow) {
		case FromTagToTag:
			copyFromTagToTag(request);
			break;
		case AfterTimeTag:
			copyAfterTimeTag(request);
			break;
		case BeforeTimeTag:
			copyBeforeTimeTag(request);
			break;
		default:
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
			break;
	}
}

void StorageAndRetrievalService::resizePacketStores(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ResizePacketStores)) {
		return;
	}

	NumOfPacketStoresSize numOfPacketStores = request.readUint16();
	for (NumOfPacketStoresSize i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		uint16_t packetStoreSize = request.readUint16(); // In bytes
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		auto& packetStore = packetStores[packetStoreId];

		if (packetStoreSize >= ECSSMaxPacketStoreSizeInBytes) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::UnableToHandlePacketStoreSize);
			continue;
		}
		if (packetStore.storageStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
			continue;
		}
		if (packetStore.openRetrievalStatus == PacketStore::InProgress) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
			continue;
		}
		if (packetStore.byTimeRangeRetrievalStatus) {
			ErrorHandler::reportError(request,
			                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
			continue;
		}
		packetStore.sizeInBytes = packetStoreSize;
	}
}

void StorageAndRetrievalService::changeTypeToCircular(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ChangeTypeToCircular)) {
		return;
	}

	auto idToChange = readPacketStoreId(request);
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}
	auto& packetStore = packetStores[idToChange];

	if (packetStore.storageStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStore.byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStore.openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStore.packetStoreType = PacketStore::Circular;
}

void StorageAndRetrievalService::changeTypeToBounded(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ChangeTypeToBounded)) {
		return;
	}

	auto idToChange = readPacketStoreId(request);
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}
	auto& packetStore = packetStores[idToChange];

	if (packetStore.storageStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled);
		return;
	}
	if (packetStore.byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStore.openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStore.packetStoreType = PacketStore::Bounded;
}

void StorageAndRetrievalService::changeVirtualChannel(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ChangeVirtualChannel)) {
		return;
	}

	auto idToChange = readPacketStoreId(request);
	uint8_t virtualChannel = request.readUint8();
	if (packetStores.find(idToChange) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return;
	}
	auto& packetStore = packetStores[idToChange];

	if (virtualChannel < VirtualChannelLimits.min or virtualChannel > VirtualChannelLimits.max) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel);
		return;
	}
	if (packetStore.byTimeRangeRetrievalStatus) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval);
		return;
	}
	if (packetStore.openRetrievalStatus == PacketStore::InProgress) {
		ErrorHandler::reportError(request,
		                          ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress);
		return;
	}
	packetStore.virtualChannel = virtualChannel;
}

void StorageAndRetrievalService::execute(Message& request) {
	switch (request.messageType) {
		case EnableStorageInPacketStores:
			enableStorageFunction(request);
			break;
		case DisableStorageInPacketStores:
			disableStorageFunction(request);
			break;
		case StartByTimeRangeRetrieval:
			startByTimeRangeRetrieval(request);
			break;
		case DeletePacketStoreContent:
			deletePacketStoreContent(request);
			break;
		case ReportContentSummaryOfPacketStores:
			packetStoreContentSummaryReport(request);
			break;
		case ChangeOpenRetrievalStartingTime:
			changeOpenRetrievalStartTimeTag(request);
			break;
		case ResumeOpenRetrievalOfPacketStores:
			resumeOpenRetrievalOfPacketStores(request);
			break;
		case SuspendOpenRetrievalOfPacketStores:
			suspendOpenRetrievalOfPacketStores(request);
			break;
		case AbortByTimeRangeRetrieval:
			abortByTimeRangeRetrieval(request);
			break;
		case ReportStatusOfPacketStores:
			packetStoresStatusReport(request);
			break;
		case CreatePacketStores:
			createPacketStores(request);
			break;
		case DeletePacketStores:
			deletePacketStores(request);
			break;
		case ReportConfigurationOfPacketStores:
			packetStoreConfigurationReport(request);
			break;
		case CopyPacketsInTimeWindow:
			copyPacketsInTimeWindow(request);
			break;
		case ResizePacketStores:
			resizePacketStores(request);
			break;
		case ChangeTypeToCircular:
			changeTypeToCircular(request);
			break;
		case ChangeTypeToBounded:
			changeTypeToBounded(request);
			break;
		case ChangeVirtualChannel:
			changeVirtualChannel(request);
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}
