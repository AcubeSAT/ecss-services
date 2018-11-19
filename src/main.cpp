#include <iostream>
#include <Services/TestService.hpp>
#include "Services/ParameterService.hpp"
#include "Message.hpp"

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

	//ST[20] test
	ParameterService paramService;
	Message sentPacket = Message(20, 1, Message::TC, 1);  //application id is a dummy number (1)
	sentPacket.appendUint16(341);  //the packet sent contains the ID of the desired parameter
	Message returnedPacket = paramService.reportParameter(sentPacket);

	uint16_t id = returnedPacket.readUint16();
	uint32_t val = returnedPacket.readUint32();

	std::cout << "Parameter ID: " << std::dec << id << std::endl << "Parameter value: " << std::dec << val << std::endl;

	return 0;
}
