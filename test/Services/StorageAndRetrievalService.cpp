#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"

void buildPacketCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 8;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps1ps2ps2ps25ps799ps311ps5555ps1111";
	uint16_t offsets[9] = {0, 3, 6, 9, 13, 18, 23, 29, 34};
	uint16_t sizes[8] = {100, 200, 550, 340, 292, 670, 400, 270};
	uint8_t virtualChannels[8] = {0, 6, 1, 2, 3, 15, 5, 8};
	uint16_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE];
		for (auto &x : packetStoreData) {
			x = 0;
		}
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1], packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> packetStoreId(packetStoreData);
		request.appendOctetString(packetStoreId);
		request.appendUint16(sizes[i]);
		if (!(i % 2)) {
			request.appendUint16(packetStoreTypeCode[0]);
		} else {
			request.appendUint16(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

TEST_CASE("Storage And Retrieval Service") {
	SECTION("Packet store creation") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores,Message::TC,1);
		buildPacketCreationRequest(request);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidVirtualChannel) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AlreadyExistingPacketStore) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxNumberOfPacketStoresReached) == 1);
		CHECK(Services.storageAndRetrieval.packetStores.size() == 4);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);

		CHECK(Services.storageAndRetrieval.packetStores[id].sizeInBytes == 200);
		CHECK(Services.storageAndRetrieval.packetStores[id2].sizeInBytes == 340);
		CHECK(Services.storageAndRetrieval.packetStores[id3].sizeInBytes == 292);
		CHECK(Services.storageAndRetrieval.packetStores[id4].sizeInBytes == 400);

		CHECK(Services.storageAndRetrieval.packetStores[id].virtualChannel == 6);
		CHECK(Services.storageAndRetrieval.packetStores[id2].virtualChannel == 2);
		CHECK(Services.storageAndRetrieval.packetStores[id3].virtualChannel == 3);
		CHECK(Services.storageAndRetrieval.packetStores[id4].virtualChannel == 5);

		CHECK(Services.storageAndRetrieval.packetStores[id].packetStoreType == PacketStore::PacketStoreType::Bounded);
		CHECK(Services.storageAndRetrieval.packetStores[id2].packetStoreType == PacketStore::PacketStoreType::Bounded);
		CHECK(Services.storageAndRetrieval.packetStores[id3].packetStoreType == PacketStore::PacketStoreType::Circular);
		CHECK(Services.storageAndRetrieval.packetStores[id4].packetStoreType == PacketStore::PacketStoreType::Circular);

	}
//	SECTION("Storage function enabling") {
//		Message request(StorageAndRetrievalService::ServiceType,
//		                StorageAndRetrievalService::MessageType::EnableStorageFunction,Message::TC,1);
//
//	}
}