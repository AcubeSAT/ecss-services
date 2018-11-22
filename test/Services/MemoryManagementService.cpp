#include <catch2/catch.hpp>
#include <Services/MemoryManagementService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[6,2]", "[service][st06]") {
	// Required test variables
	char *pStr = static_cast<char *>(malloc(4));
	*pStr = 'T';
	*(pStr + 1) = 'G';
	*(pStr + 2) = '\0';
	uint8_t data[2] = {'h', 'R'};

	MemoryManagementService memMangService;

	Message receivedPacket = Message(6, 2, Message::TC, 1);
	receivedPacket.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	receivedPacket.appendUint16(2); // Iteration count
	receivedPacket.appendUint64(reinterpret_cast<uint64_t >(pStr)); // Start address
	receivedPacket.appendOctetString(2, data);
	receivedPacket.appendUint64(reinterpret_cast<uint64_t >(pStr + 2)); // Start address
	receivedPacket.appendOctetString(1, data);
	memMangService.rawDataMemorySubservice.loadRawData(receivedPacket);

	CHECK(pStr[0] == 'h');
	CHECK(pStr[1] == 'R');
	CHECK(pStr[2] == 'h');
}

TEST_CASE("TM[6,5]", "[service][st06]") {
	uint8_t testString_1[6] = "FStrT";
	uint8_t testString_2[8] = "SecStrT";
	uint8_t testString_3[2] = {5, 8};

	uint8_t *checkString = nullptr;
	uint16_t readSize = 0;

	MemoryManagementService memMangService;
	Message receivedPacket = Message(6, 5, Message::TC, 1);
	receivedPacket.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	receivedPacket.appendUint16(3); // Iteration count (Equal to 3 test strings)
	receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_1)); // Start address
	receivedPacket.appendUint16(sizeof(testString_1)/ sizeof(testString_1[0])); // Data read length

	receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_2));
	receivedPacket.appendUint16(sizeof(testString_2)/ sizeof(testString_2[0]));

	receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_3));
	receivedPacket.appendUint16(sizeof(testString_3)/ sizeof(testString_3[0]));
	memMangService.rawDataMemorySubservice.dumpRawData(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 6);
	CHECK(response.messageType == 6);
	REQUIRE(response.dataSize == 49);

	CHECK(response.readEnum8() == MemoryManagementService::MemoryID::RAM);
	CHECK(response.readUint16() == 3);
	CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_1));
	checkString = response.readOctetString(&readSize);
	CHECK(readSize == sizeof(testString_1)/ sizeof(testString_1[0]));
	CHECK(checkString[0] == 'F');
	CHECK(checkString[1] == 'S');
	CHECK(checkString[2] == 't');
	CHECK(checkString[3] == 'r');
	CHECK(checkString[4] == 'T');
	CHECK(checkString[5] == '\0');

	CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_2));
	checkString = response.readOctetString(&readSize);
	CHECK(readSize == sizeof(testString_2)/ sizeof(testString_2[0]));
	CHECK(checkString[0] == 'S');
	CHECK(checkString[1] == 'e');
	CHECK(checkString[2] == 'c');
	CHECK(checkString[3] == 'S');
	CHECK(checkString[4] == 't');
	CHECK(checkString[5] == 'r');
	CHECK(checkString[6] == 'T');
	CHECK(checkString[7] == '\0');

	CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_3));
	checkString = response.readOctetString(&readSize);
	CHECK(readSize == sizeof(testString_3)/ sizeof(testString_3[0]));
	CHECK(checkString[0] == 5);
	CHECK(checkString[1] == 8);
}
