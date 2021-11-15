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

void buildPacketDeletionRequest(Message& request) {
	uint16_t numOfPacketStores = 6;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps1ps2ps27ps799ps5555ps1111";
	uint16_t offsets[7] = {0, 3, 6, 10, 15, 21, 27};
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
		/**
		 * @note so currently there are 4 packet stores, named "ps2", "ps25", "ps799", "ps5555"
		 */
	}

	SECTION("Storage function enabling") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageFunction,Message::TC,1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps33";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);

		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		CHECK(Services.storageAndRetrieval.packetStores[id].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id3].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == false);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 2);
		CHECK(Services.storageAndRetrieval.packetStores[id].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id3].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == false);

		Message request2(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::EnableStorageFunction,Message::TC,1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 6);
		CHECK(Services.storageAndRetrieval.packetStores[id].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id3].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == true);
	}

	SECTION("Storage function disabling") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DisableStorageFunction,Message::TC,1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps33";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);

		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData7[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id7(packetStoreData7);

		CHECK(Services.storageAndRetrieval.packetStores[id2].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id7].storageStatus == true);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 9);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 5);
		CHECK(Services.storageAndRetrieval.packetStores[id2].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id7].storageStatus == true);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::DisableStorageFunction,Message::TC,1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 9);
		CHECK(Services.storageAndRetrieval.packetStores[id2].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].storageStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id7].storageStatus == false);
	}

	SECTION("Open retrieval start time tag changing") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartTimeTag,Message::TC,1);
		uint32_t startTimeTag = 200;
		/**
		 * todo: use function (CUC) to get the timestamp
		 */
		uint16_t numOfPacketStores = 6;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);
		request.appendOctetString(id5);
		request.appendOctetString(id6);

		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus = PacketStore::InProgress;

		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStartTimeTag == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStartTimeTag == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStartTimeTag == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStartTimeTag == 0);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 13);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 2);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStartTimeTag == 200);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStartTimeTag == 200);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStartTimeTag == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStartTimeTag == 0);

		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::Suspended;

		Message request2(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartTimeTag,Message::TC,1);
		startTimeTag = 15;
		numOfPacketStores = 0;
		request2.appendUint32(startTimeTag);
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 14);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 3);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStartTimeTag == 15);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStartTimeTag == 15);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStartTimeTag == 15);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStartTimeTag == 0);
	}

	SECTION("Open retrieval resuming") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores,Message::TC,1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);
		request.appendOctetString(id5);
		request.appendOctetString(id6);

		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus = PacketStore::Suspended;

		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = true;
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = true;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 18);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 9);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 2);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus == PacketStore::Suspended);

		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = false;

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores,Message::TC,1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 19);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 3);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus == PacketStore::InProgress);
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
	}

	SECTION("Open retrieval suspension") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores,Message::TC,1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);

		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus == PacketStore::InProgress);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus == PacketStore::InProgress);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 21);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 11);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus == PacketStore::InProgress);

		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus = PacketStore::InProgress;

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores,Message::TC,1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 21);
		CHECK(Services.storageAndRetrieval.packetStores[id].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id2].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus == PacketStore::Suspended);
		CHECK(Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus == PacketStore::Suspended);
	}

	SECTION("Starting the by-time-range retrieval of packet stores") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval,Message::TC,1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);
		uint32_t fromTime = 4;
		uint32_t toTime = 18;

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		/**
		 * @todo: actually skip the rest of the invalid message before reading the next id
		 */
		request.appendOctetString(id);
//		request.appendUint32(fromTime);
//		request.appendUint32(toTime);
		request.appendOctetString(id2);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);
		request.appendOctetString(id3);
//		request.appendUint32(fromTime);
//		request.appendUint32(toTime);
		request.appendOctetString(id4);
//		request.appendUint32(fromTime);
//		request.appendUint32(toTime);
		request.appendOctetString(id5);
		request.appendUint32(6);
		request.appendUint32(4);
		request.appendOctetString(id6);
//		request.appendUint32(fromTime);
//		request.appendUint32(toTime);

		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = true;

		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id2].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == false);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 26);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 13);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ByTimeRangeRetrievalAlreadyEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 1);

		CHECK(Services.storageAndRetrieval.packetStores[id2].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id2].retrievalStartTime == 4);
		CHECK(Services.storageAndRetrieval.packetStores[id2].retrievalEndTime == 18);
		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id].retrievalStartTime == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id].retrievalEndTime == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].retrievalStartTime == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id5].retrievalEndTime == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id6].retrievalStartTime == 0);
		CHECK(Services.storageAndRetrieval.packetStores[id6].retrievalEndTime == 0);

		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = true;
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = true;
	}

	SECTION("Aborting the by-time-range retrieval of packet stores") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval,Message::TC,1);
		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps2";
		uint8_t packetStoreData2[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps7444";
		uint8_t packetStoreData3[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps1111";
		uint8_t packetStoreData4[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps799";
		uint8_t packetStoreData5[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps5555";
		uint8_t packetStoreData6[ECSS_MAX_PACKET_STORE_ID_SIZE] = "ps25";

		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id(packetStoreData);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id2(packetStoreData2);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id3(packetStoreData3);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id4(packetStoreData4);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id5(packetStoreData5);
		String <ECSS_MAX_PACKET_STORE_ID_SIZE> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);

		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == true);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 28);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 15);
		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus == true);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == true);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval,Message::TC,1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 28);
		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == false);
	}

//	SECTION("Packet store deletion") {
//		Message request(StorageAndRetrievalService::ServiceType,
//		                StorageAndRetrievalService::MessageType::DeletePacketStores,Message::TC,1);
//		buildPacketDeletionRequest(request);
//		MessageParser::execute(request);
//	}


}