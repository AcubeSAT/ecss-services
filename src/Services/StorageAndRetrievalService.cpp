#include "Services/StorageAndRetrievalService.hpp"

String<ECSSPacketStoreIdSize> StorageAndRetrievalService::readPacketStoreId(Message& message) {
	etl::array<uint8_t, ECSSPacketStoreIdSize> packetStoreId = {};
	message.readString(packetStoreId.data(), ECSSPacketStoreIdSize);
	return packetStoreId.data();
}

void StorageAndRetrievalService::deleteContentUntil(const String<ECSSPacketStoreIdSize>& packetStoreId,
                                                    Time::DefaultCUC timeLimit) {
	auto& telemetryPackets = packetStores[packetStoreId].storedTelemetryPackets;
	while (not telemetryPackets.empty() and telemetryPackets.front().first <= timeLimit) {
		telemetryPackets.pop_front();
	}
}

void StorageAndRetrievalService::copyFromTagToTag(Message& request) {
	const Time::DefaultCUC startTime(request.read<Time::DefaultCUC>());
	const Time::DefaultCUC endTime(request.read<Time::DefaultCUC>());

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
	const Time::DefaultCUC startTime(request.read<Time::DefaultCUC>());

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
	const Time::DefaultCUC endTime(request.read<Time::DefaultCUC>());

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
                                                   const Message& request) {
	if (packetStores.find(fromPacketStoreId) == packetStores.end() or
	    packetStores.find(toPacketStoreId) == packetStores.end()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
		return false;
	}
	return true;
}

bool StorageAndRetrievalService::checkTimeWindow(Time::DefaultCUC startTime, Time::DefaultCUC endTime, const Message& request) {
	if (startTime >= endTime) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::checkDestinationPacketStore(const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                             const Message& request) {
	if (not packetStores[toPacketStoreId].storedTelemetryPackets.empty()) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::DestinationPacketStoreNotEmtpy);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                         Time::DefaultCUC startTime, Time::DefaultCUC endTime, const Message& request) {
	if (endTime < packetStores[fromPacketStoreId].storedTelemetryPackets.front().first ||
	    startTime > packetStores[fromPacketStoreId].storedTelemetryPackets.back().first) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::noTimestampInTimeWindow(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                         Time::DefaultCUC timeTag, const Message& request, bool isAfterTimeTag) {
	if (isAfterTimeTag) {
		if (timeTag > packetStores[fromPacketStoreId].storedTelemetryPackets.back().first) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
			return true;
		}
	} else if (timeTag < packetStores[fromPacketStoreId].storedTelemetryPackets.front().first) {
		ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::CopyOfPacketsFailed);
		return true;
	}
	return false;
}

bool StorageAndRetrievalService::failedFromTagToTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                    const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                    Time::DefaultCUC startTime, Time::DefaultCUC endTime, const Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkTimeWindow(startTime, endTime, request) or checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, startTime, endTime, request));
}

bool StorageAndRetrievalService::failedAfterTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                    const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                    Time::DefaultCUC startTime, const Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, startTime, request, true));
}

bool StorageAndRetrievalService::failedBeforeTimeTag(const String<ECSSPacketStoreIdSize>& fromPacketStoreId,
                                                     const String<ECSSPacketStoreIdSize>& toPacketStoreId,
                                                     Time::DefaultCUC endTime, const Message& request) {
	return (not checkPacketStores(fromPacketStoreId, toPacketStoreId, request) or
	        checkDestinationPacketStore(toPacketStoreId, request) or
	        noTimestampInTimeWindow(fromPacketStoreId, endTime, request, false));
}

