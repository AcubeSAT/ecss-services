#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/StorageAndRetrievalService.hpp"

uint32_t timestamps1[6] = {2, 4, 5, 7, 9, 11};
uint32_t timestamps2[5] = {0, 1, 4, 15, 22};
uint32_t timestamps3[4] = {4, 7, 9, 14};
uint32_t timestamps4[8] = {4, 6, 34, 40, 44, 51, 52, 58};

void buildPacketCreationRequest(Message& request) {
	uint16_t numOfPacketStores = 8;
	request.appendUint16(numOfPacketStores);
	uint8_t concatenatedPacketStoreNames[] = "ps1ps2ps2ps25ps799ps311ps5555ps1111";
	uint16_t offsets[9] = {0, 3, 6, 9, 13, 18, 23, 29, 34};
	uint16_t sizes[8] = {100, 200, 550, 340, 292, 670, 400, 270};
	uint8_t virtualChannels[8] = {0, 6, 1, 2, 3, 15, 5, 8};
	uint8_t packetStoreTypeCode[2] = {0, 1};

	for (int i = 0; i < numOfPacketStores; i++) {
		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize];
		for (auto& x : packetStoreData) {
			x = 0;
		}
		std::copy(concatenatedPacketStoreNames + offsets[i], concatenatedPacketStoreNames + offsets[i + 1],
		          packetStoreData);
		String<ECSSMaxPacketStoreIdSize> packetStoreId(packetStoreData);
		request.appendOctetString(packetStoreId);
		request.appendUint16(sizes[i]);
		if (!(i % 2)) {
			request.appendUint8(packetStoreTypeCode[0]);
		} else {
			request.appendUint8(packetStoreTypeCode[1]);
		}
		request.appendUint8(virtualChannels[i]);
	}
}

