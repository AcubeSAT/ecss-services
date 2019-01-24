#include <iostream>
#include "Helpers/CRCHelper.hpp"
#include "Helpers/TimeHelper.hpp"
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/TimeManagementService.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Helpers/CRCHelper.hpp"
#include "ErrorHandler.hpp"
#include "etl/String.hpp"


int main() {
	Message packet = Message(0, 0, Message::TC, 1);

	packet.appendString<5>("hello");
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
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
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
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr)); // Start address
	rcvPack.appendOctetString(String<2>(data, 2));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 2)); // Append the CRC value
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(pStr + 1)); // Start address
	rcvPack.appendOctetString(String<1>(data, 1));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 1)); // Append the CRC value
	memMangService.rawDataMemorySubservice.loadRawData(rcvPack);

	rcvPack = Message(6, 9, Message::TC, 1);

	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(data)); // Start address
	rcvPack.appendUint16(2);
	rcvPack.appendUint64(reinterpret_cast<uint64_t >(data + 1)); // Start address
	rcvPack.appendUint16(1);
	memMangService.rawDataMemorySubservice.checkRawData(rcvPack);


	// ST[01] test

	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 1, Message::TC, 3);
	reqVerifService.successAcceptanceVerification(receivedMessage);

	receivedMessage = Message(1, 2, Message::TC, 3);
	reqVerifService.failAcceptanceVerification(receivedMessage, ErrorHandler::UnknownAcceptanceError);

	receivedMessage = Message(1, 3, Message::TC, 3);
	reqVerifService.successStartExecutionVerification(receivedMessage);

	receivedMessage = Message(1, 4, Message::TC, 3);
	reqVerifService.failStartExecutionVerification(receivedMessage,
		ErrorHandler::UnknownStartExecutionError);

	receivedMessage = Message(1, 5, Message::TC, 3);
	reqVerifService.successProgressExecutionVerification(receivedMessage,
		ErrorHandler::UnknownStepID);

	receivedMessage = Message(1, 6, Message::TC, 3);
	reqVerifService.failProgressExecutionVerification(receivedMessage,
		ErrorHandler::UnknownProgressExecutionError, ErrorHandler::UnknownStepID);

	receivedMessage = Message(1, 7, Message::TC, 3);
	reqVerifService.successCompletionExecutionVerification(receivedMessage);

	receivedMessage = Message(1, 8, Message::TC, 3);
	reqVerifService.failCompletionExecutionVerification(receivedMessage,
		ErrorHandler::UnknownCompletionExecutionError);

	receivedMessage = Message(1, 10, Message::TC, 3);
	reqVerifService.failRoutingVerification(receivedMessage, ErrorHandler::UnknownRoutingError);

	// ST[05] (5,1 to 5,4) test [works]
	const char eventReportData[12] = "Hello World";
	EventReportService eventReportService;
	eventReportService.informativeEventReport(EventReportService::InformativeUnknownEvent,
	                                          eventReportData);
	eventReportService.lowSeverityAnomalyReport(EventReportService::LowSeverityUnknownEvent,
	                                            eventReportData);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::MediumSeverityUnknownEvent,
	                                               eventReportData);
	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent,
	                                             eventReportData);

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
	message = Message(1, 1, Message::TC, 3);
	//messageParser.execute(message);
	message = Message(1, 2, Message::TC, 3);
	//messageParser.execute(message);
	message = Message(1, 7, Message::TC, 3);
	//messageParser.execute(message);
	message = Message(1, 8, Message::TC, 3);
	//messageParser.execute(message);
	message = Message(1, 10, Message::TC, 3);
	//messageParser.execute(message);

	// ErrorHandler test
	std::cout << std::flush;
	std::cerr << std::flush;
	ErrorHandler::reportError(receivedPacket, ErrorHandler::MessageTooShort);
	ErrorHandler::reportInternalError(ErrorHandler::MessageTooLarge);
	Message errorMessage(0, 0, Message::TC, 1);
	errorMessage.appendBits(2, 7);
	errorMessage.appendByte(15);


	// TimeHelper test
	uint64_t test = TimeHelper::implementCUCTimeFormat(1200);
	std::cout << "\n" << test << "\n";

	// ST[09] test
	TimeManagementService timeReport;
	timeReport.cucTimeReport();

	// ST[05] (5,5 to 5,8) test [works]
	EventReportService::Event eventIDs[] = {EventReportService::HighSeverityUnknownEvent,
										 EventReportService::MediumSeverityUnknownEvent};
	EventReportService::Event eventIDs2[] = {EventReportService::HighSeverityUnknownEvent};
	Message eventMessage(5, 6, Message::TC, 1);
	eventMessage.appendUint16(2);
	eventMessage.appendEnum16(eventIDs[0]);
	eventMessage.appendEnum16(eventIDs[1]);

	Message eventMessage2(5, 5, Message::TC, 1);
	eventMessage2.appendUint16(1);
	eventMessage2.appendEnum16(eventIDs2[0]);

	Message eventMessage3(5, 7, Message::TC, 1);
	eventReportService.disableReportGeneration(eventMessage);
	eventReportService.listOfDisabledEventsReport();
	eventReportService.enableReportGeneration(eventMessage2);
	eventReportService.requestListOfDisabledEvents(eventMessage3);

	return 0;
}
