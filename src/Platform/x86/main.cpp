#include <iostream>
#include <Logger.hpp>
#include <Time/UTCTimestamp.hpp>
#include <Platform/x86/Helpers/UTCTimestamp.hpp>
#include "Helpers/CRCHelper.hpp"
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Helpers/Statistic.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ErrorHandler.hpp"
#include "etl/String.hpp"
#include "ServicePool.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
HousekeepingService& housekeepingService = Services.housekeeping;
ParameterService parameterManagement;
void storeSamplesToParametersYamcs(uint16_t id1, uint16_t id2, uint16_t id3, uint16_t id4, uint16_t id5, uint16_t id6, uint16_t id7, uint16_t id8, uint16_t id9, uint16_t id10, uint16_t id11) {
	//	Message samples(HousekeepingService::ServiceType,
	//	                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TM, 1);

	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id3)->get()).setValue(99);
	static_cast<Parameter<uint32_t>&>(parameterManagement.getParameter(id4)->get()).setValue(22);
	static_cast<Parameter<uint32_t>&>(parameterManagement.getParameter(id5)->get()).setValue(43);
	static_cast<Parameter<uint32_t>&>(parameterManagement.getParameter(id6)->get()).setValue(65);

	static_cast<Parameter<float_t>&>(parameterManagement.getParameter(id7)->get()).setValue(2.5);
	static_cast<Parameter<float_t>&>(parameterManagement.getParameter(id8)->get()).setValue(3.89);
	static_cast<Parameter<float_t>&>(parameterManagement.getParameter(id9)->get()).setValue(4.345);
	static_cast<Parameter<float_t>&>(parameterManagement.getParameter(id10)->get()).setValue(5.4);
	static_cast<Parameter<uint32_t>&>(parameterManagement.getParameter(id11)->get()).setValue(100);



}

