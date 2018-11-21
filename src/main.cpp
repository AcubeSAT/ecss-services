#include <iostream>
#include <Services/TestService.hpp>
#include "Message.hpp"
#include "Services/MemMangService.hpp"

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
	MemoryManagementService memMangService;
	Message rcvPack = Message(6, 2, Message::TC, 1);
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::RAM); // Memory ID
	rcvPack.appendUint16(1); // Iteration count

	// Start address
	rcvPack.appendUint64(static_cast<uint64_t >(reinterpret_cast<std::size_t >(string)));
	rcvPack.appendUint16(sizeof(string)/ sizeof(string[0])); // Data read length
	memMangService.rawDataMemorySubservice.dumpRawData(rcvPack);

	return 0;
}
