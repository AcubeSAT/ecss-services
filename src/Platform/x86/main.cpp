#include <Logger.hpp>
#include <Platform/x86/Helpers/UTCTimestamp.hpp>
#include <Time/UTCTimestamp.hpp>
#include <ctime>
#include <iostream>
#include <filesystem>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "Helpers/Statistic.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ServicePool.hpp"
#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FileManagementService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "etl/String.hpp"

int main() {
	LOG_NOTICE << "ECSS Services test application";

	Message packet = Message(0, 0, Message::TC, 1);

	packet.appendString(String<5>("hello"));
	packet.appendBits(15, 0x28a8);
	packet.appendBits(1, 1);
	packet.appendFloat(5.7);
	packet.appendSint32(-123456789);

	std::cout << "Hello, World!" << std::endl;
	std::cout << std::hex << packet.data.data() << std::endl; // packet data must be 'helloQQ'

	char string[6];
	packet.readCString(string, 5);
	std::cout << "Word: " << string << " " << packet.readBits(15) << packet.readBits(1) << std::endl;
	std::cout << packet.readFloat() << " " << std::dec << packet.readSint32() << std::endl;

	// ST[17] test
	TestService& testService = Services.testService;
	Message receivedPacket =
	    Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest, Message::TC, 1);
	receivedPacket.append<ApplicationProcessId>(7);
	testService.onBoardConnection(receivedPacket);

	// ST[20] test
	ParameterService& paramService = Services.parameterManagement;

	// Test code for reportParameter
	Message sentPacket = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
	                             Message::TC, 1); // application id is a dummy number (1)
	sentPacket.appendUint16(2);                   // number of contained IDs
	sentPacket.append<ParameterId>(0);            // first ID
	sentPacket.append<ParameterId>(1);            // second ID
	paramService.reportParameters(sentPacket);

	// Test code for setParameter
	Message sentPacket2 = Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues,
	                              Message::TC, 1); // application id is a dummy number (1)
	sentPacket2.appendUint16(2);                   // number of contained IDs
	sentPacket2.append<ParameterId>(0);            // first parameter ID
	sentPacket2.appendUint32(63238);               // settings for first parameter
	sentPacket2.append<ParameterId>(1);            // 2nd parameter ID
	sentPacket2.appendUint32(45823);               // settings for 2nd parameter

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
	Message rcvPack = Message(MemoryManagementService::ServiceType,
	                          MemoryManagementService::MessageType::DumpRawMemoryData, Message::TC, 1);
	rcvPack.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(3);                                               // Iteration count
	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(string));  // Start address
	rcvPack.append<MemoryDataLength>(sizeof(string) / sizeof(string[0]));  // Data read length

	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(anotherStr));
	rcvPack.append<MemoryDataLength>(sizeof(anotherStr) / sizeof(anotherStr[0]));

	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(yetAnotherStr));
	rcvPack.append<MemoryDataLength>(sizeof(yetAnotherStr) / sizeof(yetAnotherStr[0]));
	memMangService.rawDataMemorySubservice.dumpRawData(rcvPack);

	rcvPack = Message(MemoryManagementService::ServiceType,
	                  MemoryManagementService::MessageType::LoadRawMemoryDataAreas, Message::TC, 1);

	uint8_t data[2] = {'h', 'R'};
	rcvPack.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2);                                               // Iteration count
	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(pStr));    // Start address
	rcvPack.appendOctetString(String<2>(data, 2));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 2));               // Append the CRC value
	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(pStr + 1)); // Start address
	rcvPack.appendOctetString(String<1>(data, 1));
	rcvPack.appendBits(16, CRCHelper::calculateCRC(data, 1)); // Append the CRC value
	memMangService.loadRawData(rcvPack);

	rcvPack = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::CheckRawMemoryData,
	                  Message::TC, 1);

	rcvPack.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	rcvPack.appendUint16(2);                                               // Iteration count
	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(data));    // Start address
	rcvPack.append<MemoryDataLength>(2);
	rcvPack.append<StartAddress>(reinterpret_cast<StartAddress>(data + 1)); // Start address
	rcvPack.append<MemoryDataLength>(1);
	memMangService.rawDataMemorySubservice.checkRawData(rcvPack);

	// ST[01] test

	RequestVerificationService& reqVerifService = Services.requestVerification;

	Message receivedMessage =
	    Message(RequestVerificationService::ServiceType,
	            RequestVerificationService::MessageType::SuccessfulAcceptanceReport, Message::TC, 3);
	reqVerifService.successAcceptanceVerification(receivedMessage);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::FailedAcceptanceReport, Message::TC, 3);
	reqVerifService.failAcceptanceVerification(receivedMessage, ErrorHandler::UnknownAcceptanceError);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::SuccessfulStartOfExecution, Message::TC, 3);
	reqVerifService.successStartExecutionVerification(receivedMessage);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::FailedStartOfExecution, Message::TC, 3);
	reqVerifService.failStartExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionStartError);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::SuccessfulProgressOfExecution, Message::TC, 3);
	reqVerifService.successProgressExecutionVerification(receivedMessage, 0);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::FailedProgressOfExecution, Message::TC, 3);
	reqVerifService.failProgressExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionProgressError, 0);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::SuccessfulCompletionOfExecution, Message::TC, 3);
	reqVerifService.successCompletionExecutionVerification(receivedMessage);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::FailedCompletionOfExecution, Message::TC, 3);
	reqVerifService.failCompletionExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionCompletionError);

	receivedMessage = Message(RequestVerificationService::ServiceType,
	                          RequestVerificationService::MessageType::FailedRoutingReport, Message::TC, 3);
	reqVerifService.failRoutingVerification(receivedMessage, ErrorHandler::UnknownRoutingError);

	// ST[05] (5,1 to 5,4) test [works]
	constexpr char eventReportData[12] = "Hello World";
	EventReportService eventReportService;
	eventReportService.informativeEventReport(EventReportService::UnknownEvent, eventReportData);
	eventReportService.lowSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	eventReportService.mediumSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);
	eventReportService.highSeverityAnomalyReport(EventReportService::UnknownEvent, eventReportData);

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

	// ST[05] (5,5 to 5,8) test [works]
	EventReportService::Event eventIDs[] = {EventReportService::UnknownEvent,};
	EventReportService::Event eventIDs2[] = {EventReportService::UnknownEvent};
	Message eventMessage(EventReportService::ServiceType,
	                     EventReportService::MessageType::DisableReportGenerationOfEvents, Message::TC, 1);
	eventMessage.appendUint16(2);
	eventMessage.append<EventDefinitionId>(eventIDs[0]);
	eventMessage.append<EventDefinitionId>(eventIDs[1]);

	Message eventMessage2(EventReportService::ServiceType,
	                      EventReportService::MessageType::EnableReportGenerationOfEvents, Message::TC, 1);
	eventMessage2.appendUint16(1);
	eventMessage2.append<EventDefinitionId>(eventIDs2[0]);

	Message eventMessage3(EventReportService::ServiceType, EventReportService::MessageType::ReportListOfDisabledEvents,
	                      Message::TC, 1);
	eventReportService.disableReportGeneration(eventMessage);
	eventReportService.listOfDisabledEventsReport();
	eventReportService.enableReportGeneration(eventMessage2);
	eventReportService.requestListOfDisabledEvents(eventMessage3);

	// ST[19] test

	EventActionService& eventActionService = Services.eventAction;

	Message eventActionDefinition(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction,
	                              Message::TC, 1);
	eventActionDefinition.appendEnum16(0);
	eventActionDefinition.append<ApplicationProcessId>(2);
	eventActionDefinition.append<EventDefinitionId>(1);
	String<64> TCdata = "0123456789123456789123456789123456789123456789123456789123456789";
	eventActionDefinition.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition);

	Message eventActionDefinition1(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction,
	                               Message::TC, 1);
	eventActionDefinition1.appendEnum16(0);
	eventActionDefinition1.append<ApplicationProcessId>(2);
	eventActionDefinition1.append<EventDefinitionId>(1);
	TCdata = "hi1";
	eventActionDefinition1.appendString(TCdata);
	std::cout << "After this message there should be a failed start of execution error \n";
	eventActionService.addEventActionDefinitions(eventActionDefinition1);

	Message eventActionDefinition2(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction,
	                               Message::TC, 1);
	eventActionDefinition2.appendEnum16(0);
	eventActionDefinition2.append<ApplicationProcessId>(4);
	eventActionDefinition2.append<EventDefinitionId>(2);
	TCdata = "hi2";
	eventActionDefinition2.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition2);

	Message eventActionDefinition7(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction,
	                               Message::TC, 1);
	eventActionDefinition7.appendEnum16(0);
	eventActionDefinition7.append<ApplicationProcessId>(4);
	eventActionDefinition7.append<EventDefinitionId>(4);
	TCdata = "hi2";
	eventActionDefinition7.appendString(TCdata);
	eventActionService.addEventActionDefinitions(eventActionDefinition7);

	std::cout << "Status should be 000:";
	for (auto& element: eventActionService.eventActionDefinitionMap) {
		std::cout << element.second.enabled;
	}

	Message eventActionDefinition5(EventActionService::ServiceType, EventActionService::MessageType::EnableEventAction,
	                               Message::TC, 1);
	eventActionDefinition5.appendUint16(3);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.append<ApplicationProcessId>(2);
	eventActionDefinition5.append<EventDefinitionId>(1);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.append<ApplicationProcessId>(4);
	eventActionDefinition5.append<EventDefinitionId>(2);
	eventActionDefinition5.appendUint16(0);
	eventActionDefinition5.append<ApplicationProcessId>(4);
	eventActionDefinition5.append<EventDefinitionId>(4);

	eventActionService.enableEventActionDefinitions(eventActionDefinition5);
	std::cout << "\nStatus should be 111:";
	for (auto& element: eventActionService.eventActionDefinitionMap) {
		std::cout << element.second.enabled;
	}

	Message eventActionDefinition3(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction,
	                               Message::TC, 1);
	eventActionDefinition3.appendUint16(3);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.append<ApplicationProcessId>(2);
	eventActionDefinition3.append<EventDefinitionId>(1);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.append<ApplicationProcessId>(4);
	eventActionDefinition3.append<EventDefinitionId>(2);
	eventActionDefinition3.appendUint16(0);
	eventActionDefinition3.append<ApplicationProcessId>(4);
	eventActionDefinition3.append<EventDefinitionId>(4);
	eventActionService.disableEventActionDefinitions(eventActionDefinition3);
	std::cout << "Status should be 000:";
	for (auto& element: eventActionService.eventActionDefinitionMap) {
		std::cout << element.second.enabled;
	}

	eventActionService.enableEventActionDefinitions(eventActionDefinition5);

	Message eventActionDefinition4(EventActionService::ServiceType, EventActionService::MessageType::DeleteEventAction,
	                               Message::TC, 1);
	eventActionDefinition4.appendUint16(1);
	eventActionDefinition4.appendUint16(0);
	eventActionDefinition4.append<ApplicationProcessId>(2);
	eventActionDefinition4.append<EventDefinitionId>(1);

	std::cout << "After this message there should be a failed start of execution error \n";
	eventActionService.deleteEventActionDefinitions(eventActionDefinition4);

	Message eventActionDefinition6(EventActionService::ServiceType, EventActionService::MessageType::DisableEventAction,
	                               Message::TC, 1);
	eventActionDefinition6.appendUint16(1);
	eventActionDefinition6.appendUint16(0);
	eventActionDefinition6.append<ApplicationProcessId>(2);
	eventActionDefinition6.append<EventDefinitionId>(1);
	eventActionService.disableEventActionDefinitions(eventActionDefinition6);
	std::cout << "After this message there should NOT be a failed start of execution error \n";
	eventActionService.deleteEventActionDefinitions(eventActionDefinition4);

	// ST13 test

	LargePacketTransferService largePacketTransferService;
	String<256> dataToTransfer = "12345678";
	largePacketTransferService.firstDownlinkPartReport(LargePacketTransferService::ServiceType,
	                                                   LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                                   dataToTransfer);

	// ST[11] test
	TimeBasedSchedulingService timeBasedSchedulingService;
	Time::DefaultCUC currentTime(time(nullptr)); // Get the current system time
	std::cout << "\n\nST[11] service is running";
	std::cout << "\nCurrent time in seconds (UNIX epoch): " << currentTime.asTAIseconds() << std::endl;

	Message receivedMsg =
	    Message(TimeBasedSchedulingService::ServiceType,
	            TimeBasedSchedulingService::MessageType::EnableTimeBasedScheduleExecutionFunction, Message::TC, 1);
	Message testMessage1(6, 5, Message::TC, 1);
	Message testMessage2(4, 5, Message::TC, 1);
	testMessage1.appendUint16(4253);  // Append dummy data
	testMessage2.appendUint16(45667); // Append dummy data

	timeBasedSchedulingService.enableScheduleExecution(receivedMsg); // Enable the schedule

	// Insert activities in the schedule
	receivedMsg = Message(TimeBasedSchedulingService::ServiceType,
	                      TimeBasedSchedulingService::MessageType::InsertActivities, Message::TC, 1);
	receivedMsg.appendUint16(2); // Total number of requests

	receivedMsg.append<Time::DefaultCUC>(Time::DefaultCUC(currentTime.asTAIseconds() + 1556435U));
	receivedMsg.appendString(MessageParser::composeECSS(testMessage1));

	receivedMsg.append<Time::DefaultCUC>(Time::DefaultCUC(currentTime.asTAIseconds() + 1957232U));
	receivedMsg.appendString(MessageParser::composeECSS(testMessage2));
	timeBasedSchedulingService.insertActivities(receivedMsg);

	// Time shift activities
	receivedMsg = Message(TimeBasedSchedulingService::ServiceType,
	                      TimeBasedSchedulingService::MessageType::TimeShiftALlScheduledActivities, Message::TC, 1);
	receivedMsg.appendSint32(-6789);
	timeBasedSchedulingService.timeShiftAllActivities(receivedMsg);
	std::cout << "Activities should be time shifted by: " << -6789 << " seconds." << std::endl;

	// Report the activities
	receivedMsg = Message(TimeBasedSchedulingService::ServiceType,
	                      TimeBasedSchedulingService::MessageType::DetailReportAllScheduledActivities, Message::TC, 1);
	timeBasedSchedulingService.detailReportAllActivities(receivedMsg);

	// Report the activities by ID
	receivedMsg = Message(TimeBasedSchedulingService::ServiceType,
	                      TimeBasedSchedulingService::MessageType::ActivitiesSummaryReportById, Message::TC, 1);
	timeBasedSchedulingService.summaryReportActivitiesByID(receivedMsg);

	//ST[23]
	namespace fs = std::filesystem;
	FileManagementService& fileManagementService = Services.fileManagement;

	fs::current_path(fs::temp_directory_path());
	std::cout << "\n\nST[23] File System Service - Start\n\n";
	fs::create_directories("st23");
	String<ECSSTCRequestStringSize> repo = "st23";

	//Create directory
	Message createDirectoryMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC, 0);
	String<ECSSTCRequestStringSize> directory = "created_directory";
	createDirectoryMessage.appendOctetString(repo);
	createDirectoryMessage.appendOctetString(directory);
	MessageParser::execute(createDirectoryMessage);
	std::cout << "Created directory.\n";
	std::cout << "fs::exists(\"st23/created_directory\") -> " << fs::exists("st23/created_directory") << "\n";
	std::cout << "fs::is_directory(\"st23/created_directory\") -> " << fs::is_directory("st23/created_directory") << "\n\n";

	String<ECSSTCRequestStringSize> fullPathToDirectory = repo;
	fullPathToDirectory.append("/").append(directory);

	//Create file
	Message createFileMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
	String<ECSSTCRequestStringSize> file = "created_file";
	createFileMessage.appendOctetString(fullPathToDirectory);
	createFileMessage.appendOctetString(file);
	MessageParser::execute(createFileMessage);
	std::cout << "Created file.\n";
	std::cout << "fs::exists(\"st23/created_directory/created_file\") -> " << fs::exists("st23/created_directory/created_file") << "\n";
	std::cout << "fs::is_regular_file(\"st23/created_directory/created_file\") -> " << fs::is_regular_file("st23/created_directory/created_file") << "\n\n";

	//Lock file
	Message lockFileMessage(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
	lockFileMessage.appendOctetString(fullPathToDirectory);
	lockFileMessage.appendOctetString(file);
	MessageParser::execute(lockFileMessage);
	std::cout << "Locked file.\n";
	Filesystem::FileLockStatus lockStatus = Filesystem::getFileLockStatus("st23/created_directory/created_file");
	std::cout << "FileSystem::getFileLockStatus == Locked -> " << (lockStatus == Filesystem::FileLockStatus::Locked) << "\n\n";

	//Unlock file
	Message unlockFileMessage(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
	unlockFileMessage.appendOctetString(fullPathToDirectory);
	unlockFileMessage.appendOctetString(file);
	MessageParser::execute(unlockFileMessage);
	lockStatus = Filesystem::getFileLockStatus("st23/created_directory/created_file");
	std::cout << "Unlocked file.\n";
	std::cout << "FileSystem::getFileLockStatus == Unlocked -> " << (lockStatus == Filesystem::FileLockStatus::Unlocked) <<"\n\n";

	//Delete file
	Message deleteFileMessage(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
	deleteFileMessage.appendOctetString(fullPathToDirectory);
	deleteFileMessage.appendOctetString(file);
	MessageParser::execute(deleteFileMessage);
	std::cout << "Deleted file.\n";
	std::cout << "fs::exists(\"st23/created_directory/created_file\") -> " << fs::exists("st23/created_directory/created_file") << "\n";
	std::cout << "fs::is_regular_file(\"st23/created_directory/created_file\") -> " << fs::is_regular_file("st23/created_directory/created_file") << "\n\n";

	//Delete directory
	Message deleteDirectoryMessage(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,0);
	deleteDirectoryMessage.appendOctetString(repo);
	deleteDirectoryMessage.appendOctetString(directory);
	MessageParser::execute(deleteDirectoryMessage);
	std::cout << "Deleted directory.\n";
	std::cout << "fs::exists(\"st23/created_directory\") -> " << fs::exists("st23/created_directory") << "\n";
	std::cout << "fs::is_directory(\"st23/created_directory\") -> " << fs::is_directory("st23/created_directory") << "\n\n";

	//cleanup
	fs::remove_all(fs::temp_directory_path() / "st23");
	std::cout << "ST[23] File System Service - End\n\n";
	//ST[23] end

	LOG_NOTICE << "ECSS Services test complete";

	ErrorHandler::reportInternalError(static_cast<ErrorHandler::InternalErrorType>(254));

	std::cout << UTCTimestamp() << std::endl;
	return 0;
}