void storeSamplesToParameters(uint16_t id1, uint16_t id2, uint16_t id3) {
	static_cast<Parameter<uint16_t>&>(Services.parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(Services.parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint32_t>&>(Services.parameterManagement.getParameter(id3)->get()).setValue(99);
}

void initializeHousekeepingStructuresyamcs() {
	uint8_t ids[1] = {3};
	uint32_t interval = 7;
	etl::vector<uint16_t, 6> simplyCommutatedIds = {1013, 1014, 1015, 1043, 1044, 1045};

	HousekeepingStructure structures[1];
	int i = 0;
	for (auto& newStructure: structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (uint16_t parameterId: simplyCommutatedIds) {
			newStructure.simplyCommutatedParameterIds.push_back(parameterId);
		}
		housekeepingService.housekeepingStructures.insert({ids[i], newStructure});
		i++;
	}
}

void initializeHousekeepingStructures() {
	uint8_t ids[3] = {0, 4, 6};
	uint32_t interval = 7;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {0, 1, 2};

	HousekeepingStructure structures[3];
	int i = 0;
	for (auto& newStructure: structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (uint16_t parameterId: simplyCommutatedIds) {
			newStructure.simplyCommutatedParameterIds.push_back(parameterId);
		}
		housekeepingService.housekeepingStructures.insert({ids[i], newStructure});
		i++;
	}
}

void initializeStatistics(uint16_t interval1, uint16_t interval2) {
	Statistic stat1;
	Statistic stat2;
	stat1.selfSamplingInterval = interval1;
	stat2.selfSamplingInterval = interval2;
	uint16_t id1 = 5016;
	uint16_t id2 = 5017;

	int numOfSamples = 3;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-1: [ 1, 3, 5 ]
		stat1.updateStatistics(i * 2 + 1);
	}
	numOfSamples = 6;
	for (int i = 0; i < numOfSamples; i++) { // Values of stat-2: [ 3, 5, 7, 9, 11, 13 ]
		stat2.updateStatistics(i * 2 + 3);
	}
	Services.parameterStatistics.statisticsMap.insert({id1, stat1});
	Services.parameterStatistics.statisticsMap.insert({id2, stat2});
}


int main() {
	storeSamplesToParameters(0, 1, 2);
	initializeHousekeepingStructures();

	sleep(5);

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
	Message receivedPacket =
	    Message(TestService::ServiceType, TestService::MessageType::AreYouAliveTest, Message::TC, 1);
	testService.areYouAlive(receivedPacket);
	receivedPacket = Message(TestService::ServiceType, TestService::MessageType::OnBoardConnectionTest, Message::TC, 1);
	receivedPacket.appendUint16(7);
	testService.onBoardConnection(receivedPacket);

	sleep(5);

	// ST[03] test
	//[3,25] Housekeeping_ADCS_0.01
	uint8_t structId = 6;

	housekeepingService.housekeepingParametersReport(structId);
	sleep(5);

	// ST[20] test
	ParameterService& paramService = Services.parameterManagement;


	// Test code for reportParameter
	Message sentPacket = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
	                            Message::TC, 1); // application id is a dummy number (1)
	sentPacket.appendUint16(4);                   // number of contained IDs
	sentPacket.appendUint16(5000);                   // first ID
	sentPacket.appendUint16(5001);                   // second ID
	sentPacket.appendUint16(1092);                   // third ID
	sentPacket.appendUint16(1093);                   // forth ID
	paramService.reportParameters(sentPacket);

	 Message sentPackettime = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
								 Message::TC, 1); // application id is a dummy number (1)
	 sentPackettime.appendUint16(1);                   // number of contained IDs
	 sentPackettime.appendUint16(5010);                   // first ID
	 paramService.reportParameters(sentPackettime);

	sleep(5);

	// Test code for setParameter
	//Message sentPacket2 = Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues,
	                         //    Message::TC, 1); // application id is a dummy number (1)
	//sentPacket2.appendUint16(4);                   // number of contained IDs
	//sentPacket2.appendUint16(5000);                   // first parameter ID
	//sentPacket2.appendFloat(63238);               // settings for first parameter
	//sentPacket2.appendUint16(5001);                   // 2nd parameter ID
	//sentPacket2.appendFloat(45823);               // settings for 2nd parameter
	//sentPacket2.appendUint16(1092);                   // 3rd parameter ID
	//sentPacket2.appendFloat(65);               // settings for 3rd parameter
	//sentPacket2.appendUint16(1093);                   // 4th parameter ID
	//sentPacket2.appendFloat(325);               // settings for 4th parameter

	//paramService.setParameters(sentPacket2);
	//paramService.reportParameters(sentPacket);

	//sleep(5);

	//ST[04] test

	initializeStatistics(6, 7);
	Message request = Message(ParameterStatisticsService::ServiceType,
	                          ParameterStatisticsService::MessageType::ReportParameterStatistics, Message::TC, 1);
	Services.parameterStatistics.hasAutomaticStatisticsReset = false;

	MessageParser::execute(request);



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

	sleep(5);


	// ErrorHandler test
	std::cout << std::flush;
	std::cerr << std::flush;
	ErrorHandler::reportError(receivedPacket, ErrorHandler::MessageTooShort);
	ErrorHandler::reportInternalError(ErrorHandler::MessageTooLarge);
	Message errorMessage(0, 0, Message::TC, 1);
	errorMessage.appendBits(2, 7);
	errorMessage.appendByte(15);

	sleep(5);


	// ST[11] test
	TimeBasedSchedulingService timeBasedSchedulingService;
	auto currentTime = static_cast<uint32_t>(time(nullptr)); // Get the current system time
	std::cout << "\n\nST[11] service is running";
	std::cout << "\nCurrent time in seconds (UNIX epoch): " << currentTime << std::endl;

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

	receivedMsg.appendUint32(currentTime + 1556435U);
	receivedMsg.appendString(MessageParser::composeECSS(testMessage1));

	receivedMsg.appendUint32(currentTime + 1957232U);
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


	LOG_NOTICE << "ECSS Services test application";

	//Receiving from Yamcs

	//TCP Socket creation
	int listening = socket(AF_INET,SOCK_STREAM,0);
	if ( listening< 0) {
		printf("\nTCP socket creation failed");
		return 1;
	} else {
		LOG_DEBUG << "Socket created successfully";
	}
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port= htons(10025);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	//TCP Socket Binding
	if (bind(listening, (sockaddr*) &addr, sizeof(addr))<0){
		printf("\nTCP socket binding failed\n");
		return 2;
	}
	else {
		LOG_DEBUG <<"Binding with 10025 finished successfully";
	}

	//TCP Socket Listening
	if(listen(listening,1) <0){
		printf("\nTCP socket listening failed\n");
		return 1;
	}
	else{
		LOG_DEBUG <<"Listening successfully";
	}


	//TCP Socket Accept
	int clientSocket = accept(listening,(sockaddr*)&addr, (socklen_t*)&addr);
	if (clientSocket<0){
		printf("\nTCP socket acceptance failed\n");
		return 1;
	}
	else{
		LOG_DEBUG <<"Accepted";
	}

	//receeiving a message
	char message[1000];
	int msglen;
	int addrlen = sizeof(addr);
	while(true){
		std::ostringstream ss;
		msglen = recvfrom(clientSocket, (char*)message, sizeof(message), 0, (struct sockaddr*) &addr, reinterpret_cast<socklen_t*>(&addrlen));
		if (msglen < 0) {
			printf("No messages from YAMCS received\n");
			break;
		}
		for (int i = 0; i < msglen; i++) {
			ss << static_cast<int>(message[i]) << " ";

		}
		printf("size of message %d", msglen);
		LOG_NOTICE << ss.str();
		Message m = MessageParser::parse(reinterpret_cast<uint8_t*>(message), msglen);
		MessageParser::execute(m);
	}
	//TCP socket closed
	close(listening);



	LOG_NOTICE << "ECSS Services test complete";

	std::cout << UTCTimestamp() << std::endl;
	return 0;
	LOG_NOTICE << "ECSS Services test complete";
	return 0;


	//TCP socket closed
	close(listening);
}