void StorageAndRetrievalService::createContentSummary(Message& report,
                                                      const String<ECSSPacketStoreIdSize>& packetStoreId) {
	const Time::DefaultCUC oldestStoredPacketTime(packetStores[packetStoreId].storedTelemetryPackets.front().first);
	report.append<Time::DefaultCUC>(oldestStoredPacketTime);

	const Time::DefaultCUC newestStoredPacketTime(packetStores[packetStoreId].storedTelemetryPackets.back().first);
	report.append<Time::DefaultCUC>(newestStoredPacketTime);

	report.append<Time::DefaultCUC>(packetStores[packetStoreId].openRetrievalStartTimeTag);

	auto filledPercentage1 = static_cast<uint16_t>(static_cast<float>(packetStores[packetStoreId].storedTelemetryPackets.size()) * 100 / // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	                                               ECSSMaxPacketStoreSize);
	report.append<PercentageFilled>(filledPercentage1);

	const uint16_t numOfPacketsToBeTransferred = std::count_if( // NOLINT(cppcoreguidelines-init-variables)
	    std::begin(packetStores[packetStoreId].storedTelemetryPackets),
	    std::end(packetStores[packetStoreId].storedTelemetryPackets), [this, &packetStoreId](auto packet) {
		    return packet.first >= packetStores[packetStoreId].openRetrievalStartTimeTag;
	    });
	auto filledPercentage2 = static_cast<uint16_t>(static_cast<float>(numOfPacketsToBeTransferred) * 100 / ECSSMaxPacketStoreSize); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	report.append<PercentageFilled>(filledPercentage2);
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
		uint16_t const numberOfBytesToSkip = 8;
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
                                                           Time::DefaultCUC timestamp) {
	Message tmPacket; // NOLINT(misc-const-correctness) as we might need to change
	packetStores[packetStoreId].storedTelemetryPackets.push_back({timestamp, tmPacket});
}

void StorageAndRetrievalService::resetPacketStores() {
	packetStores.clear();
}

NumOfPacketStores StorageAndRetrievalService::currentNumberOfPacketStores() {
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
	const NumOfPacketStores numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			function(packetStore.second);
		}
		return;
	}

	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();

	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (failedStartOfByTimeRangeRetrieval(packetStoreId, request)) {
			continue;
		}
		const Time::DefaultCUC retrievalStartTime(request.read<Time::DefaultCUC>());
		const Time::DefaultCUC retrievalEndTime(request.read<Time::DefaultCUC>());

		if (retrievalStartTime >= retrievalEndTime) { //cppcheck-suppress knownConditionTrueFalse
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::InvalidTimeWindow);
			continue;
		}

		// todo (#261): 6.15.3.5.2.d(4), actually count the current time

		auto& packetStore = packetStores[packetStoreId];
		packetStore.byTimeRangeRetrievalStatus = true;
		packetStore.retrievalStartTime = retrievalStartTime;
		packetStore.retrievalEndTime = retrievalEndTime;
		// todo (#262): start the by-time-range retrieval process according to the priority policy
	}
}

void StorageAndRetrievalService::deletePacketStoreContent(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::DeletePacketStoreContent)) {
		return;
	}

	const Time::DefaultCUC timeLimit = request.read<Time::DefaultCUC>();
	const NumOfPacketStores numOfPacketStores = request.readUint16();

	if (numOfPacketStores == 0) {
		for (const auto& packetStore: packetStores) {
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
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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
	NumOfPacketStores numOfPacketStores = request.readUint16();

	if (numOfPacketStores == 0) {
		report.appendUint16(packetStores.size());
		for (const auto& packetStore: packetStores) {
			auto packetStoreId = packetStore.first;
			report.appendString(packetStoreId);
			createContentSummary(report, packetStoreId);
		}
		storeMessage(report);
		return;
	}
	NumOfPacketStores numOfValidPacketStores = 0;
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) != packetStores.end()) {
			numOfValidPacketStores++;
		}
	}
	report.appendUint16(numOfValidPacketStores);
	request.resetRead();
	numOfPacketStores = request.readUint16();

	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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

	const Time::DefaultCUC newStartTimeTag = request.read<Time::DefaultCUC>();
	/**
	 * @todo (#263): check if newStartTimeTag is in the future
	 */
	const NumOfPacketStores numOfPacketStores = request.readUint16();
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

	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
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
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			packetStore.second.openRetrievalStatus = PacketStore::Suspended;
		}
		return;
	}
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		for (auto& packetStore: packetStores) {
			packetStore.second.byTimeRangeRetrievalStatus = false;
		}
		return;
	}
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		if (packetStores.find(packetStoreId) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		packetStores[packetStoreId].byTimeRangeRetrievalStatus = false;
	}
}

