#include <iostream>
#include <Services/TestService.hpp>
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
	std::cout << "Word: " << string << " " << packet.readBits(15) << packet.readBits(1) << std::endl;
	std::cout << packet.readFloat() << " " << std::dec << packet.readSint32() << std::endl;

	// ST[17] test
	TestService testService;
	Message receivedPacket = Message(17, 1, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(17, 3, Message::TC, 1);
	receivedPacket.appendUint16(7);
	testService.onBoardConnection(receivedPacket);

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
	rcvPack.appendUint16(sizeof(string)/ sizeof(string[0])); // Data read length

	rcvPack.appendUint64(reinterpret_cast<uint64_t >(anotherStr));
	rcvPack.appendUint16(sizeof(anotherStr)/ sizeof(anotherStr[0]));

	rcvPack.appendUint64(reinterpret_cast<uint64_t >(yetAnotherStr));
	rcvPack.appendUint16(sizeof(yetAnotherStr)/ sizeof(yetAnotherStr[0]));
	memMangService.rawDataMemorySubservice.dumpRawData(rcvPack);

	rcvPack = Message(6, 2, Message::TC, 1);

	uint8_t data[2] = {'h', 'R'};
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr)); // Start address
	rcvPack.appendUint16(2); // Data length to append
	rcvPack.appendOctetString(2, data);
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr + 1)); // Start address
	rcvPack.appendUint16(1);
	rcvPack.appendOctetString(1, data);
	memMangService.rawDataMemorySubservice.loadRawData(rcvPack);

	return 0;
}
