#include <catch2/catch.hpp>
#include <Services/TestService.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include <cstring>
#include "MessageParser.hpp"
#include "Services/ServiceTests.hpp"
#include "ServicePool.hpp"

TEST_CASE("execute() for the TC packets", "[MessageParser][execute()]") {
	SECTION("ST[19]") {
		EventActionService& eventActionService = Services.eventAction;

		SECTION("Add event-action definitions TC[19,1]") {
			char checkstring[256];
			Message message(19, 1, Message::TC, 0);
			message.appendEnum16(0);
			message.appendEnum16(2);
			String<64> data = "123";
			message.appendString(data);

			MessageParser::execute(message);
			CHECK(eventActionService.eventActionDefinitionArray[0].empty == 0);
			CHECK(eventActionService.eventActionDefinitionArray[0].applicationId == 0);
			CHECK(eventActionService.eventActionDefinitionArray[0].eventDefinitionID == 2);
			CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);
			CHECK(message.readEnum16() == 0);
			CHECK(message.readEnum16() == 2);
			message.readString(checkstring, 3);
			CHECK(eventActionService.eventActionDefinitionArray[0].request.compare(data) == 0);

			Message message2(19, 1, Message::TC, 0);
			message2.appendEnum16(1);
			message2.appendEnum16(3);
			data = "456";
			message2.appendString(data);

			MessageParser::execute(message2);
			CHECK(eventActionService.eventActionDefinitionArray[1].empty == 0);
			CHECK(eventActionService.eventActionDefinitionArray[1].applicationId == 1);
			CHECK(eventActionService.eventActionDefinitionArray[1].eventDefinitionID == 3);
			CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

			CHECK(message2.readEnum16() == 1);
			CHECK(message2.readEnum16() == 3);
			CHECK(Services.eventAction.eventActionDefinitionArray[1].request.compare(data) == 0);
		}

		SECTION("Delete event-action definitions TC[19,2]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message0(19, 1, Message::TC, 0);
			message0.appendEnum16(1);
			message0.appendEnum16(0);
			String<64> data = "0";
			message0.appendString(data);
			MessageParser::execute(message0);
			Message message1(19, 1, Message::TC, 0);
			message1.appendEnum16(1);
			message1.appendEnum16(1);
			data = "1";
			message1.appendString(data);
			MessageParser::execute(message1);
			Message message2(19, 1, Message::TC, 0);
			message2.appendEnum16(1);
			message2.appendEnum16(2);
			data = "2";
			message2.appendString(data);
			MessageParser::execute(message2);
			Message message3(19, 1, Message::TC, 0);
			message3.appendEnum16(1);
			message3.appendEnum16(3);
			data = "3";
			message3.appendString(data);
			MessageParser::execute(message3);
			Message message4(19, 1, Message::TC, 0);
			message4.appendEnum16(1);
			message4.appendEnum16(4);
			data = "4";
			message4.appendString(data);
			MessageParser::execute(message4);

			Message message(19, 2, Message::TC, 0);
			message.appendUint16(2);
			message.appendEnum16(1);
			message.appendEnum16(4);
			message.appendEnum16(1);
			message.appendEnum16(2);
			MessageParser::execute(message);

			CHECK(eventActionService.eventActionDefinitionArray[0].empty == 0);
			CHECK(eventActionService.eventActionDefinitionArray[0].applicationId == 1);
			CHECK(eventActionService.eventActionDefinitionArray[0].eventDefinitionID == 0);
			CHECK(eventActionService.eventActionDefinitionArray[0].request.compare("0") == 0);
			CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);

			CHECK(eventActionService.eventActionDefinitionArray[1].empty == 0);
			CHECK(eventActionService.eventActionDefinitionArray[1].applicationId == 1);
			CHECK(eventActionService.eventActionDefinitionArray[1].eventDefinitionID == 1);
			CHECK(eventActionService.eventActionDefinitionArray[1].request.compare("1") == 0);
			CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

			CHECK(eventActionService.eventActionDefinitionArray[2].empty == 1);
			CHECK(eventActionService.eventActionDefinitionArray[2].applicationId == 0);
			CHECK(eventActionService.eventActionDefinitionArray[2].eventDefinitionID == 65535);
			CHECK(eventActionService.eventActionDefinitionArray[2].request.compare("") == 0);
			CHECK(eventActionService.eventActionDefinitionArray[2].enabled == 0);

			CHECK(eventActionService.eventActionDefinitionArray[3].empty == 0);
			CHECK(eventActionService.eventActionDefinitionArray[3].applicationId == 1);
			CHECK(eventActionService.eventActionDefinitionArray[3].eventDefinitionID == 3);
			CHECK(eventActionService.eventActionDefinitionArray[3].request.compare("3") == 0);
			CHECK(eventActionService.eventActionDefinitionArray[3].enabled == 1);

			CHECK(eventActionService.eventActionDefinitionArray[4].empty == 1);
			CHECK(eventActionService.eventActionDefinitionArray[4].applicationId == 0);
			CHECK(eventActionService.eventActionDefinitionArray[4].eventDefinitionID == 65535);
			CHECK(eventActionService.eventActionDefinitionArray[4].request.compare("") == 0);
			CHECK(eventActionService.eventActionDefinitionArray[4].enabled == 0);

		}

		SECTION("Delete all event-action definitions TC[19,3]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message0(19, 1, Message::TC, 0);
			message0.appendEnum16(1);
			message0.appendEnum16(0);
			String<64> data = "0";
			message0.appendString(data);
			MessageParser::execute(message0);
			Message message1(19, 1, Message::TC, 0);
			message1.appendEnum16(1);
			message1.appendEnum16(1);
			data = "1";
			message1.appendString(data);
			MessageParser::execute(message1);
			Message message2(19, 1, Message::TC, 0);
			message2.appendEnum16(1);
			message2.appendEnum16(2);
			data = "2";
			message2.appendString(data);
			MessageParser::execute(message2);
			Message message3(19, 1, Message::TC, 0);
			message3.appendEnum16(1);
			message3.appendEnum16(3);
			data = "3";
			message3.appendString(data);
			MessageParser::execute(message3);
			Message message4(19, 1, Message::TC, 0);
			message4.appendEnum16(1);
			message4.appendEnum16(4);
			data = "4";
			message4.appendString(data);
			MessageParser::execute(message4);

			Message message(19, 3, Message::TC, 0);
			MessageParser::execute(message);

			for (int i = 0; i < 256; i++) {
				CHECK(eventActionService.eventActionDefinitionArray[i].empty == 1);
				CHECK(eventActionService.eventActionDefinitionArray[i].applicationId == 0);
				CHECK(eventActionService.eventActionDefinitionArray[i].eventDefinitionID == 65535);
				CHECK(eventActionService.eventActionDefinitionArray[i].request.compare("") == 0);
			}
		}

		SECTION("Enable event-action definitions TC[19,4]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message0(19, 1, Message::TC, 0);
			message0.appendEnum16(1);
			message0.appendEnum16(0);
			String<64> data = "0";
			message0.appendString(data);
			MessageParser::execute(message0);
			Message message1(19, 1, Message::TC, 0);
			message1.appendEnum16(1);
			message1.appendEnum16(1);
			data = "00";
			message1.appendString(data);
			MessageParser::execute(message1);
			Message message2(19, 4, Message::TC, 0);
			message2.appendUint16(2);
			message2.appendEnum16(1);
			message2.appendEnum16(0);
			message2.appendEnum16(1);
			message2.appendEnum16(1);
			MessageParser::execute(message2);
			CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 1);
			CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 1);

		}

		SECTION("Disable event-action definitions TC[19,5]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message0(19, 1, Message::TC, 0);
			message0.appendEnum16(1);
			message0.appendEnum16(0);
			String<64> data = "0";
			message0.appendString(data);
			MessageParser::execute(message0);
			Message message1(19, 1, Message::TC, 0);
			message1.appendEnum16(1);
			message1.appendEnum16(0);
			data = "00";
			message1.appendString(data);
			MessageParser::execute(message1);
			Message message2(19, 5, Message::TC, 0);
			message2.appendUint16(1);
			message2.appendEnum16(1);
			message2.appendEnum16(0);
			MessageParser::execute(message2);
			CHECK(eventActionService.eventActionDefinitionArray[0].enabled == 0);
			CHECK(eventActionService.eventActionDefinitionArray[1].enabled == 0);
		}

		SECTION("Request event-action definition status TC[19,6]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message(19, 6, Message::TC, 0);
			MessageParser::execute(message);
			REQUIRE(ServiceTests::hasOneMessage());

			Message report = ServiceTests::get(0);
			CHECK(report.messageType == 7);
		}

		SECTION("Event-action status report TM[19,7]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();
			ServiceTests::reset();

			Message message0(19, 1, Message::TC, 0);
			message0.appendEnum16(1);
			message0.appendEnum16(0);
			String<64> data = "0";
			message0.appendString(data);
			MessageParser::execute(message0);
			Message message1(19, 1, Message::TC, 0);
			message1.appendEnum16(1);
			message1.appendEnum16(2);
			data = "2";
			message1.appendString(data);
			MessageParser::execute(message1);
			Message message2(19, 5, Message::TC, 0);
			message2.appendUint16(1);
			message2.appendEnum16(1);
			message2.appendEnum16(0);
			MessageParser::execute(message2);
			eventActionService.eventActionStatusReport();
			REQUIRE(ServiceTests::hasOneMessage());

			Message report = ServiceTests::get(0);
			CHECK(report.readUint8() == 2);
			CHECK(report.readEnum16() == 1);
			CHECK(report.readEnum16() == 0);
			CHECK(report.readUint8() == 0);
			CHECK(report.readEnum16() == 1);
			CHECK(report.readEnum16() == 2);
			CHECK(report.readUint8() == 1);
		}

		SECTION("Enable event-action function TC[19,8]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message(19, 8, Message::TC, 0);
			MessageParser::execute(message);
			CHECK(eventActionService.getEventActionFunctionStatus() == true);
		}

		SECTION("Disable event-action function TC[19,9]") {
			EventActionService& eventActionService = Services.eventAction;
			Services.reset();

			Message message(19, 9, Message::TC, 0);
			MessageParser::execute(message);
			CHECK(eventActionService.getEventActionFunctionStatus() == false);
		}
	}

	SECTION("ST[05]") {
		SECTION("Enable Report Generation TC[5,5]") {
			EventReportService& eventReportService = Services.eventReport;
			Services.reset();
			ServiceTests::reset();

			eventReportService.getStateOfEvents().reset();
			EventReportService::Event eventID[] = {EventReportService::AssertionFail,
				EventReportService::LowSeverityUnknownEvent};
			Message message(5, 5, Message::TC, 1);
			message.appendUint16(2);
			message.appendEnum16(eventID[0]);
			message.appendEnum16(eventID[1]);
			MessageParser::execute(message);
			CHECK(eventReportService.getStateOfEvents()[2] == 1);
			CHECK(eventReportService.getStateOfEvents()[4] == 1);
		}

		SECTION("Disable Report Generation TC[5,6]") {
			EventReportService& eventReportService = Services.eventReport;
			Services.reset();
			ServiceTests::reset();

			EventReportService::Event eventID[] = {EventReportService::InformativeUnknownEvent,
				EventReportService::MediumSeverityUnknownEvent};
			Message message(5, 6, Message::TC, 1);
			message.appendUint16(2);
			message.appendEnum16(eventID[0]);
			message.appendEnum16(eventID[1]);
			MessageParser::execute(message);
			CHECK(eventReportService.getStateOfEvents()[0] == 0);
			CHECK(eventReportService.getStateOfEvents()[5] == 0);

			const String<64> eventReportData = "HelloWorld";
			eventReportService.highSeverityAnomalyReport(EventReportService::InformativeUnknownEvent,
				eventReportData);
			CHECK(ServiceTests::hasOneMessage() == false);
		}

		SECTION("Request list of disabled events TC[5,7]") {
			EventReportService& eventReportService = Services.eventReport;
			Services.reset();
			ServiceTests::reset();


			Message message(5, 7, Message::TC, 1);
			MessageParser::execute(message);
			REQUIRE(ServiceTests::hasOneMessage());

			Message report = ServiceTests::get(0);
			// Check if there is message of type 8 created
			CHECK(report.messageType == 8);
		}
	}

	SECTION("ST[06]"){
		SECTION("TC[6,2]") {
			ServiceTests::reset();

			// Required test variables
			char *pStr = static_cast<char *>(malloc(4));
			*pStr = 'T';
			*(pStr + 1) = 'G';
			*(pStr + 2) = '\0';
			uint8_t data[2] = {'h', 'R'};

			Message receivedPacket = Message(6, 2, Message::TC, 1);
			receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
			receivedPacket.appendUint16(2); // Iteration count
			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(pStr)); // Start address
			receivedPacket.appendOctetString(String<2>(data));
			receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 2)); // Append CRC
			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(pStr + 2)); // Start address
			receivedPacket.appendOctetString(String<1>(data)); // Append CRC
			receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 1));
			MessageParser::execute(receivedPacket);

			CHECK(pStr[0] == 'h');
			CHECK(pStr[1] == 'R');
			CHECK(pStr[2] == 'h');
		}

		SECTION("TC[6,5]") {
			ServiceTests::reset();

			uint8_t testString_1[6] = "FStrT";
			uint8_t testString_2[8] = "SecStrT";
			uint8_t testString_3[2] = {5, 8};

			uint8_t checkString[ECSS_MAX_STRING_SIZE];
			uint16_t readSize = 0, checksum = 0;

			Message receivedPacket = Message(6, 5, Message::TC, 1);
			receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
			receivedPacket.appendUint16(3); // Iteration count (Equal to 3 test strings)
			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_1)); // Start address
			receivedPacket.appendUint16(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_2));
			receivedPacket.appendUint16(sizeof(testString_2) / sizeof(testString_2[0]));

			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_3));
			receivedPacket.appendUint16(sizeof(testString_3) / sizeof(testString_3[0]));
			MessageParser::execute(receivedPacket);
			REQUIRE(ServiceTests::hasOneMessage());

			Message response = ServiceTests::get(0);
			CHECK(response.serviceType == 6);
			CHECK(response.messageType == 6);
			REQUIRE(response.dataSize == 55);

			CHECK(response.readEnum8() == MemoryManagementService::MemoryID::EXTERNAL);
			CHECK(response.readUint16() == 3);
			CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_1));
			readSize = response.readOctetString(checkString);
			checksum = response.readBits(16);
			CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
			CHECK(checkString[0] == 'F');
			CHECK(checkString[1] == 'S');
			CHECK(checkString[2] == 't');
			CHECK(checkString[3] == 'r');
			CHECK(checkString[4] == 'T');
			CHECK(checkString[5] == '\0');
			CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

			CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_2));
			readSize = response.readOctetString(checkString);
			checksum = response.readBits(16);
			CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
			CHECK(checkString[0] == 'S');
			CHECK(checkString[1] == 'e');
			CHECK(checkString[2] == 'c');
			CHECK(checkString[3] == 'S');
			CHECK(checkString[4] == 't');
			CHECK(checkString[5] == 'r');
			CHECK(checkString[6] == 'T');
			CHECK(checkString[7] == '\0');
			CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

			CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_3));
			readSize = response.readOctetString(checkString);
			checksum = response.readBits(16);
			CHECK(readSize == sizeof(testString_3) / sizeof(testString_3[0]));
			CHECK(checkString[0] == 5);
			CHECK(checkString[1] == 8);
			CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));
		}

		SECTION("TC[6,9]") {
			ServiceTests::reset();

			uint8_t testString_1[6] = "FStrT";
			uint8_t testString_2[8] = "SecStrT";
			uint16_t readSize = 0, checksum = 0;

			Message receivedPacket = Message(6, 9, Message::TC, 1);
			receivedPacket.appendEnum8(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
			receivedPacket.appendUint16(2); // Iteration count
			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_1)); // Start address
			receivedPacket.appendUint16(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

			receivedPacket.appendUint64(reinterpret_cast<uint64_t >(testString_2));
			receivedPacket.appendUint16(sizeof(testString_2) / sizeof(testString_2[0]));
			MessageParser::execute(receivedPacket);
			REQUIRE(ServiceTests::hasOneMessage());

			Message response = ServiceTests::get(0);
			CHECK(response.serviceType == 6);
			CHECK(response.messageType == 10);
			REQUIRE(response.dataSize == 27);

			CHECK(response.readEnum8() == MemoryManagementService::MemoryID::EXTERNAL);
			CHECK(response.readUint16() == 2);
			CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_1));
			readSize = response.readUint16();
			checksum = response.readBits(16);
			CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
			CHECK(checksum == CRCHelper::calculateCRC(testString_1, readSize));

			CHECK(response.readUint64() == reinterpret_cast<uint64_t >(testString_2));
			readSize = response.readUint16();
			checksum = response.readBits(16);
			CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
			CHECK(checksum == CRCHelper::calculateCRC(testString_2, readSize));
		}
	}

	SECTION("ST[08]") {
		FunctionManagementService & fms = Services.functionManagement;
		void test(String<MAX_ARG_LENGTH> a);
		SECTION("Malformed name") {
			ServiceTests::reset();
			fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
			Message msg(8, 1, Message::TC, 1);
			msg.appendString(String<FUNC_NAME_LENGTH>("t3st"));
			MessageParser::execute(msg);
			CHECK(ServiceTests::get(0).messageType == 4);
			CHECK(ServiceTests::get(0).serviceType == 1);
		}

		SECTION("Too long message") {
			ServiceTests::reset();
			fms.include(String<FUNC_NAME_LENGTH>("test"), &test);
			Message msg(8, 1, Message::TC, 1);
			msg.appendString(String<FUNC_NAME_LENGTH>("test"));
			msg.appendString(String<65>
				("eqrhjweghjhwqgthjkrghthjkdsfhgsdfhjsdjsfdhgkjdfsghfjdgkdfsgdfgsgd"));
			MessageParser::execute(msg);
			CHECK(ServiceTests::get(0).messageType == 4);
			CHECK(ServiceTests::get(0).serviceType == 1);
		}
	}

	SECTION("ST[17]") {
		MessageParser messageParser;
		TestService& testService = Services.testService;
		ServiceTests::reset();

		Message message = Message(17, 1, Message::TC, 1);
		MessageParser::execute(message);
		Message response = ServiceTests::get(0);
		CHECK(response.serviceType == 17);
		CHECK(response.messageType == 2);
		CHECK(response.packetType == Message::TM);

		message = Message(17, 3, Message::TC, 1);
		message.appendUint16(7);
		MessageParser::execute(message);
		response = ServiceTests::get(1);
		CHECK(response.serviceType == 17);
		CHECK(response.messageType == 4);
		CHECK(response.packetType == Message::TM);
	}


}