void StorageAndRetrievalService::packetStoresStatusReport(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportStatusOfPacketStores)) {
		return;
	}

	Message report = createTM(PacketStoresStatusReport);
	report.appendUint16(packetStores.size());
	for (const auto& packetStore: packetStores) {
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		if (packetStores.size() >= ECSSMaxPacketStores) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::MaxNumberOfPacketStoresReached);
			return;
		}
		auto idToCreate = readPacketStoreId(request);

		if (packetStores.find(idToCreate) != packetStores.end()) {
			uint16_t const numberOfBytesToSkip = 4;
			request.skipBytes(numberOfBytesToSkip);
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::AlreadyExistingPacketStore);
			continue;
		}
		const PacketStoreSize packetStoreSize = request.read<PacketStoreSize>();
		const PacketStoreType typeCode = request.read<PacketStoreType>();
		const PacketStore::PacketStoreType packetStoreType = (typeCode == 0) ? PacketStore::Circular : PacketStore::Bounded;
		const VirtualChannel virtualChannel = request.read<VirtualChannel>();

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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
	if (numOfPacketStores == 0) {
		NumOfPacketStores numOfPacketStoresToDelete = 0; // NOLINT(misc-const-correctness)
		etl::array<etl::string<ECSSPacketStoreIdSize>, ECSSMaxPacketStores> packetStoresToDelete = {};
		for (const auto& packetStore: packetStores) {
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
		for (NumOfPacketStores l = 0; l < numOfPacketStoresToDelete; l++) {
			etl::array<uint8_t, ECSSPacketStoreIdSize> data = {};
			etl::string<ECSSPacketStoreIdSize> idToDelete = packetStoresToDelete[l];
			std::copy(idToDelete.begin(), idToDelete.end(), data.data());
			String<ECSSPacketStoreIdSize> const key(data.data());
			packetStores.erase(key);
		}
		return;
	}

	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		auto idToDelete = readPacketStoreId(request);
		if (packetStores.find(idToDelete) == packetStores.end()) {
			ErrorHandler::reportError(request, ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore);
			continue;
		}
		const auto& packetStore = packetStores[idToDelete];

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

void StorageAndRetrievalService::packetStoreConfigurationReport(const Message& request) {
	if (!request.assertTC(ServiceType, MessageType::ReportConfigurationOfPacketStores)) {
		return;
	}
	Message report = createTM(PacketStoreConfigurationReport);

	report.appendUint16(packetStores.size());
	for (const auto& packetStore: packetStores) {
		auto packetStoreId = packetStore.first;
		report.appendString(packetStoreId);
		report.appendUint16(packetStore.second.sizeInBytes);
		const PacketStoreType typeCode = (packetStore.second.packetStoreType == PacketStore::Circular) ? 0 : 1;
		report.append<PacketStoreType>(typeCode);
		report.append<VirtualChannel>(packetStore.second.virtualChannel);
	}
	storeMessage(report);
}

void StorageAndRetrievalService::copyPacketsInTimeWindow(Message& request) {
	if (!request.assertTC(ServiceType, MessageType::CopyPacketsInTimeWindow)) {
		return;
	}

	uint8_t const typeOfTimeWindow = request.readEnum8();
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

	const NumOfPacketStores numOfPacketStores = request.readUint16();
	for (NumOfPacketStores i = 0; i < numOfPacketStores; i++) {
		auto packetStoreId = readPacketStoreId(request);
		const PacketStoreSize packetStoreSize = request.read<PacketStoreSize>(); // In bytes
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
	const VirtualChannel virtualChannel = request.read<VirtualChannel>();
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
