#include "Services/StorageAndRetrievalService.hpp"
#include <iostream>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "catch2/catch_all.hpp"

StorageAndRetrievalService& storageAndRetrieval = Services.storageAndRetrieval;

uint32_t timestamps1[6] = {2, 4, 5, 7, 9, 11};
uint32_t timestamps2[5] = {0, 1, 4, 15, 22};
uint32_t timestamps3[4] = {4, 7, 9, 14};
uint32_t timestamps4[8] = {4, 6, 34, 40, 44, 51, 52, 58};

void initializePacketStores() {
	uint16_t numOfPacketStores = 4;
	uint8_t concatenatedPacketStoreNames[] = "ps2ps25ps799ps5555";
	uint16_t offsets[5] = {0, 3, 7, 12, 18};
	uint16_t sizes[4] = {100, 200, 550, 340};
	uint8_t virtualChannels[4] = {4, 6, 1, 2};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSPacketStoreIdSize];
		std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);

		String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);
		PacketStore newPacketStore;
		newPacketStore.sizeInBytes = sizes[i];
		newPacketStore.packetStoreType = ((i % 2) == 0) ? PacketStore::Circular : PacketStore::Bounded;
		newPacketStore.virtualChannel = virtualChannels[i];
		storageAndRetrieval.addPacketStore(packetStoreId, newPacketStore);
	}
}

void validPacketStoreCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 4;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps2ps25ps799ps5555";
	uint16_t offsets[5] = {0, 3, 7, 12, 18};
	uint16_t sizes[4] = {100, 200, 550, 340};
	uint8_t virtualChannels[4] = {4, 6, 1, 2};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSPacketStoreIdSize];
		std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);

		String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);
		request.appendString(packetStoreId);
		request.appendUint16(sizes[i]);
		if ((i % 2) == 0) {
			request.appendUint8(packetStoreTypeCode[0]);
		} else {
			request.appendUint8(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

void invalidPacketStoreCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 5;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps2ps1ps2ps44ps0000";
	uint16_t offsets[6] = {0, 3, 6, 9, 13, 19};
	uint16_t sizes[5] = {33, 55, 66, 77, 99};
	uint8_t invalidChannel1 = VirtualChannelLimits.min - 1;
	uint8_t invalidChannel2 = VirtualChannelLimits.max + 1;
	uint8_t virtualChannels[5] = {5, invalidChannel1, 1, invalidChannel2, 2};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSPacketStoreIdSize];
		std::fill(std::begin(packetStoreData), std::end(packetStoreData), 0);
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);

		String<ECSSPacketStoreIdSize> packetStoreId(packetStoreData);
		request.appendString(packetStoreId);
		request.appendUint16(sizes[i]);
		if ((i % 2) == 0) {
			request.appendUint8(packetStoreTypeCode[0]);
		} else {
			request.appendUint8(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

etl::array<String<ECSSPacketStoreIdSize>, 4> validPacketStoreIds() {
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
	uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps25";
	uint8_t packetStoreData3[ECSSPacketStoreIdSize] = "ps799";
	uint8_t packetStoreData4[ECSSPacketStoreIdSize] = "ps5555";

	String<ECSSPacketStoreIdSize> id(packetStoreData);
	String<ECSSPacketStoreIdSize> id2(packetStoreData2);
	String<ECSSPacketStoreIdSize> id3(packetStoreData3);
	String<ECSSPacketStoreIdSize> id4(packetStoreData4);

	etl::array<String<ECSSPacketStoreIdSize>, 4> validPacketStores = {id, id2, id3, id4};
	return validPacketStores;
}

etl::array<String<ECSSPacketStoreIdSize>, 4> invalidPacketStoreIds() {
	uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps1";
	uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps36";
	uint8_t packetStoreData3[ECSSPacketStoreIdSize] = "ps999";
	uint8_t packetStoreData4[ECSSPacketStoreIdSize] = "ps1234";

	String<ECSSPacketStoreIdSize> id(packetStoreData);
	String<ECSSPacketStoreIdSize> id2(packetStoreData2);
	String<ECSSPacketStoreIdSize> id3(packetStoreData3);
	String<ECSSPacketStoreIdSize> id4(packetStoreData4);

	etl::array<String<ECSSPacketStoreIdSize>, 4> validPacketStores = {id, id2, id3, id4};
	return validPacketStores;
}

void padWithZeros(etl::array<String<ECSSPacketStoreIdSize>, 4>& packetStoreIds) {
	uint8_t offsets[] = {3, 4, 5, 6};
	int index = 0;
	// Padding every empty position with zeros, to avoid memory garbage collection, which leads to a faulty result.
	for (auto& packetStoreId: packetStoreIds) {
		uint8_t startingPosition = offsets[index++];
		for (uint8_t i = startingPosition; i < ECSSPacketStoreIdSize; i++) {
			packetStoreId[i] = 0;
		}
	}
}

void addTelemetryPacketsInPacketStores() {
	auto packetStoreIds = validPacketStoreIds();

	for (auto& timestamp: timestamps1) {
		storageAndRetrieval.addTelemetryToPacketStore(packetStoreIds[0], timestamp);
	}
	for (auto& timestamp: timestamps2) {
		storageAndRetrieval.addTelemetryToPacketStore(packetStoreIds[1], timestamp);
	}
	for (auto& timestamp: timestamps3) {
		storageAndRetrieval.addTelemetryToPacketStore(packetStoreIds[2], timestamp);
	}
	for (auto& timestamp: timestamps4) {
		storageAndRetrieval.addTelemetryToPacketStore(packetStoreIds[3], timestamp);
	}
}

void resetPacketStores() {
	storageAndRetrieval.resetPacketStores();
}

TEST_CASE("Creating packet stores") {
	SECTION("Valid packet store creation request") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		validPacketStoreCreationRequest(request);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 0);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);

		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[0]).sizeInBytes == 100);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[1]).sizeInBytes == 200);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[2]).sizeInBytes == 550);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[3]).sizeInBytes == 340);

		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[0]).virtualChannel == 4);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[1]).virtualChannel == 6);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[2]).virtualChannel == 1);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[3]).virtualChannel == 2);

		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[0]).packetStoreType ==
		      PacketStore::PacketStoreType::Circular);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[1]).packetStoreType ==
		      PacketStore::PacketStoreType::Bounded);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[2]).packetStoreType ==
		      PacketStore::PacketStoreType::Circular);
		CHECK(storageAndRetrieval.getPacketStore(packetStoreIds[3]).packetStoreType ==
		      PacketStore::PacketStoreType::Bounded);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid packet store creation request") {
		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
		String<ECSSPacketStoreIdSize> existingPacketStoreId(packetStoreData);
		PacketStore existingPacketStore;
		storageAndRetrieval.addPacketStore(existingPacketStoreId, existingPacketStore);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 1);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		invalidPacketStoreCreationRequest(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidVirtualChannel) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AlreadyExistingPacketStore) == 2);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Exceeding the maximum number of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		invalidPacketStoreCreationRequest(request);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxNumberOfPacketStoresReached) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Deleting packet stores") {
	SECTION("Valid deletion of specified packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid deletion of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion of specified packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		storageAndRetrieval.getPacketStore(packetStoreIds[0]).storageStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[1]).byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).storageStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithOpenRetrievalInProgress) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion request of non existing packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = invalidPacketStoreIds();

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 4);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid deletion of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
		}

		storageAndRetrieval.getPacketStore(packetStoreIds[0]).storageStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[1]).byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithOpenRetrievalInProgress) == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both valid and invalid deletion requests") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 8;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: correctPacketStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		for (auto& packetStoreId: wrongPacketStoreIds) {
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).storageStatus = true;
		storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).byTimeRangeRetrievalStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 2);
		REQUIRE(storageAndRetrieval.packetStoreExists(correctPacketStoreIds[0]));
		REQUIRE(storageAndRetrieval.packetStoreExists(correctPacketStoreIds[1]));
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 4);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Enabling the storage of packet stores") {
	SECTION("Valid enabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.packetStoreExists(packetStoreIds[i]));
			storageAndRetrieval.getPacketStore(packetStoreIds[i]).storageStatus = false;
			request.appendString(packetStoreIds[i]);
		}
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storageStatus = false;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).storageStatus = false;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storageStatus == true);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storageStatus == true);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storageStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storageStatus == false);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid enabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = invalidPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreIds[i]));
			request.appendString(packetStoreIds[i]);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enabling the storage of all packet stores") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).storageStatus == true);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disabling the storage of packet stores") {
	SECTION("Valid disabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			storageAndRetrieval.getPacketStore(packetStoreIds[i]).storageStatus = true;
			request.appendString(packetStoreIds[i]);
		}
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storageStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).storageStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storageStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storageStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storageStatus == true);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storageStatus == true);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid disabling of storage") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = invalidPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreIds[i]));
			request.appendString(packetStoreIds[i]);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enabling the storage of all packet stores") {
		initializePacketStores();
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).storageStatus == false);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the open retrieval start-time-tag") {
	SECTION("Successful change of the start-time-tag") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 2;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStartTimeTag == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed change of the start-time-tag") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 6;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStartTimeTag == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::InProgress;
			request.appendString(packetStoreId);
		}

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 3);

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both successful and failed attempts to change the start-time-tag of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);

		uint32_t startTimeTag = 200;
		uint16_t numOfPacketStores = 0;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStartTimeTag == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
		}
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus = PacketStore::InProgress;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 2);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStartTimeTag == 200);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStartTimeTag == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStartTimeTag == 0);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Resuming the open retrieval process") {
	SECTION("Successful resuming of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed resuming of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = true;
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			request.appendString(packetStoreId);
		}
		storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).openRetrievalStatus = PacketStore::Suspended;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 3);

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).openRetrievalStatus ==
		        PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both successful and failed attempts to resume the open retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
		}
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).byTimeRangeRetrievalStatus = true;
		storageAndRetrieval.getPacketStore(packetStoreIds[3]).byTimeRangeRetrievalStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 2);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStatus == PacketStore::InProgress);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Suspending the open retrieval process") {
	SECTION("Successful suspension of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::InProgress;
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus == PacketStore::InProgress);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed suspension of the open retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::InProgress;
			request.appendString(packetStoreId);
		}
		storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).openRetrievalStatus = PacketStore::InProgress;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).openRetrievalStatus ==
		        PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).openRetrievalStatus ==
		        PacketStore::InProgress);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful attempt to suspend the open retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.packetStoreExists(packetStoreId));
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::InProgress;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).openRetrievalStatus == PacketStore::Suspended);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).openRetrievalStatus == PacketStore::Suspended);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Starting the by-time-range retrieval of packet stores") {
	SECTION("Successful starting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		uint32_t timeTags1[4] = {20, 30, 40, 50};
		uint32_t timeTags2[4] = {60, 70, 80, 90};

		int index = 0;
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			request.appendString(packetStoreId);
			uint32_t timeTag1 = timeTags1[index];
			uint32_t timeTag2 = timeTags2[index++];
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreIds[i]);
			REQUIRE(packetStore.byTimeRangeRetrievalStatus == true);
			REQUIRE(packetStore.retrievalStartTime == timeTags1[i]);
			REQUIRE(packetStore.retrievalEndTime == timeTags2[i]);
		}
		auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreIds[3]);
		REQUIRE(packetStore.byTimeRangeRetrievalStatus == false);
		REQUIRE(packetStore.retrievalStartTime == 0);
		REQUIRE(packetStore.retrievalEndTime == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed starting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = correctPacketStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus =
			    (i % 2 == 0) ? PacketStore::Suspended : PacketStore::InProgress;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = i % 2 == 0;

			request.appendString(packetStoreId);
			uint32_t timeTag1 = 20;
			uint32_t timeTag2 = 40;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}
		storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).byTimeRangeRetrievalStatus = false;

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
			uint32_t timeTag1 = 20;
			uint32_t timeTag2 = 40;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ByTimeRangeRetrievalAlreadyEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 1);

		for (int i = 0; i < numOfPacketStores / 2; i++) {
			auto& packetStore = storageAndRetrieval.getPacketStore(correctPacketStoreIds[i]);
			REQUIRE(packetStore.byTimeRangeRetrievalStatus == (i % 2 == 0));
			REQUIRE(packetStore.retrievalStartTime == 0);
			REQUIRE(packetStore.retrievalEndTime == 0);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).retrievalStartTime == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid window requested") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			request.appendString(packetStoreId);
			uint32_t timeTag1 = 90;
			uint32_t timeTag2 = 20;
			request.appendUint32(timeTag1);
			request.appendUint32(timeTag2);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 3);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).byTimeRangeRetrievalStatus == false);

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).retrievalStartTime == 0);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Aborting the by-time-range retrieval of packet stores") {
	SECTION("Successful aborting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = true;
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[i]).byTimeRangeRetrievalStatus == false);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).byTimeRangeRetrievalStatus == true);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed aborting of the by-time-range retrieval") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		auto correctPacketStoreIds = validPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: correctPacketStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = true;
		}

		for (int i = 0; i < numOfPacketStores; i++) {
			auto packetStoreId = wrongPacketStoreIds[i];
			REQUIRE(not storageAndRetrieval.packetStoreExists(packetStoreId));
			request.appendString(packetStoreId);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);
		for (auto& packetStoreId: correctPacketStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = true;
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful abort of the by-time-range retrieval of all packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = true;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).byTimeRangeRetrievalStatus == false);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).byTimeRangeRetrievalStatus == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the status of packet stores") {
	SECTION("Valid reporting of the packet store status") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSPacketStoreIdSize] = "ps5555";

		int count = 0;
		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = (count % 2 == 0);
			packetStore.byTimeRangeRetrievalStatus = (count % 2 != 0);
			packetStore.openRetrievalStatus = (count % 2 == 0) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportStatusOfPacketStores, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoresStatusReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSPacketStoreIdSize];
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData)));
		CHECK(report.readBoolean() == true);
		CHECK(report.readEnum8() == 1);
		CHECK(report.readBoolean() == false);
		// Packet store 2
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData2)));
		CHECK(report.readBoolean() == false);
		CHECK(report.readEnum8() == 0);
		CHECK(report.readBoolean() == true);
		// Packet store 3
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData4)));
		CHECK(report.readBoolean() == false);
		CHECK(report.readEnum8() == 0);
		CHECK(report.readBoolean() == true);
		// Packet store 4
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData3)));
		CHECK(report.readBoolean() == true);
		CHECK(report.readEnum8() == 1);
		CHECK(report.readBoolean() == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the configuration of packet stores") {
	SECTION("Valid reporting of the configuration") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSPacketStoreIdSize] = "ps5555";

		int count = 0;
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).packetStoreType =
			    (count % 2 == 0) ? PacketStore::Circular : PacketStore::Bounded;
			count++;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportConfigurationOfPacketStores, Message::TC, 1);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSPacketStoreIdSize];
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData)));
		CHECK(report.readUint16() == 100);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint8() == 4);
		// Packet store 2
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData2)));
		CHECK(report.readUint16() == 200);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint8() == 6);
		// Packet store 3
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData4)));
		CHECK(report.readUint16() == 340);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint8() == 2);
		// Packet store 4
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(data), std::end(data), std::begin(packetStoreData3)));
		CHECK(report.readUint16() == 550);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint8() == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Resizing the packet stores") {
	SECTION("Successful resizing of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t newSizes[4] = {11, 22, 33, 44};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		int index = 0;
		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
			packetStore.byTimeRangeRetrievalStatus = false;

			request.appendString(packetStoreId);
			request.appendUint16(newSizes[index]);
			index++;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		for (int i = 0; i < numOfPacketStores; i++) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[i]).sizeInBytes == newSizes[i]);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).sizeInBytes == 340);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed resizing of packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t oldSizes[4] = {100, 200, 550, 340};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);
		int index = 0;
		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = (index % 2 == 0);
			packetStore.byTimeRangeRetrievalStatus = (index == 1);
			packetStore.openRetrievalStatus = (index == 3) ? PacketStore::InProgress : PacketStore::Suspended;

			request.appendString(packetStoreId);
			request.appendUint16(35);
			index++;
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithStorageStatusEnabled) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 1);
		int i = 0;

		for (auto& packetStoreId: packetStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).sizeInBytes == oldSizes[i++]);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Memory unable to handle the requested size") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint16_t newSizes[4] = {1000, 2000, 3400, 5500};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);
		int index = 0;
		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;

			request.appendString(packetStoreId);
			request.appendUint16(newSizes[index++]);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::UnableToHandlePacketStoreSize) == 4);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request to resize non existing packet stores") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		auto correctPacketStoreIds = validPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		uint16_t oldSizes[4] = {100, 200, 550, 340};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);
		for (auto& packetStoreId: wrongPacketStoreIds) {
			request.appendString(packetStoreId);
			request.appendUint16(35);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 4);
		int i = 0;
		for (auto& packetStoreId: correctPacketStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).sizeInBytes == oldSizes[i++]);
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the packet store type to circular") {
	SECTION("Successful changing of type to circular") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		for (auto& packetStoreId: packetStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.packetStoreType = PacketStore::Bounded;
			packetStore.storageStatus = false;
			packetStore.byTimeRangeRetrievalStatus = false;
			packetStore.openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

		request.appendString(packetStoreIds[0]);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).packetStoreType == PacketStore::Bounded);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

		request2.appendString(packetStoreIds[3]);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).packetStoreType == PacketStore::Circular);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed changing of type to circular") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		int count = 0;
		for (auto& packetStoreId: correctPacketStoreIds) {
			auto& packetStore = storageAndRetrieval.getPacketStore(packetStoreId);
			packetStore.packetStoreType = PacketStore::Bounded;
			packetStore.storageStatus = (count == 0);
			packetStore.byTimeRangeRetrievalStatus = (count == 1);
			packetStore.openRetrievalStatus = (count == 2) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                             correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

			request.appendString(finalIds[i]);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).packetStoreType == PacketStore::Bounded);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the packet store type to bounded") {
	SECTION("Successful changing of type to bounded") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).packetStoreType = PacketStore::Circular;
			storageAndRetrieval.getPacketStore(packetStoreId).storageStatus = false;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

		request.appendString(packetStoreIds[0]);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).packetStoreType == PacketStore::Circular);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

		request2.appendString(packetStoreIds[3]);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).packetStoreType == PacketStore::Bounded);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).packetStoreType == PacketStore::Bounded);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed changing of type to bounded") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		int count = 0;
		for (auto& packetStoreId: correctPacketStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).packetStoreType = PacketStore::Circular;
			storageAndRetrieval.getPacketStore(packetStoreId).storageStatus = (count == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = (count == 1);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus =
			    (count == 2) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                             correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithStorageStatusEnabled,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

			request.appendString(finalIds[i]);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).packetStoreType == PacketStore::Circular);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).packetStoreType == PacketStore::Circular);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Changing the virtual channel of packet stores") {
	SECTION("Successful change of virtual channel") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		uint8_t virtualChannels[2] = {1, 5};

		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
		}

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

		request.appendString(packetStoreIds[0]);
		request.appendUint8(virtualChannels[0]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).virtualChannel == virtualChannels[0]);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).virtualChannel == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).virtualChannel == 1);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).virtualChannel == 2);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

		request2.appendString(packetStoreIds[3]);
		request2.appendUint8(virtualChannels[1]);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 0);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).virtualChannel == virtualChannels[0]);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).virtualChannel == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).virtualChannel == 1);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).virtualChannel == virtualChannels[1]);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed change of virtual channel") {
		initializePacketStores();
		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		uint8_t oldVirtualChannels[4] = {4, 6, 1, 2};

		int count = 0;
		for (auto& packetStoreId: correctPacketStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = (count == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus =
			    (count == 1) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}

		String<ECSSPacketStoreIdSize> finalIds[4] = {wrongPacketStoreIds[0], correctPacketStoreIds[0],
		                                             correctPacketStoreIds[1], correctPacketStoreIds[2]};

		ErrorHandler::ExecutionStartErrorType expectedErrors[4] = {
		    ErrorHandler::ExecutionStartErrorType::NonExistingPacketStore,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithByTimeRangeRetrieval,
		    ErrorHandler::ExecutionStartErrorType::GetPacketStoreWithOpenRetrievalInProgress,
		    ErrorHandler::ExecutionStartErrorType::InvalidVirtualChannel};

		for (int i = 0; i < 4; i++) {
			Message request(StorageAndRetrievalService::ServiceType,
			                StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

			request.appendString(finalIds[i]);
			request.appendUint8(i == 3 ? VirtualChannelLimits.max + 1 : 3);
			MessageParser::execute(request);
			CHECK(ServiceTests::count() == i + 1);
			CHECK(ServiceTests::countThrownErrors(expectedErrors[i]) == 1);
		}

		int index = 0;
		for (auto& packetStoreId: correctPacketStoreIds) {
			REQUIRE(storageAndRetrieval.getPacketStore(packetStoreId).virtualChannel == oldVirtualChannels[index]);
			index++;
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting the content summary of packet stores") {
	SECTION("Successful content summary report of specified packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 2;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			storageAndRetrieval.getPacketStore(packetStoreIds[i]).openRetrievalStartTimeTag = 5;
			request.appendString(packetStoreIds[i]);
		}

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps25";

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 2);

		// Packet store 1
		uint8_t data[ECSSPacketStoreIdSize];
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 20);
		// Packet store 2
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData2), std::end(packetStoreData2), std::begin(data)));
		CHECK(report.readUint32() == timestamps2[0]);
		CHECK(report.readUint32() == timestamps2[4]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 25);
		CHECK(report.readUint16() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful content summary report of all packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		int count = 0;
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStartTimeTag = (count == 3) ? 20 : 15;
			count++;
		}

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData4[ECSSPacketStoreIdSize] = "ps799";

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 0;
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 4);

		// Packet store 1
		uint8_t data[ECSSPacketStoreIdSize];
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 0);
		// Packet store 2
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData2), std::end(packetStoreData2), std::begin(data)));
		CHECK(report.readUint32() == timestamps2[0]);
		CHECK(report.readUint32() == timestamps2[4]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 25);
		CHECK(report.readUint16() == 10);
		// Packet store 3
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData3), std::end(packetStoreData3), std::begin(data)));
		CHECK(report.readUint32() == timestamps4[0]);
		CHECK(report.readUint32() == timestamps4[7]);
		CHECK(report.readUint32() == 20);
		CHECK(report.readUint16() == 40);
		CHECK(report.readUint16() == 30);
		// Packet store 4
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData4), std::end(packetStoreData4), std::begin(data)));
		CHECK(report.readUint32() == timestamps3[0]);
		CHECK(report.readUint32() == timestamps3[3]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 20);
		CHECK(report.readUint16() == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed content summary report of packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		String<ECSSPacketStoreIdSize> finalIds[3] = {wrongPacketStoreIds[0], wrongPacketStoreIds[1],
		                                             correctPacketStoreIds[0]};

		storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).openRetrievalStartTimeTag = 5;

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);

		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);
		for (int i = 0; i < numOfPacketStores; i++) {
			request.appendString(finalIds[i]);
		}

		uint8_t packetStoreData[ECSSPacketStoreIdSize] = "ps2";

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 2);

		Message report = ServiceTests::get(2);
		REQUIRE(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport);
		REQUIRE(report.readUint16() == 1);

		// Packet store 1
		uint8_t data[ECSSPacketStoreIdSize];
		report.readString(data, ECSSPacketStoreIdSize);
		CHECK(std::equal(std::begin(packetStoreData), std::end(packetStoreData), std::begin(data)));
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 5);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 20);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Deleting packet store content") {
	SECTION("Successful deletion of content, specified time-tag in the middle of the packets stored") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);

		uint32_t storageTime = 5;
		uint16_t numOfPacketStores = 2;
		request.appendUint32(storageTime);
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < numOfPacketStores; i++) {
			auto packetStoreId = packetStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			request.appendString(packetStoreId);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets.size() == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets.size() == 5);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		uint32_t expectedTimeStamps1[3] = {7, 9, 11};
		uint32_t expectedTimeStamps2[2] = {15, 22};
		uint32_t leftTimeStamps1[3];
		uint32_t leftTimeStamps2[2];

		int count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets) {
			leftTimeStamps1[count++] = tmPacket.first;
		}
		count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets) {
			leftTimeStamps2[count++] = tmPacket.first;
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets.size() == 3);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets.size() == 2);
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps1), std::end(expectedTimeStamps1), std::begin(leftTimeStamps1)));
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps2), std::end(expectedTimeStamps2), std::begin(leftTimeStamps2)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful deletion of content, specified time-tag is smaller than the min stored timestamp") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);

		uint32_t storageTime = 3;
		uint16_t numOfPacketStores = 2;
		request.appendUint32(storageTime);
		request.appendUint16(numOfPacketStores);

		for (int i = 2; i < numOfPacketStores + 2; i++) {
			auto packetStoreId = packetStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			request.appendString(packetStoreId);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.size() == 8);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		uint32_t expectedTimeStamps1[4] = {4, 7, 9, 14};
		uint32_t expectedTimeStamps2[8] = {4, 6, 34, 40, 44, 51, 52, 58};
		uint32_t leftTimeStamps1[4];
		uint32_t leftTimeStamps2[8];

		int count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets) {
			leftTimeStamps1[count++] = tmPacket.first;
		}
		count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets) {
			leftTimeStamps2[count++] = tmPacket.first;
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.size() == 8);
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps1), std::end(expectedTimeStamps1), std::begin(leftTimeStamps1)));
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps2), std::end(expectedTimeStamps2), std::begin(leftTimeStamps2)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Successful deletion of content, specified time-tag is larger than the max stored timestamp") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);

		uint32_t storageTime = 59;
		uint16_t numOfPacketStores = 2;
		request.appendUint32(storageTime);
		request.appendUint16(numOfPacketStores);

		for (int i = 2; i < numOfPacketStores + 2; i++) {
			auto packetStoreId = packetStoreIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus = PacketStore::Suspended;
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = false;
			request.appendString(packetStoreId);
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.size() == 8);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both successful and failed deletion of content for all packet stores") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);

		uint32_t storageTime = 15;
		uint16_t numOfPacketStores = 0;
		request.appendUint32(storageTime);
		request.appendUint16(numOfPacketStores);

		int count = 0;
		for (auto& packetStoreId: packetStoreIds) {
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = (count == 0);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus =
			    (count == 1) ? PacketStore::InProgress : PacketStore::Suspended;
			count++;
		}
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets.size() == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets.size() == 5);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.size() == 8);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 1);

		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets.size() == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets.size() == 5);
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets.size() == 6);

		uint32_t expectedTimeStamps1[6] = {2, 4, 5, 7, 9, 11};
		uint32_t expectedTimeStamps2[5] = {0, 1, 4, 15, 22};
		uint32_t expectedTimeStamps4[6] = {34, 40, 44, 51, 52, 58};

		uint32_t leftTimeStamps1[6];
		uint32_t leftTimeStamps2[5];
		uint32_t leftTimeStamps4[6];

		count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[0]).storedTelemetryPackets) {
			leftTimeStamps1[count++] = tmPacket.first;
		}
		count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[1]).storedTelemetryPackets) {
			leftTimeStamps2[count++] = tmPacket.first;
		}
		count = 0;
		for (auto& tmPacket: storageAndRetrieval.getPacketStore(packetStoreIds[3]).storedTelemetryPackets) {
			leftTimeStamps4[count++] = tmPacket.first;
		}

		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps1), std::end(expectedTimeStamps1), std::begin(leftTimeStamps1)));
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps2), std::end(expectedTimeStamps2), std::begin(leftTimeStamps2)));
		REQUIRE(
		    std::equal(std::begin(expectedTimeStamps4), std::end(expectedTimeStamps4), std::begin(leftTimeStamps4)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Failed deletion of content") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		String<ECSSPacketStoreIdSize> finalIds[7] = {
		    wrongPacketStoreIds[0], wrongPacketStoreIds[1], wrongPacketStoreIds[2], correctPacketStoreIds[0],
		    correctPacketStoreIds[1], correctPacketStoreIds[2], correctPacketStoreIds[3]};

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);

		uint32_t storageTime = 59;
		uint16_t numOfPacketStores = 7;
		request.appendUint32(storageTime);
		request.appendUint16(numOfPacketStores);

		for (int i = 0; i < 3; i++) {
			auto packetStoreId = finalIds[i];
			request.appendString(packetStoreId);
		}

		for (int i = 3; i < 7; i++) {
			auto packetStoreId = finalIds[i];
			storageAndRetrieval.getPacketStore(packetStoreId).byTimeRangeRetrievalStatus = (i == 4 || i == 6);
			storageAndRetrieval.getPacketStore(packetStoreId).openRetrievalStatus =
			    (i == 3 || i == 5) ? PacketStore::InProgress : PacketStore::Suspended;
			request.appendString(packetStoreId);
		}

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).storedTelemetryPackets.size() == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).storedTelemetryPackets.size() == 5);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).storedTelemetryPackets.size() == 8);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 2);

		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[0]).storedTelemetryPackets.size() == 6);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[1]).storedTelemetryPackets.size() == 5);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).storedTelemetryPackets.size() == 4);
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[3]).storedTelemetryPackets.size() == 8);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Copying packets in time window, from tag to tag") {
	SECTION("Both time-tags earlier than the earliest time-tag") {
		/**
		 * CASE 0:
		 *
		 * 	(tag1)-------(tag2)-------(earliest packet timestamp)-----(..more packets..)-----(latest packet timestamp)
		 * 				              left-most packet in deque	     tag2 somewhere inside   right-most packet in deque
		 */
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 0;
		uint32_t timeTag2 = 1;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag1 earlier than the earliest stored time-tag, and time-tag2 in between the existing ones") {
		/**
		 * CASE 1:
		 *
		 * 	(tag1)--------(earliest packet timestamp)-----(..more packets..)-----(tag2)--------(latest packet timestamp)
		 * 				left-most packet in deque				tag2 somewhere inside		right-most packet in deque
		 */
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 0;
		uint32_t timeTag2 = 4;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 2);
		int index = 0;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			REQUIRE(tmPacket.first == timestamps1[index++]);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both time-tags in between the stored timestamps") {
		/**
		 * CASE 2:
		 *
		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(tag2)--------(latest packet timestamp)
		 * 	left-most packet in deque						both tag1 and tag2 inside 		right-most packet in deque
		 */
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 35;
		uint32_t timeTag2 = 52;
		auto fromPacketStoreId = packetStoreIds[3];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 4);
		int index = 3;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			REQUIRE(tmPacket.first == timestamps4[index++]);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag1 in between the stored timestamps and tag2 larger than the max timestamp") {
		/**
		 * CASE 3:
		 *
		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(latest packet timestamp)--------(tag2)
		 * 	left-most packet in deque		tag1 inside						right-most packet in deque
		 */
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 3;
		uint32_t timeTag2 = 27;
		auto fromPacketStoreId = packetStoreIds[1];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 3);
		int index = 2;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			REQUIRE(tmPacket.first == timestamps2[index++]);
		}

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Both time-tags larger than largest stored time-tag") {
		/**
		 * CASE 4:
		 *
		 * 	(earliest packet timestamp)-----(..more packets..)-----(latest packet timestamp)-----(tag1)-------(tag2)
		 *   left-most packet in deque	     tag2 somewhere inside   right-most packet in deque
		 */
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 12;
		uint32_t timeTag2 = 14;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid packet store requested") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto correctPacketStoreIds = validPacketStoreIds();
		auto wrongPacketStoreIds = invalidPacketStoreIds();
		padWithZeros(correctPacketStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(correctPacketStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 3;
		uint32_t timeTag2 = 27;
		auto fromPacketStoreId = wrongPacketStoreIds[0];
		auto toPacketStoreId = correctPacketStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::NonExistingPacketStore) == 1);
		REQUIRE(storageAndRetrieval.getPacketStore(toPacketStoreId).storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid time window requested") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 26;
		uint32_t timeTag2 = 17;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 1);
		REQUIRE(storageAndRetrieval.getPacketStore(toPacketStoreId).storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Destination packet not empty") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		REQUIRE(not storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 3;
		uint32_t timeTag2 = 7;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DestinationPacketStoreNotEmtpy) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("No packets contained into the requested time-window") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::FromTagToTag;
		uint32_t timeTag1 = 0;
		uint32_t timeTag2 = 3;
		auto fromPacketStoreId = packetStoreIds[3];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		CHECK(storageAndRetrieval.getPacketStore(toPacketStoreId).storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Copying packets in time window, after time-tag") {
	SECTION("Time-tag in between the stored time-tags") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::AfterTimeTag;
		uint32_t timeTag1 = 6;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 3);
		uint32_t expectedTimestamps[3] = {7, 9, 11};
		uint32_t existingTimestamps[3];

		int index = 0;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			existingTimestamps[index++] = tmPacket.first;
		}
		REQUIRE(
		    std::equal(std::begin(expectedTimestamps), std::end(expectedTimestamps), std::begin(existingTimestamps)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag earlier than the earliest stored time-tag") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::AfterTimeTag;
		uint32_t timeTag1 = 1;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 6);
		uint32_t existingTimestamps[6];

		int index = 0;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			existingTimestamps[index++] = tmPacket.first;
		}
		REQUIRE(std::equal(std::begin(timestamps1), std::end(timestamps1), std::begin(existingTimestamps)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag larger than the largest stored time-tag") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::AfterTimeTag;
		uint32_t timeTag1 = 25;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag1);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Copying packets in time window, before time-tag") {
	SECTION("Time-tag in between the stored time-tags") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::BeforeTimeTag;
		uint32_t timeTag2 = 6;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 3);
		uint32_t expectedTimestamps[3] = {2, 4, 5};
		uint32_t existingTimestamps[3];

		int index = 0;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			existingTimestamps[index++] = tmPacket.first;
		}
		REQUIRE(
		    std::equal(std::begin(expectedTimestamps), std::end(expectedTimestamps), std::begin(existingTimestamps)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag larger than the largest stored time-tag") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::BeforeTimeTag;
		uint32_t timeTag2 = 56;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 0);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.size() == 6);
		uint32_t existingTimestamps[6];

		int index = 0;
		for (auto& tmPacket: targetPacketStore.storedTelemetryPackets) {
			existingTimestamps[index++] = tmPacket.first;
		}
		REQUIRE(std::equal(std::begin(timestamps1), std::end(timestamps1), std::begin(existingTimestamps)));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Time-tag earlier than the earliest stored time-tag") {
		initializePacketStores();
		addTelemetryPacketsInPacketStores();

		REQUIRE(storageAndRetrieval.currentNumberOfPacketStores() == 4);
		auto packetStoreIds = validPacketStoreIds();
		padWithZeros(packetStoreIds);

		// Empty the target packet store, so the copy can occur
		storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.clear();
		REQUIRE(storageAndRetrieval.getPacketStore(packetStoreIds[2]).storedTelemetryPackets.empty());

		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);

		uint8_t typeOfTimeWindow = StorageAndRetrievalService::TimeWindowType::BeforeTimeTag;
		uint32_t timeTag2 = 1;
		auto fromPacketStoreId = packetStoreIds[0];
		auto toPacketStoreId = packetStoreIds[2];

		request.appendEnum8(typeOfTimeWindow);
		request.appendUint32(timeTag2);
		request.appendString(fromPacketStoreId);
		request.appendString(toPacketStoreId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		auto& targetPacketStore = storageAndRetrieval.getPacketStore(toPacketStoreId);
		REQUIRE(targetPacketStore.storedTelemetryPackets.empty());

		ServiceTests::reset();
		Services.reset();
	}
}
