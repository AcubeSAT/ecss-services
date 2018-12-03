#include <iostream>
#include "Helpers/CRCHelper.hpp"
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/EventReportService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
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
	// parameters take random values and works as expected
	RequestVerificationService reqVerifService;
	reqVerifService.successAcceptanceVerification(Message::TC, true, 2, 2, 10);
	reqVerifService.failAcceptanceVerification(Message::TC, true, 2, 2, 10, 5);
	reqVerifService.successExecutionVerification(Message::TC, true, 2, 2, 10);
	reqVerifService.failExecutionVerification(Message::TC, true, 2, 2, 10, 6);
	reqVerifService.failRoutingVerification(Message::TC, true, 2, 2, 10, 7);

	// ST[05] (5,1 to 5,4) test [works]
	const unsigned char eventReportData[12] = "Hello World";
	EventReportService eventReportService;
	eventReportService.informativeEventReport(EventReportService::InformativeUnknownEvent,
	                                          eventReportData, 11);
	eventReportService.lowSeverityAnomalyReport(EventReportService::LowSeverityUnknownEvent,
	                                            eventReportData, 11);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::MediumSeverityUnknownEvent,
	                                               eventReportData, 11);
	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent,
	                                             eventReportData, 11);

	// MessageParser class test
	std::cout << "\n";
	// ST[17] test
	Message message = Message(17, 1, Message::TC, 1);
	MessageParser messageParser;
	messageParser.execute(message);
	message = Message(17, 3, Message::TC, 1);
	message.appendUint16(7);
	messageParser.execute(message);

	// ST[01] test
	message = Message(1, 1, Message::TC, 2);
	messageParser.execute(message);
	message = Message(1, 2, Message::TC, 2);
	messageParser.execute(message);
	message = Message(1, 7, Message::TC, 2);
	messageParser.execute(message);
	message = Message(1, 8, Message::TC, 2);
	messageParser.execute(message);
	message = Message(1, 10, Message::TC, 2);
	messageParser.execute(message);

	//ST[05] (5,5 to 5,8) test [works]
	EventReportService::Event eventIDs[] = {EventReportService::HighSeverityUnknownEvent,
										 EventReportService::MediumSeverityUnknownEvent};
	EventReportService::Event eventIDs2[] = {EventReportService::HighSeverityUnknownEvent};
	eventReportService.disableReportGeneration(2, eventIDs);
	eventReportService.listOfDisabledEventsReport();
	eventReportService.enableReportGeneration(1,eventIDs2);
	eventReportService.requestListOfDisabledEvents();

	return 0;
}
