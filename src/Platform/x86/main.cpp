#include <iostream>
#include <ServicePool.hpp>
#include <Logger.hpp>
#include "Helpers/CRCHelper.hpp"
#include "Helpers/TimeHelper.hpp"
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/TimeManagementService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "ServicePool.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "Helpers/CRCHelper.hpp"
#include "ErrorHandler.hpp"
#include "etl/String.hpp"
#include "ECSS_ST_Definitions.hpp"

int main() {
	LOG_NOTICE << "ECSS Services test application";

	Message packet = Message(0, 0, Message::TC, 1);

	packet.appendString(String<5>("hello"));
	packet.appendBits(15, 0x28a8);
	packet.appendBits(1, 1);
	packet.appendFloat(5.7);
	packet.appendSint32(-123456789);

	std::cout << "Hello, World!" << std::endl;
	std::cout << std::hex << packet.data << std::endl; // packet data must be 'helloQQ'

	char string[6];
	packet.readCString(string, 5);
	std::cout << "Word: " << string << " " << packet.readBits(15) << packet.readBits(1) << std::endl;
	std::cout << packet.readFloat() << " " << std::dec << packet.readSint32() << std::endl;

	// ST[17] test
	TestService& testService = Services.testService;
	Message receivedPacket = Message(Test, AreYouAliveTest, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(Test, OnBoardConnectionTest, Message::TC, 1);
	receivedPacket.appendUint16(7);
	testService.onBoardConnection(receivedPacket);

	// ST[20] test
	ParameterService& paramService = Services.parameterManagement;

	// Test code for reportParameter
	Message sentPacket = Message(ParameterManagement, ReportParameterValues, Message::TC, 1); // application id is a dummy number (1)
	sentPacket.appendUint16(2); // number of contained IDs
	sentPacket.appendUint16(0); // first ID
	sentPacket.appendUint16(1); // second ID
	paramService.reportParameters(sentPacket);

	// Test code for setParameter
	Message sentPacket2 = Message(ParameterManagement, SetParameterValues, Message::TC, 1); // application id is a dummy number (1)
	sentPacket2.appendUint16(2); // number of contained IDs
	sentPacket2.appendUint16(0); // first parameter ID
	sentPacket2.appendUint32(63238); // settings for first parameter
	sentPacket2.appendUint16(1); // 2nd parameter ID
	sentPacket2.appendUint32(45823); // settings for 2nd parameter

	paramService.setParameters(sentPacket2);
	paramService.reportParameters(sentPacket);

	// ST[06] testing
	char anotherStr[8] = "Fgthred";
	char yetAnotherStr[2] = "F";
	char pStr[4];
	*pStr = 'T';
	*(pStr + 1) = 'G';
	*(pStr + 2) = 0;

	MemoryManagementService& memMangService = Services.memoryManagement;
	Message rcvPack = Message(MemoryManagement, DumpRawMemoryData, Message::TC, 1);
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(3); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t>(string)); // Start address
	rcvPack.appendUint16(sizeof(string) / sizeof(string[0])); // Data read length

	rcvPack.appendUint64(reinterpret_cast<uint64_t>(anotherStr));
	rcvPack.appendUint16(sizeof(anotherStr) / sizeof(anotherStr[0]));

	rcvPack.appendUint64(reinterpret_cast<uint64_t>(yetAnotherStr));
	rcvPack.appendUint16(sizeof(yetAnotherStr) / sizeof(yetAnotherStr[0]));
	memMangService.rawDataMemorySubservice.dumpRawData(rcvPack);

	rcvPack = Message(MemoryManagement, LoadRawMemoryDataAreas, Message::TC, 1);

	uint8_t data[2] = {'h', 'R'};
	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t>(pStr)); // Start address
	rcvPack.appendOctetString(String<2>(data, 2));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 2)); // Append the CRC value
	rcvPack.appendUint64(reinterpret_cast<uint64_t>(pStr + 1)); // Start address
	rcvPack.appendOctetString(String<1>(data, 1));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 1)); // Append the CRC value
	memMangService.rawDataMemorySubservice.loadRawData(rcvPack);

	rcvPack = Message(MemoryManagement, CheckRawMemoryData, Message::TC, 1);

	rcvPack.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2); // Iteration count
	rcvPack.appendUint64(reinterpret_cast<uint64_t>(data)); // Start address
	rcvPack.appendUint16(2);
	rcvPack.appendUint64(reinterpret_cast<uint64_t>(data + 1)); // Start address
	rcvPack.appendUint16(1);
	memMangService.rawDataMemorySubservice.checkRawData(rcvPack);

	// ST[01] test

	RequestVerificationService& reqVerifService = Services.requestVerification;

	Message receivedMessage = Message(RequestVerification, SuccessfulAcceptanceReport, Message::TC, 3);
	reqVerifService.successAcceptanceVerification(receivedMessage);

	receivedMessage = Message(RequestVerification, FailedAcceptanceReport, Message::TC, 3);
	reqVerifService.failAcceptanceVerification(receivedMessage, ErrorHandler::UnknownAcceptanceError);

	receivedMessage = Message(RequestVerification, SuccessfulStartOfExecution, Message::TC, 3);
	reqVerifService.successStartExecutionVerification(receivedMessage);

	receivedMessage = Message(RequestVerification, FailedStartOfExecution, Message::TC, 3);
	reqVerifService.failStartExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionStartError);

	receivedMessage = Message(RequestVerification, SuccessfulProgressOfExecution, Message::TC, 3);
	reqVerifService.successProgressExecutionVerification(receivedMessage, 0);

	receivedMessage = Message(RequestVerification, FailedProgressOfExecution, Message::TC, 3);
	reqVerifService.failProgressExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionProgressError, 0);

	receivedMessage = Message(RequestVerification, SuccessfulCompletionOfExecution, Message::TC, 3);
	reqVerifService.successCompletionExecutionVerification(receivedMessage);

	receivedMessage = Message(RequestVerification, FailedCompletionOfExecution, Message::TC, 3);
	reqVerifService.failCompletionExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionCompletionError);

	receivedMessage = Message(RequestVerification, FailedRoutingReport, Message::TC, 3);
	reqVerifService.failRoutingVerification(receivedMessage, ErrorHandler::UnknownRoutingError);

	// ST[05] (5,1 to 5,4) test [works]
	const char eventReportData[12] = "Hello World";
	EventReportService eventReportService;
	eventReportService.informativeEventReport(EventReportService::InformativeUnknownEvent, eventReportData);
	eventReportService.lowSeverityAnomalyReport(EventReportService::LowSeverityUnknownEvent, eventReportData);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::MediumSeverityUnknownEvent, eventReportData);
	eventReportService.highSeverityAnomalyReport(EventReportService::HighSeverityUnknownEvent, eventReportData);

	Message trivialMessage = Message();
	MessageParser::execute(trivialMessage);

	// ErrorHandler test
	std::cout << std::flush;
	std::cerr << std::flush;
	ErrorHandler::reportError(receivedPacket, ErrorHandler::MessageTooShort);
	ErrorHandler::reportInternalError(ErrorHandler::MessageTooLarge);
	Message errorMessage(0, 0, Message::TC, 1);
	errorMessage.appendBits(2, 7);
	errorMessage.appendByte(15);

	// ST[09] test
	TimeManagementService& timeReport = Services.timeManagement;
	TimeAndDate timeInfo;
	// 10/04/1998 10:15:00
	timeInfo.year = 1998;
	timeInfo.month = 4;
	timeInfo.day = 10;
	timeInfo.hour = 10;
	timeInfo.minute = 15;
	timeInfo.second = 0;
	timeReport.cdsTimeReport(timeInfo);

	// ST[05] (5,5 to 5,8) test [works]
	EventReportService::Event eventIDs[] = {EventReportService::HighSeverityUnknownEvent,
	                                        EventReportService::MediumSeverityUnknownEvent};
	EventReportService::Event eventIDs2[] = {EventReportService::HighSeverityUnknownEvent};
	Message eventMessage(EventReport, DisableReportGenerationOfEvents, Message::TC, 1);
	eventMessage.appendUint16(2);
	eventMessage.appendEnum16(eventIDs[0]);
	eventMessage.appendEnum16(eventIDs[1]);

	Message eventMessage2(EventReport, EnableReportGenerationOfEvents, Message::TC, 1);
	eventMessage2.appendUint16(1);
	eventMessage2.appendEnum16(eventIDs2[0]);

	Message eventMessage3(EventReport, ReportListOfDisabledEvent, Message::TC, 1);
	eventReportService.disableReportGeneration(eventMessage);
	eventReportService.listOfDisabledEventsReport();
	eventReportService.enableReportGeneration(eventMessage2);
	eventReportService.requestListOfDisabledEvents(eventMessage3);

	// ST[19] test

	EventActionService & eventActionService = Services.eventAction;

	Message eventActionDefinition(EventAction, AddEventAction, Message::TC, 1);
	eventActionDefinition.appendEnum16(0);
	eventActionDefinition.appendEnum16(2);
	eventActionDefinition.appendEnum16(1);
	String<64> TCdata = "0123456789123456789123456789123456789123456789123456789123456789";
	eventActionDefinition.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition);

	Message eventActionDefinition1(EventAction, AddEventAction, Message::TC, 1);
	eventActionDefinition1.appendEnum16(0);
	eventActionDefinition1.appendEnum16(2);
	eventActionDefinition1.appendEnum16(1);
	TCdata = "hi1";
	eventActionDefinition1.appendString(TCdata);
	std::cout << "After this message there should be a failed start of execution error \n";
	eventActionService.addEventActionDefinitions(eventActionDefinition1);

	Message eventActionDefinition2(EventAction, AddEventAction, Message::TC, 1);
	eventActionDefinition2.appendEnum16(0);
	eventActionDefinition2.appendEnum16(4);
	eventActionDefinition2.appendEnum16(2);
	TCdata = "hi2";
	eventActionDefinition2.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition2);

	Message eventActionDefinition7(EventAction, AddEventAction, Message::TC, 1);
	eventActionDefinition7.appendEnum16(0);
	eventActionDefinition7.appendEnum16(4);
	eventActionDefinition7.appendEnum16(4);
	TCdata = "hi2";
	eventActionDefinition7.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition7);

	std::cout << "Status should be 000:";
	for (auto& element : eventActionService.eventActionDefinitionMap){
		std::cout << element.second.enabled;
	}

	Message eventActionDefinition5(EventAction, EnableEventAction, Message::TC, 1);
	eventActionDefinition5.appendUint16(3);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.appendUint16(2);
	eventActionDefinition5.appendUint16(1);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.appendUint16(4);
	eventActionDefinition5.appendUint16(2);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.appendUint16(4);
	eventActionDefinition5.appendUint16(4);

	eventActionService.enableEventActionDefinitions(eventActionDefinition5);
	std::cout << "\nStatus should be 111:";
	for (auto& element : eventActionService.eventActionDefinitionMap){
		std::cout << element.second.enabled;
	}

	Message eventActionDefinition3(EventAction, DisableEventAction, Message::TC, 1);
	eventActionDefinition3.appendUint16(3);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.appendUint16(2);
	eventActionDefinition3.appendUint16(1);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.appendUint16(4);
	eventActionDefinition3.appendUint16(2);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.appendUint16(4);
	eventActionDefinition3.appendUint16(4);
	eventActionService.disableEventActionDefinitions(eventActionDefinition3);
	std::cout << "Status should be 000:";
	for (auto& element : eventActionService.eventActionDefinitionMap){
		std::cout << element.second.enabled;
	}

	eventActionService.enableEventActionDefinitions(eventActionDefinition5);

	Message eventActionDefinition4(EventAction, DeleteEventAction, Message::TC, 1);
	eventActionDefinition4.appendUint16(1);
	eventActionDefinition4.appendUint16(0);
	eventActionDefinition4.appendUint16(2);
	eventActionDefinition4.appendUint16(1);

	std::cout << "After this message there should be a failed start of execution error \n";
	eventActionService.deleteEventActionDefinitions(eventActionDefinition4);

	Message eventActionDefinition6(EventAction, DisableEventAction, Message::TC, 1);
	eventActionDefinition6.appendUint16(1);
	eventActionDefinition6.appendUint16(0);
	eventActionDefinition6.appendUint16(2);
	eventActionDefinition6.appendUint16(1);
	eventActionService.disableEventActionDefinitions(eventActionDefinition6);
	std::cout << "After this message there should NOT be a failed start of execution error \n";
	eventActionService.deleteEventActionDefinitions(eventActionDefinition4);


	// ST13 test

	LargePacketTransferService largePacketTransferService;
	String<256> dataToTransfer = "12345678";
	largePacketTransferService.firstDownlinkPartReport(1, 1, dataToTransfer);

	// ST[11] test
	TimeBasedSchedulingService timeBasedSchedulingService;
	auto currentTime = static_cast<uint32_t>(time(nullptr)); // Get the current system time
	std::cout << "\n\nST[11] service is running";
	std::cout << "\nCurrent time in seconds (UNIX epoch): " << currentTime << std::endl;

	Message receivedMsg = Message(TimeBasedScheduling, EnableTimeBasedScheduleExecutionFunction, Message::TC, 1);
	Message testMessage1(MemoryManagement, DumpRawMemoryData, Message::TC, 1);
	Message testMessage2(4, 5, Message::TC, 1);
	testMessage1.appendUint16(4253); // Append dummy data
	testMessage2.appendUint16(45667); // Append dummy data

	timeBasedSchedulingService.enableScheduleExecution(receivedMsg); // Enable the schedule

	// Insert activities in the schedule
	receivedMsg = Message(TimeBasedScheduling, InsertActivities, Message::TC, 1);
	receivedMsg.appendUint16(2); // Total number of requests

	receivedMsg.appendUint32(currentTime + 1556435U);
	receivedMsg.appendString(MessageParser::composeECSS(testMessage1));

	receivedMsg.appendUint32(currentTime + 1957232U);
	receivedMsg.appendString(MessageParser::composeECSS(testMessage2));
	timeBasedSchedulingService.insertActivities(receivedMsg);

	// Time shift activities
	receivedMsg = Message(TimeBasedScheduling, TimeShiftAllScheduledActivities, Message::TC, 1);
	receivedMsg.appendSint32(-6789);
	timeBasedSchedulingService.timeShiftAllActivities(receivedMsg);
	std::cout << "Activities should be time shifted by: " << -6789 << " seconds." << std::endl;

	// Report the activities
	receivedMsg = Message(TimeBasedScheduling, DetailReportAllScheduledActivities, Message::TC, 1);
	timeBasedSchedulingService.detailReportAllActivities(receivedMsg);

	// Report the activities by ID
	receivedMsg = Message(TimeBasedScheduling, ActivitiesSummaryReportByIdentifier, Message::TC, 1);
	timeBasedSchedulingService.summaryReportActivitiesByID(receivedMsg);

	LOG_NOTICE << "ECSS Services test complete";
	return 0;
}
