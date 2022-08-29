#include <Message.hpp>
#include <Services/MemoryManagementService.hpp>
#include <catch2/catch_all.hpp>
#include "Helpers/CRCHelper.hpp"
#include "ServiceTests.hpp"

MemoryManagementService& memMangService = Services.memoryManagement;

TEST_CASE("TC[6,2]", "[service][st06]") {
	// Required test variables
	char* pStr = static_cast<char*>(malloc(4));
	*pStr = 'T';
	*(pStr + 1) = 'G';
	*(pStr + 2) = '\0';
	uint8_t data[2] = {'h', 'R'};

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas, Message::TC, 1);
	receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	receivedPacket.appendUint16(2);                                          // Iteration count
	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(pStr));           // Start address
	receivedPacket.appendOctetString(String<2>(data));
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 2));   // Append CRC
	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(pStr + 2)); // Start address
	receivedPacket.appendOctetString(String<1>(data));                 // Append CRC
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 1));
	MessageParser::execute(receivedPacket);

	CHECK(pStr[0] == 'h');
	CHECK(pStr[1] == 'R');
	CHECK(pStr[2] == 'h');

	free(pStr);
}

TEST_CASE("TC[6,5]", "[service][st06]") {
	uint8_t testString_1[6] = "FStrT";
	uint8_t testString_2[8] = "SecStrT";
	uint8_t testString_3[2] = {5, 8};

	uint8_t checkString[ECSSMaxStringSize];
	uint16_t readSize = 0, checksum = 0;

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::DumpRawMemoryData, Message::TC, 1);
	receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL);     // Memory ID
	receivedPacket.appendUint16(3);                                              // Iteration count (Equal to 3 test strings)
	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(testString_1));       // Start address
	receivedPacket.appendUint16(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(testString_2));
	receivedPacket.appendUint16(sizeof(testString_2) / sizeof(testString_2[0]));

	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(testString_3));
	receivedPacket.appendUint16(sizeof(testString_3) / sizeof(testString_3[0]));
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	REQUIRE(response.dataSize == 55);

	CHECK(response.readEnum8() == MemoryManagementService::MemoryID::EXTERNAL);
	CHECK(response.readUint16() == 3);
	CHECK(response.readUint64() == reinterpret_cast<uint64_t>(testString_1));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(16);
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checkString[0] == 'F');
	CHECK(checkString[1] == 'S');
	CHECK(checkString[2] == 't');
	CHECK(checkString[3] == 'r');
	CHECK(checkString[4] == 'T');
	CHECK(checkString[5] == '\0');
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

	CHECK(response.readUint64() == reinterpret_cast<uint64_t>(testString_2));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(16);
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

	CHECK(response.readUint64() == reinterpret_cast<uint64_t>(testString_3));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(16);
	CHECK(readSize == sizeof(testString_3) / sizeof(testString_3[0]));
	CHECK(checkString[0] == 5);
	CHECK(checkString[1] == 8);
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));
}

TEST_CASE("TC[6,9]", "[service][st06]") {
	uint8_t testString_1[6] = "FStrT";
	uint8_t testString_2[8] = "SecStrT";
	uint16_t readSize = 0, checksum = 0;

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::CheckRawMemoryData, Message::TC, 1);
	receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL);     // Memory ID
	receivedPacket.appendUint16(2);                                              // Iteration count
	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(testString_1));       // Start address
	receivedPacket.appendUint16(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

	receivedPacket.appendUint64(reinterpret_cast<uint64_t>(testString_2));
	receivedPacket.appendUint16(sizeof(testString_2) / sizeof(testString_2[0]));
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	REQUIRE(response.dataSize == 27);

	CHECK(response.readEnum8() == MemoryManagementService::MemoryID::EXTERNAL);
	CHECK(response.readUint16() == 2);
	CHECK(response.readUint64() == reinterpret_cast<uint64_t>(testString_1));
	readSize = response.readUint16();
	checksum = response.readBits(16);
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_1, readSize));

	CHECK(response.readUint64() == reinterpret_cast<uint64_t>(testString_2));
	readSize = response.readUint16();
	checksum = response.readBits(16);
	CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_2, readSize));
}
