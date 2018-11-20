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

	//ST[20] test
	ParameterService paramService;

	//Test code for reportParameter
	Message sentPacket = Message(20, 1, Message::TC, 1);  //application id is a dummy number (1)
	sentPacket.appendUint16(2);  //number of contained IDs
	sentPacket.appendUint16(0);  //first ID
	sentPacket.appendUint16(1);  //second ID
	Message returnedPacket = paramService.reportParameterIds(sentPacket);

	uint16_t numOfIds = returnedPacket.readUint16();

	std::cout << std::endl << "Number of contained configs: " << numOfIds << std::endl;

	for (int i = 0; i < numOfIds; i++) {

		std::cout << "Parameter ID: " << std::dec << returnedPacket.readUint16() << std::endl
		          << "Parameter value: " << std::dec << returnedPacket.readUint32() << std::endl;

	}

	std::cout << std::endl << "(First value is hours, second is minutes)" << std::endl;

	return 0;
}
