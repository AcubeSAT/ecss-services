#include <iostream>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Services/EventReportService.hpp>
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

	// ST[01] test
	// parameters take random values and works as expected
	RequestVerificationService reqVerifService;
	reqVerifService.successAcceptanceVerification(Message::TC, true, 2, 2, 10);
	reqVerifService.failAcceptanceVerification(Message::TC, true, 2, 2, 10, 5);
	reqVerifService.successExecutionVerification(Message::TC, true, 2, 2, 10);
	reqVerifService.failExecutionVerification(Message::TC, true, 2, 2, 10, 6);
	reqVerifService.failRoutingVerification(Message::TC, true, 2, 2, 10, 7);

	// ST[05] test [works]
	char data[12] = "Hello World";
	EventReportService eventReportService;
	eventReportService.informativeEventReport(1,(const uint8_t *)data,11);
	eventReportService.lowSeverityAnomalyReport(2,(const uint8_t *)data,11);
	eventReportService.mediumSeverityAnomalyReport(3,(const uint8_t *)data,11);
	eventReportService.highSeverityAnomalyReport(4,(const uint8_t *)data,11);

	return 0;
}
