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


void storeSamplesToParameters(uint16_t id1, uint16_t id2, uint16_t id3) {
	//	Message samples(HousekeepingService::ServiceType,
	//	                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TM, 1);

	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint8_t>&>(parameterManagement.getParameter(id3)->get()).setValue(99);
}

int main() {

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



	LOG_NOTICE << "ECSS Services test complete";

	std::cout << UTCTimestamp() << std::endl;
	return 0;
	LOG_NOTICE << "ECSS Services test complete";
	return 0;


	//TCP socket closed
	close(listening);
}