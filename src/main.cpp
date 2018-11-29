#include <iostream>
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Message.hpp"
#include "Services/MemoryManagementService.hpp"

int main() {
	Message packet = Message(0, 0, Message::TC, 1);

	packet.appendString(5, "hello");
	packet.appendBits(15, 0x28a8);
	packet.appendBits(1, 1);
	packet.appendFloat(5.7);
	packet.appendSint32(-123456789);

	std::cout << "Hello, World!" << std::endl;
	std::cout << std::hex << packet.data << std::endl; // packet data must be 'helloQQ'

	char string[6];
	packet.readString(string, 5);
	std::cout << "Word: " << string << " " << packet.readBits(15) << packet.readBits(1)
	          << std::endl;
	std::cout << packet.readFloat() << " " << std::dec << packet.readSint32() << std::endl;

	// ST[17] test
	TestService testService;
	Message receivedPacket = Message(17, 1, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(17, 3, Message::TC, 1);
	receivedPacket.appendUint16(7);
	testService.onBoardConnection(receivedPacket);


	// ST[20] test
	ParameterService paramService;

	// Test code for reportParameter
	Message sentPacket = Message(20, 1, Message::TC, 1);  //application id is a dummy number (1)
	sentPacket.appendUint16(2);  //number of contained IDs
	sentPacket.appendUint16(0);  //first ID
	sentPacket.appendUint16(1);  //second ID
	paramService.reportParameterIds(sentPacket);

	// Test code for setParameter
	Message sentPacket2 = Message(20, 3, Message::TC, 1);  //application id is a dummy number (1)
	sentPacket2.appendUint16(2);  //number of contained IDs
	sentPacket2.appendUint16(0);  //first parameter ID
	sentPacket2.appendUint32(63238);  //settings for first parameter
	sentPacket2.appendUint16(1);  //2nd parameter ID
	sentPacket2.appendUint32(45823);  //settings for 2nd parameter

	paramService.setParameterIds(sentPacket2);
	paramService.reportParameterIds(sentPacket);

// ST[06] testing
	char anotherStr[8] = "Fgthred";
	char yetAnotherStr[2] = "F";
	char *pStr = static_cast<char *>(malloc(4));
	*pStr = 'T';
	*(pStr + 1) = 'G';
	*(pStr + 2) = '\0';

	MemoryManagementService memMangService;
	Message rcvPack = Message(6, 5, Message::TC, 1);
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	rcvPack.appendUint16(3); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(string)); // Start address
	rcvPack.appendUint16(sizeof(string) / sizeof(string[0])); // Data read length

	rcvPack.appendUint64(reinterpret_cast<uint64_t >(anotherStr));
	rcvPack.appendUint16(sizeof(anotherStr) / sizeof(anotherStr[0]));

	rcvPack.appendUint64(reinterpret_cast<uint64_t >(yetAnotherStr));
	rcvPack.appendUint16(sizeof(yetAnotherStr) / sizeof(yetAnotherStr[0]));
	memMangService.rawDataMemorySubservice.dumpRawData(rcvPack);

	rcvPack = Message(6, 2, Message::TC, 1);

	uint8_t data[2] = {'h', 'R'};
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr)); // Start address
	rcvPack.appendOctetString(2, data);
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr + 1)); // Start address
	rcvPack.appendOctetString(1, data);
	memMangService.rawDataMemorySubservice.loadRawData(rcvPack);


	// ST[01] test

	RequestVerificationService reqVerifService;
	Message receivedMessage = Message(1, 1, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID(dummy value)
	receivedMessage.appendEnumerated(2, 3); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count(dummy value)
	reqVerifService.successAcceptanceVerification(receivedMessage);

	receivedMessage = Message(1, 2, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID(dummy value)
	receivedMessage.appendEnumerated(2, 3); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count(dummy value)
	receivedMessage.appendEnum16(5); // error code(dummy value)
	reqVerifService.failAcceptanceVerification(receivedMessage);

	receivedMessage = Message(1, 7, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID(dummy value)
	receivedMessage.appendEnumerated(2, 3); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count(dummy value)
	reqVerifService.successExecutionVerification(receivedMessage);

	receivedMessage = Message(1, 8, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID(dummy value)
	receivedMessage.appendEnumerated(2, 3); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count(dummy value)
	receivedMessage.appendEnum16(6); // error code(dummy value)
	reqVerifService.failExecutionVerification(receivedMessage);

	receivedMessage = Message(1, 10, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID(dummy value)
	receivedMessage.appendEnumerated(2, 3); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count(dummy value)
	receivedMessage.appendEnum16(7); // error code(dummy value)
	reqVerifService.failRoutingVerification(receivedMessage);
	return 0;
}