void addTelemetryPacketsInPacketStores() {
	uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
	uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
	uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
	uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

	String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
	String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
	String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
	String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

	for (auto& timestamp : timestamps1) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets.push_back({timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps2) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[id2].storedTelemetryPackets.push_back({timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps3) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[id3].storedTelemetryPackets.push_back({timestamp, tmPacket});
	}
	for (auto& timestamp : timestamps4) {
		Message tmPacket;
		Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.push_back({timestamp, tmPacket});
	}
}

Message buildReportTypeAdditionRequest(String<ECSSMaxPacketStoreIdSize>& packetStoreId) {
	Message request(StorageAndRetrievalService::ServiceType,
	                StorageAndRetrievalService::MessageType::AddReportTypesToAppProcessConfiguration, Message::TC, 1);
	request.appendOctetString(packetStoreId);
	uint16_t numOfApplicationIds = 3;
	request.appendUint16(numOfApplicationIds);

	// app-ID1
	request.appendUint16(1);
	uint16_t numOfServices = 4;
	request.appendUint16(numOfServices);
	// service-1
	request.appendUint16(1);
	uint16_t numOfReports = 5;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(1);
	request.appendUint16(2);
	request.appendUint16(3);
	request.appendUint16(4);
	request.appendUint16(5);
	// service-2
	request.appendUint16(4);
	numOfReports = 2;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(5);
	request.appendUint16(6);
	// service-3
	request.appendUint16(3);
	numOfReports = 7;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(5);
	request.appendUint16(6);
	request.appendUint16(9);
	request.appendUint16(11);
	request.appendUint16(12);
	request.appendUint16(15);
	request.appendUint16(16);
	// service-4
	request.appendUint16(20);
	numOfReports = 4;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(2);
	request.appendUint16(3);
	request.appendUint16(9);
	request.appendUint16(10);

	// app-ID2
	request.appendUint16(8);
	numOfServices = 2;
	request.appendUint16(numOfServices);
	// service-1
	request.appendUint16(1);
	numOfReports = 5;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(1);
	request.appendUint16(2);
	request.appendUint16(3);
	request.appendUint16(4);
	request.appendUint16(5);
	// service-2
	request.appendUint16(4);
	numOfReports = 2;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(5);
	request.appendUint16(6);

	// app-ID3
	request.appendUint16(3);
	numOfServices = 3;
	request.appendUint16(numOfServices);
	// service-1
	request.appendUint16(4);
	numOfReports = 3;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(6);
	request.appendUint16(8);
	request.appendUint16(11);
	// service-2
	request.appendUint16(23);
	numOfReports = 4;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(5);
	request.appendUint16(6);
	request.appendUint16(7);
	request.appendUint16(9);
	// service-3
	request.appendUint16(15);
	numOfReports = 6;
	request.appendUint16(numOfReports);
	// reports
	request.appendUint16(1);
	request.appendUint16(6);
	request.appendUint16(9);
	request.appendUint16(12);
	request.appendUint16(31);
	request.appendUint16(33);

	return request;
}

TEST_CASE("Storage And Retrieval Service") {
	SECTION("Packet store creation") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CreatePacketStores, Message::TC, 1);
		buildPacketCreationRequest(request);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidVirtualChannel) == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AlreadyExistingPacketStore) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MaxNumberOfPacketStoresReached) == 1);
		CHECK(Services.storageAndRetrieval.packetStores.size() == 4);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

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
		                StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps33";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);

		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

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
		                 StorageAndRetrievalService::MessageType::EnableStorageInPacketStores, Message::TC, 1);
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
		                StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps33";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);

		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData7[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);
		String<ECSSMaxPacketStoreIdSize> id7(packetStoreData7);

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
		                 StorageAndRetrievalService::MessageType::DisableStorageInPacketStores, Message::TC, 1);
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
		                StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);
		uint32_t startTimeTag = 200;
		/**
		 * todo: use function (CUC) to get the timestamp
		 */
		uint16_t numOfPacketStores = 6;
		request.appendUint32(startTimeTag);
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

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
		                 StorageAndRetrievalService::MessageType::ChangeOpenRetrievalStartingTime, Message::TC, 1);
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
		                StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

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
		                 StorageAndRetrievalService::MessageType::ResumeOpenRetrievalOfPacketStores, Message::TC, 1);
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
		                StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 4;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

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
		                 StorageAndRetrievalService::MessageType::SuspendOpenRetrievalOfPacketStores, Message::TC, 1);
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
		                StorageAndRetrievalService::MessageType::StartByTimeRangeRetrieval, Message::TC, 1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);
		uint32_t fromTime = 4;
		uint32_t toTime = 18;

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);
		request.appendOctetString(id2);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);
		request.appendOctetString(id3);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);
		request.appendOctetString(id4);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);
		request.appendOctetString(id5);
		request.appendUint32(6);
		request.appendUint32(4);
		request.appendOctetString(id6);
		request.appendUint32(fromTime);
		request.appendUint32(toTime);

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
		                StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);
		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

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
		                 StorageAndRetrievalService::MessageType::AbortByTimeRangeRetrieval, Message::TC, 1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 28);
		CHECK(Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus == false);
		CHECK(Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus == false);
	}

	SECTION("Reporting the packet store status") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportStatusOfPacketStores, Message::TC, 1);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps25";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps5555";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);

		Services.storageAndRetrieval.packetStores[id].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = true;
		Services.storageAndRetrieval.packetStores[id2].storageStatus = true;
		Services.storageAndRetrieval.packetStores[id3].openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 29);
		Message report = ServiceTests::get(28);
		CHECK(report.readUint16() == 4);
		CHECK(report.messageType == StorageAndRetrievalService::MessageType::PacketStoresStatusReport);

		// Packet store 1
		uint8_t data[ECSSMaxPacketStoreIdSize];
		report.readOctetString(data);
		for (int i = 0; i < ECSSMaxPacketStoreIdSize; i++) {
			CHECK(data[i] == packetStoreData[i]);
		}
		CHECK(report.readBoolean() == false);
		CHECK(report.readUint8() == 1);
		CHECK(report.readBoolean() == true);
		// Packet store 2
		report.readOctetString(data);
		for (int i = 0; i < ECSSMaxPacketStoreIdSize; i++) {
			CHECK(data[i] == packetStoreData2[i]);
		}
		CHECK(report.readBoolean() == true);
		CHECK(report.readUint8() == 1);
		CHECK(report.readBoolean() == false);
		// Packet store 3
		report.readOctetString(data);
		for (int i = 0; i < ECSSMaxPacketStoreIdSize; i++) {
			CHECK(data[i] == packetStoreData4[i]);
		}
		CHECK(report.readBoolean() == false);
		CHECK(report.readUint8() == 1);
		CHECK(report.readBoolean() == false);
		// Packet store 4
		report.readOctetString(data);
		for (int i = 0; i < ECSSMaxPacketStoreIdSize; i++) {
			CHECK(data[i] == packetStoreData3[i]);
		}
		CHECK(report.readBoolean() == false);
		CHECK(report.readUint8() == 0);
		CHECK(report.readBoolean() == false);
	}

	SECTION("Deleting packet store content") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStoreContent, Message::TC, 1);
		addTelemetryPacketsInPacketStores();
		uint32_t timeLimit = 7;
		uint16_t numOfPacketStores = 6;
		request.appendUint32(timeLimit);
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);
		request.appendOctetString(id5);
		request.appendOctetString(id6);

		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = true;
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 33);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingPacketStore) == 17);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithByTimeRangeRetrieval) == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetPacketStoreWithOpenRetrievalInProgress) == 4);

		CHECK(Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets.size() == 6); // none deleted
		CHECK(Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.size() == 4); // none deleted
		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.size() == 6); // 1 deleted
		CHECK(Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets.size() == 2); // 3 deleted

		int index = 0;
		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets) {
			CHECK(timestamp.first == timestamps1[index++]);
		}
		index = 0;
		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets) {
			CHECK(timestamp.first == timestamps3[index++]);
		}
		index = 2;
		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets) {
			CHECK(timestamp.first == timestamps4[index++]);
		}
		index = 3;
		for (auto& timestamp : Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets) {
			CHECK(timestamp.first == timestamps2[index++]);
		}

		timeLimit = 55;
		numOfPacketStores = 0;
		request.appendUint32(timeLimit);
		request.appendUint16(numOfPacketStores);

		MessageParser::execute(request);

		CHECK(Services.storageAndRetrieval.packetStores[id].storedTelemetryPackets.empty()); // all remaining deleted
		CHECK(Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.empty()); // all remaining deleted
		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.size() == 1); // 1 remained
		CHECK(Services.storageAndRetrieval.packetStores[id6].storedTelemetryPackets.empty()); // all remaining deleted
		Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.pop_front(); // delete all to fill later
		CHECK(Services.storageAndRetrieval.packetStores[id5].storedTelemetryPackets.empty());
	}

	SECTION("Reporting the packet store configuration") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportConfigurationOfPacketStores, Message::TC, 1);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps25";

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 34);
		Message report = ServiceTests::get(33);
		CHECK(report.messageType == StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport);
		uint8_t data[ECSSMaxPacketStoreIdSize];
		CHECK(report.readUint16() == 4);

		/**
		 * @note:
		 * I don't know if there is a possibility that the map inserts the elements in an order, different than the
		 * expected one. In that case this will be an issue here, but in all of my testing it remained
		 * consistently correct.
		 */

		// packet store ps2
		report.readOctetString(data);
		int index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData[index++]);
		}
		CHECK(report.readUint16() == 200);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint16() == 6);

		// packet store ps25
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData4[index++]);
		}
		CHECK(report.readUint16() == 340);
		CHECK(report.readUint8() == 1);
		CHECK(report.readUint16() == 2);

		// packet store ps5555
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData3[index++]);
		}
		CHECK(report.readUint16() == 400);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint16() == 5);

		// packet store ps799
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData2[index++]);
		}
		CHECK(report.readUint16() == 292);
		CHECK(report.readUint8() == 0);
		CHECK(report.readUint16() == 3);
	}

	SECTION("Copying packets in time window") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		addTelemetryPacketsInPacketStores();

		/**
		 * CASE 1:
		 *
		 * 	(tag1)--------(earlier packet timestamp)-----(..more packets)-----(tag2)--------(latest packet timestamp)
		 * 				left-most packet in deque				tag2 somewehere inside		right-most packet in deque
		 */
		request.appendUint16(0);
		uint32_t timeTag1 = 0;
		uint32_t timeTag2 = 4;
		uint8_t fromPacketStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId(fromPacketStoreData);
		uint8_t toPacketStoreData[ECSSMaxPacketStoreIdSize] = "ps799";
		String<ECSSMaxPacketStoreIdSize> toPacketStoreId(toPacketStoreData);
		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());

		request.appendUint32(timeTag1);
		request.appendUint32(timeTag2);
		request.appendOctetString(fromPacketStoreId);
		request.appendOctetString(toPacketStoreId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 34);

		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 2);
		int index = 0;
		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
			CHECK(tmPacket.first == timestamps1[index++]);
		}

		/**
		 * CASE 2:
		 *
		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(tag2)--------(latest packet timestamp)
		 * 	left-most packet in deque						both tag1 and tag2 inside 		right-most packet in deque
		 */
		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request2.appendUint16(0);
		timeTag1 = 35;
		timeTag2 = 52;
		uint8_t fromPacketStoreData2[ECSSMaxPacketStoreIdSize] = "ps5555";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId2(fromPacketStoreData2);
		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());

		request2.appendUint32(timeTag1);
		request2.appendUint32(timeTag2);
		request2.appendOctetString(fromPacketStoreId2);
		request2.appendOctetString(toPacketStoreId);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 34);

		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 4);
		index = 3;
		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
			CHECK(tmPacket.first == timestamps4[index++]);
		}

		/**
		 * CASE 3:
		 *
		 * 	(earlier packet timestamp)-------(tag1)-----(..more packets)-----(latest packet timestamp)--------(tag2)
		 * 	left-most packet in deque		tag1 inside						right-most packet in deque
		 */
		Message request3(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request3.appendUint16(0);
		timeTag1 = 3;
		timeTag2 = 27;
		uint8_t fromPacketStoreData3[ECSSMaxPacketStoreIdSize] = "ps25";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId3(fromPacketStoreData3);
		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());

		request3.appendUint32(timeTag1);
		request3.appendUint32(timeTag2);
		request3.appendOctetString(fromPacketStoreId3);
		request3.appendOctetString(toPacketStoreId);

		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 34);

		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.size() == 3);
		index = 2;
		for (auto& tmPacket : Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets) {
			CHECK(tmPacket.first == timestamps2[index++]);
		}

		/**
		 * CASE 4: Invalid packet store
		 */
		// Invalid fromPacket
		Message request4(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request4.appendUint16(0);
		uint8_t fromPacketStoreData4[ECSSMaxPacketStoreIdSize] = "ps21";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId4(fromPacketStoreData4);

		request4.appendUint32(timeTag1);
		request4.appendUint32(timeTag2);
		request4.appendOctetString(fromPacketStoreId4);
		request4.appendOctetString(toPacketStoreId);

		MessageParser::execute(request4);
		CHECK(ServiceTests::count() == 35);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 1);

		// Invalid toPacket
		Message request44(StorageAndRetrievalService::ServiceType,
		                  StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request44.appendUint16(0);

		request44.appendUint32(timeTag1);
		request44.appendUint32(timeTag2);
		request44.appendOctetString(fromPacketStoreId3);
		request44.appendOctetString(fromPacketStoreId4);

		MessageParser::execute(request44);
		CHECK(ServiceTests::count() == 36);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 2);

		/**
		 * CASE 5: Invalid time window
		 */
		Message request5(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request5.appendUint16(0);
		timeTag1 = 5;
		timeTag2 = 3;
		uint8_t fromPacketStoreData5[ECSSMaxPacketStoreIdSize] = "ps2";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId5(fromPacketStoreData5);

		request5.appendUint32(timeTag1);
		request5.appendUint32(timeTag2);
		request5.appendOctetString(fromPacketStoreId5);
		request5.appendOctetString(toPacketStoreId);

		MessageParser::execute(request5);
		CHECK(ServiceTests::count() == 37);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidTimeWindow) == 2);

		/**
		 * CASE 6: Destination packet NOT empty
		 */
		Message request6(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request6.appendUint16(0);
		timeTag1 = 3;
		timeTag2 = 7;

		request6.appendUint32(timeTag1);
		request6.appendUint32(timeTag2);
		request6.appendOctetString(fromPacketStoreId5);
		request6.appendOctetString(toPacketStoreId);

		MessageParser::execute(request6);
		CHECK(ServiceTests::count() == 38);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DestinationPacketStoreNotEmtpy) == 1);

		/**
		 * CASE 7: No packets inside the requested time window in the source packet
		 */
		Message request7(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::CopyPacketsInTimeWindow, Message::TC, 1);
		request7.appendUint16(0);
		uint8_t fromPacketStoreData7[ECSSMaxPacketStoreIdSize] = "ps5555";
		String<ECSSMaxPacketStoreIdSize> fromPacketStoreId7(fromPacketStoreData7);

		Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.clear(); // empty ps799
		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());
		timeTag1 = 0;
		timeTag2 = 3;

		request7.appendUint32(timeTag1);
		request7.appendUint32(timeTag2);
		request7.appendOctetString(fromPacketStoreId7);
		request7.appendOctetString(toPacketStoreId);

		MessageParser::execute(request7);
		CHECK(ServiceTests::count() == 39);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::CopyOfPacketsFailed) == 1);
		CHECK(Services.storageAndRetrieval.packetStores[toPacketStoreId].storedTelemetryPackets.empty());

		/**
		 * todo: test the 'afterTimeTag' and 'beforeTimeTag' as well
		 */
	}

	SECTION("Resizing the packet stores") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ResizePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendUint16(444);
		request.appendOctetString(id2);
		request.appendUint16(555);
		request.appendOctetString(id3);
		request.appendUint16(666);
		request.appendOctetString(id4);
		request.appendUint16(777);
		request.appendOctetString(id5);
		request.appendUint16(888);
		request.appendOctetString(id6);
		request.appendUint16(999);

		CHECK(Services.storageAndRetrieval.packetStores[id5].sizeInBytes == 400);
		CHECK(Services.storageAndRetrieval.packetStores[id].sizeInBytes == 200);
		CHECK(Services.storageAndRetrieval.packetStores[id6].sizeInBytes == 340);
		CHECK(Services.storageAndRetrieval.packetStores[id4].sizeInBytes == 292);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 44);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 2);

		CHECK(Services.storageAndRetrieval.packetStores[id5].sizeInBytes == 888);
		CHECK(Services.storageAndRetrieval.packetStores[id].sizeInBytes == 200);
		CHECK(Services.storageAndRetrieval.packetStores[id6].sizeInBytes == 340);
		CHECK(Services.storageAndRetrieval.packetStores[id4].sizeInBytes == 292);
	}

	SECTION("Changing the packet store type to Circular") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		Services.storageAndRetrieval.packetStores[id].packetStoreType = PacketStore::Bounded;
		Services.storageAndRetrieval.packetStores[id4].packetStoreType = PacketStore::Bounded;
		Services.storageAndRetrieval.packetStores[id5].packetStoreType = PacketStore::Bounded;
		Services.storageAndRetrieval.packetStores[id6].packetStoreType = PacketStore::Bounded;

		request.appendOctetString(id2);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 45);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 5);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);
		request2.appendOctetString(id4);
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 46);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 3);

		Message request3(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id].storageStatus = true;
		request3.appendOctetString(id);
		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 47);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithStorageStatusEnabled) == 2);

		Message request4(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id6].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = true;
		request4.appendOctetString(id6);
		MessageParser::execute(request4);
		CHECK(ServiceTests::count() == 48);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 2);

		Message request5(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToCircular, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id5].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::Suspended;
		request5.appendOctetString(id5);
		Services.storageAndRetrieval.packetStores[id5].packetStoreType = PacketStore::Bounded;
		MessageParser::execute(request5);
		CHECK(ServiceTests::count() == 48);
		CHECK(Services.storageAndRetrieval.packetStores[id5].packetStoreType == PacketStore::Circular);
	}

	SECTION("Changing the packet store type to Bounded") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id2);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 49);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 6);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);
		request2.appendOctetString(id4);
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 50);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 4);

		Message request3(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id].storageStatus = true;
		request3.appendOctetString(id);
		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 51);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithStorageStatusEnabled) == 3);

		Message request4(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id6].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = true;
		request4.appendOctetString(id6);
		MessageParser::execute(request4);
		CHECK(ServiceTests::count() == 52);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 3);

		Message request5(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id5].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::Suspended;
		request5.appendOctetString(id5);
		Services.storageAndRetrieval.packetStores[id5].packetStoreType = PacketStore::Circular;
		MessageParser::execute(request5);
		CHECK(ServiceTests::count() == 52);
		CHECK(Services.storageAndRetrieval.packetStores[id5].packetStoreType == PacketStore::Bounded);
	}

	SECTION("Changing the virtual channel of packet stores") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		uint8_t virtualChannel = 5;

		request.appendOctetString(id2);
		request.appendUint8(virtualChannel);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 53);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 7);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);
		request2.appendOctetString(id4);
		request2.appendUint8(virtualChannel);
		Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id4].openRetrievalStatus = PacketStore::InProgress;
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 54);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithOpenRetrievalInProgress) == 5);

		Message request3(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::Suspended;
		request3.appendOctetString(id);
		virtualChannel = 12;
		request3.appendUint8(virtualChannel);
		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 55);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidVirtualChannel) == 3);

		Message request4(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeTypeToBounded, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = true;
		request4.appendOctetString(id6);
		request4.appendUint8(virtualChannel);
		MessageParser::execute(request4);
		CHECK(ServiceTests::count() == 56);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetPacketStoreWithByTimeRangeRetrieval) == 4);

		Message request5(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ChangeVirtualChannel, Message::TC, 1);
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::Suspended;
		request5.appendOctetString(id5);
		virtualChannel = 7;
		request5.appendUint8(virtualChannel);
		CHECK(Services.storageAndRetrieval.packetStores[id5].virtualChannel == static_cast<uint8_t>(5));
		MessageParser::execute(request5);
		CHECK(ServiceTests::count() == 56);
		CHECK(Services.storageAndRetrieval.packetStores[id5].virtualChannel == static_cast<uint8_t>(7));
	}

	SECTION("Reporting the packet store content summary") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 3;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 59);
		Message report = ServiceTests::get(58);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 9);

		uint8_t data[ECSSMaxPacketStoreIdSize];
		CHECK(report.readUint16() == 1);
		report.readOctetString(data);
		int index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData[index++]);
		}
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 0);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::ReportContentSummaryOfPacketStores, Message::TC, 1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStartTimeTag = 20;
		Services.storageAndRetrieval.packetStores[id4].storedTelemetryPackets.clear();

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 60);
		report = ServiceTests::get(59);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 9);
		CHECK(report.readUint16() == 4);
		// For ps2
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData[index++]);
		}
		CHECK(report.readUint32() == timestamps1[0]);
		CHECK(report.readUint32() == timestamps1[5]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 30);
		CHECK(report.readUint16() == 0);
		// For ps25
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData6[index++]);
		}
		CHECK(report.readUint32() == timestamps2[0]);
		CHECK(report.readUint32() == timestamps2[4]);
		CHECK(report.readUint32() == 15);
		CHECK(report.readUint16() == 25);
		CHECK(report.readUint16() == 10);
		// For ps5555
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData5[index++]);
		}
		CHECK(report.readUint32() == timestamps4[0]);
		CHECK(report.readUint32() == timestamps4[7]);
		CHECK(report.readUint32() == 20);
		CHECK(report.readUint16() == 40);
		CHECK(report.readUint16() == 30);
		// For ps7999
		report.readOctetString(data);
		index = 0;
		for (auto& x : data) {
			CHECK(x == packetStoreData4[index++]);
		}
		// Skip the rest because it was previously used to copy other packets into it.
	}

	SECTION("Deleting packet stores") {
		Message request(StorageAndRetrievalService::ServiceType,
		                StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		uint16_t numOfPacketStores = 6;
		request.appendUint16(numOfPacketStores);

		uint8_t packetStoreData[ECSSMaxPacketStoreIdSize] = "ps2";
		uint8_t packetStoreData2[ECSSMaxPacketStoreIdSize] = "ps7444";
		uint8_t packetStoreData3[ECSSMaxPacketStoreIdSize] = "ps1111";
		uint8_t packetStoreData4[ECSSMaxPacketStoreIdSize] = "ps799";
		uint8_t packetStoreData5[ECSSMaxPacketStoreIdSize] = "ps5555";
		uint8_t packetStoreData6[ECSSMaxPacketStoreIdSize] = "ps25";

		String<ECSSMaxPacketStoreIdSize> id(packetStoreData);
		String<ECSSMaxPacketStoreIdSize> id2(packetStoreData2);
		String<ECSSMaxPacketStoreIdSize> id3(packetStoreData3);
		String<ECSSMaxPacketStoreIdSize> id4(packetStoreData4);
		String<ECSSMaxPacketStoreIdSize> id5(packetStoreData5);
		String<ECSSMaxPacketStoreIdSize> id6(packetStoreData6);

		request.appendOctetString(id);
		request.appendOctetString(id2);
		request.appendOctetString(id3);
		request.appendOctetString(id4);
		request.appendOctetString(id5);
		request.appendOctetString(id6);

		Services.storageAndRetrieval.packetStores[id].storageStatus = true;
		Services.storageAndRetrieval.packetStores[id4].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus = true;
		Services.storageAndRetrieval.packetStores[id5].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::InProgress;
		Services.storageAndRetrieval.packetStores[id6].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id6].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id6].openRetrievalStatus = PacketStore::Suspended;

		CHECK(Services.storageAndRetrieval.packetStores.size() == 4);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 65);
		CHECK(Services.storageAndRetrieval.packetStores.size() == 3);
		CHECK(Services.storageAndRetrieval.packetStores.find(id6) == Services.storageAndRetrieval.packetStores.end());
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingPacketStore) == 11);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketStoreWithStorageStatusEnabled) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithByTimeRangeRetrieval) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DeletionOfPacketWithOpenRetrievalInProgress) == 1);

		Message request2(StorageAndRetrievalService::ServiceType,
		                 StorageAndRetrievalService::MessageType::DeletePacketStores, Message::TC, 1);
		numOfPacketStores = 0;
		request2.appendUint16(numOfPacketStores);

		Services.storageAndRetrieval.packetStores[id].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id4].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id4].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id4].openRetrievalStatus = PacketStore::Suspended;
		Services.storageAndRetrieval.packetStores[id5].storageStatus = false;
		Services.storageAndRetrieval.packetStores[id5].byTimeRangeRetrievalStatus = false;
		Services.storageAndRetrieval.packetStores[id5].openRetrievalStatus = PacketStore::InProgress;

		MessageParser::execute(request2);

		CHECK(Services.storageAndRetrieval.packetStores.size() == 1);
		CHECK(Services.storageAndRetrieval.packetStores.find(id) == Services.storageAndRetrieval.packetStores.end());
		CHECK(Services.storageAndRetrieval.packetStores.find(id4) == Services.storageAndRetrieval.packetStores.end());
		CHECK(Services.storageAndRetrieval.packetStores.find(id5) != Services.storageAndRetrieval.packetStores.end());
	}
}