TEST_CASE("TC message parsing", "[MessageParser]") {
	MessageParser messageParser;

	uint8_t packet[] = {0x18, 0x07, 0xc0, 0x4d, 0x00, 0x0a, 0x20, 0x81, 0x1f, 0x00, 0x00, 0x68,
		0x65, 0x6c, 0x6c, 0x6f};

	Message message = messageParser.parse(packet, 16);
	CHECK(message.packetType == Message::TC);
	CHECK(message.applicationId == 7);
	CHECK(message.dataSize == 5);
	CHECK(message.serviceType == 129);
	CHECK(message.messageType == 31);
	CHECK(memcmp(message.data, "hello", 5) == 0);
}

TEST_CASE("TC data parsing into a message", "[MessageParser]") {

}

TEST_CASE("TM message parsing", "[MessageParser]") {
	MessageParser messageParser;
	uint8_t packet[] = {0x08, 0x02, 0xc0, 0x4d, 0x00, 0x0c, 0x20, 0x16, 0x11, 0x00, 0x00, 0x68,
		0x65, 0x6c, 0x6c, 0x6f,
		0x68, 0x69};
	Message message = messageParser.parse(packet, 18);
	CHECK(message.packetType == Message::TM);
	CHECK(message.applicationId == 2);
	CHECK(message.dataSize == 7);
	CHECK(message.serviceType == 22);
	CHECK(message.messageType == 17);
	CHECK(memcmp(message.data, "hellohi", 7) == 0);
}
