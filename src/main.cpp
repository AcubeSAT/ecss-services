#include <iostream>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
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
	/*
	// ST[17] test
	TestService testService;
	Message receivedPacket = Message(17, 1, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(17, 3, Message::TC, 1);
	receivedPacket.appendUint16(7);
	testService.onBoardConnection(receivedPacket);
	 */

	// ST[01] test
	// parameters take random values and works as expected
	RequestVerificationService reqVerifService;
	reqVerifService.successAcceptanceVerification(0, true, 2, 2, 10);
	reqVerifService.failAcceptanceVerification(0, true, 2, 2, 10, 5);
	reqVerifService.successExecutionVerification(0, true, 2, 2, 10);
	reqVerifService.failExecutionVerification(0, true, 2, 2, 10, 6);
	reqVerifService.failRoutingVerification(0, true, 2, 2, 10, 7);

	return 0;
}
