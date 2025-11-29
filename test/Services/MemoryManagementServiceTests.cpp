#include <Message.hpp>
#include <Services/MemoryManagementService.hpp>
#include <catch2/catch_all.hpp>
#include "Helpers/CRCHelper.hpp"
#include "Platform/x86/Helpers/TestMemory.hpp"
#include "ServiceTests.hpp"

constexpr MemoryId TEST_MEMORY = 0;

MemoryManagementService& memMangService = Services.memoryManagement;

extern TestMemory testMemory;

TEST_CASE("TC[6,2]", "[service][st06]") {
	// Required test variables
	auto arr = testMemory.getDummyArea();
	arr[0] = 'T';
	arr[1] = 'G';
	arr[2] = '\0';
	uint8_t data[2] = {'h', 'R'};

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas, Message::TC, 1);
	receivedPacket.append<MemoryId>(TEST_MEMORY);                              // Memory ID
	receivedPacket.appendUint16(2);                                            // Iteration count
	receivedPacket.append<MemoryAddress>(0); // Start address
	receivedPacket.appendOctetString(String<2>(data));
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 2));               // Append CRC
	receivedPacket.append<MemoryAddress>(2); // Start address
	receivedPacket.appendOctetString(String<1>(data));                             // Append CRC
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 1));
	MessageParser::execute(receivedPacket);

	CHECK(arr[0] == 'h');
	CHECK(arr[1] == 'R');
	CHECK(arr[2] == 'h');
}

TEST_CASE("TC[6,5]", "[service][st06]") {

	auto dummyPointer = testMemory.getDummyArea().data();

	uint8_t stringLocations[] = { 0, 10, 20 };
	uint8_t testString_1[6] = u8"FStrT";
	uint8_t testString_2[8] = u8"SecStrT";
	uint8_t testString_3[2] = {5, 8};

	std::copy(testString_1, testString_1 + sizeof(testString_1), dummyPointer + stringLocations[0]);
	std::copy(testString_2, testString_2 + sizeof(testString_2), dummyPointer + stringLocations[1]);
	std::copy(testString_3, testString_3 + sizeof(testString_3), dummyPointer + stringLocations[2]);

	uint8_t checkString[ECSSMaxStringSize];
	uint16_t readSize = 0, checksum = 0;

	std::copy(testString_1, testString_1 + sizeof(testString_1), dummyPointer + stringLocations[0]);
	std::copy(testString_2, testString_2 + sizeof(testString_2), dummyPointer + stringLocations[1]);

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::DumpRawMemoryData, Message::TC, 1);
	receivedPacket.append<MemoryId>(TEST_MEMORY);                                            // Memory ID
	receivedPacket.appendUint16(3);                                                          // Iteration count (Equal to 3 test strings)
    receivedPacket.append<MemoryAddress>(stringLocations[0]);       // Start address
	receivedPacket.append<MemoryDataLength>(sizeof(testString_1)); // Data read length

	receivedPacket.append<MemoryAddress>(stringLocations[1]);
	receivedPacket.append<MemoryDataLength>(sizeof(testString_2)); // Data read length

	receivedPacket.append<MemoryAddress>(stringLocations[2]);
	receivedPacket.append<MemoryDataLength>(sizeof(testString_3)); // Data read length
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	REQUIRE(response.dataSize ==
	        sizeof(MemoryId) +
	        sizeof(uint16_t) + // iteration count size
			(sizeof(MemoryAddress) + sizeof(testString_1)/sizeof(testString_1[0]) + sizeof(uint16_t) + sizeof(uint16_t)) + // iteration 1
			(sizeof(MemoryAddress) + sizeof(testString_2)/sizeof(testString_2[0]) + sizeof(uint16_t) + sizeof(uint16_t)) + // iteration 2
			(sizeof(MemoryAddress) + sizeof(testString_3)/sizeof(testString_3[0]) + sizeof(uint16_t) + sizeof(uint16_t)) // iteration 3
	        );

	CHECK(response.read<MemoryId>() == TEST_MEMORY);
	CHECK(response.readUint16() == 3);
	CHECK(response.read<MemoryAddress>() == stringLocations[0]);
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8 * sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checkString[0] == 'F');
	CHECK(checkString[1] == 'S');
	CHECK(checkString[2] == 't');
	CHECK(checkString[3] == 'r');
	CHECK(checkString[4] == 'T');
	CHECK(checkString[5] == '\0');
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

	CHECK(response.read<MemoryAddress>() == stringLocations[1]);
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8 * sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
	CHECK(checkString[0] == 'S');
	CHECK(checkString[1] == 'e');
	CHECK(checkString[2] == 'c');
	CHECK(checkString[3] == 'S');
	CHECK(checkString[4] == 't');
	CHECK(checkString[5] == 'r');
	CHECK(checkString[6] == 'T');
	CHECK(checkString[7] == '\0');
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

	CHECK(response.read<MemoryAddress>() == stringLocations[2]);
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8 * sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_3) / sizeof(testString_3[0]));
	CHECK(checkString[0] == 5);
	CHECK(checkString[1] == 8);
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));
}

TEST_CASE("TC[6,9]", "[service][st06]") {

	auto dummyPointer = testMemory.getDummyArea().data();
	uint8_t stringLocations[] = { 0, 10 };
	uint8_t testString_1[6] = u8"FStrT";
	uint8_t testString_2[8] = u8"SecStrT";

	std::copy(testString_1, testString_1 + sizeof(testString_1), dummyPointer + stringLocations[0]);
	std::copy(testString_2, testString_2 + sizeof(testString_2), dummyPointer + stringLocations[1]);

	uint16_t readSize = 0, checksum = 0;

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::CheckRawMemoryData, Message::TC, 1);
	receivedPacket.append<MemoryId>(TEST_MEMORY);                                            // Memory ID
	receivedPacket.appendUint16(2);                                                          // Iteration count
	receivedPacket.append<MemoryAddress>(stringLocations[0]);       // Start address
	receivedPacket.append<MemoryDataLength>(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

	receivedPacket.append<MemoryAddress>(stringLocations[1]);
	receivedPacket.append<MemoryDataLength>(sizeof(testString_2) / sizeof(testString_2[0]));
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	REQUIRE(response.dataSize ==
	        sizeof(MemoryId) +
	        sizeof(uint16_t) + // iteration count size
	        2 * (sizeof(MemoryAddress) + sizeof(uint16_t) + sizeof(uint16_t)) // two iterations
	        );

	CHECK(response.read<MemoryId>() == TEST_MEMORY);
	CHECK(response.readUint16() == 2);
	CHECK(response.read<MemoryAddress>() == stringLocations[0]);
	readSize = response.readUint16();
	checksum = response.readBits(8 * sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_1, readSize));

	CHECK(response.read<MemoryAddress>() == stringLocations[1]);
	readSize = response.readUint16();
	checksum = response.readBits(8 * sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_2, readSize));
}
