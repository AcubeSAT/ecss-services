#include <catch2/catch.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[1,1]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 1, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID
	receivedMessage.appendEnumerated(2, 2); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count
	reqVerifService.successAcceptanceVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 1);
	CHECK(response.packetType == 0); // packet type(TM = 0, TC = 1)
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == 2); // packet version number
	CHECK(response.readEnumerated(1) == 1); // packet type
	CHECK(response.readBits(1) == true); // secondary header flag
	CHECK(response.readEnumerated(11) == 2); // application process ID
	CHECK(response.readEnumerated(2) == 2); // sequence flags
	CHECK(response.readBits(14) == 10); // packet sequence count
}

TEST_CASE("TM[1,2]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 2, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID
	receivedMessage.appendEnumerated(2, 2); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count
	receivedMessage.appendEnum16(5); // error code
	reqVerifService.failAcceptanceVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 2);
	CHECK(response.packetType == 0); // packet type(TM = 0, TC = 1)
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == 2); // packet version number
	CHECK(response.readEnumerated(1) == 1); // packet type
	CHECK(response.readBits(1) == true); // secondary header flag
	CHECK(response.readEnumerated(11) == 2); // application process ID
	CHECK(response.readEnumerated(2) == 2); // sequence flags
	CHECK(response.readBits(14) == 10); // packet sequence count
	CHECK(response.readEnum16() == 5); // error code
}

TEST_CASE("TM[1,7]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 7, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID
	receivedMessage.appendEnumerated(2, 2); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count
	reqVerifService.successExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 7);
	CHECK(response.packetType == 0); // packet type(TM = 0, TC = 1)
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == 2); // packet version number
	CHECK(response.readEnumerated(1) == 1); // packet type
	CHECK(response.readBits(1) == true); // secondary header flag
	CHECK(response.readEnumerated(11) == 2); // application process ID
	CHECK(response.readEnumerated(2) == 2); // sequence flags
	CHECK(response.readBits(14) == 10); // packet sequence count
}

TEST_CASE("TM[1,8]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 8, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID
	receivedMessage.appendEnumerated(2, 2); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count
	receivedMessage.appendEnum16(6); // error code
	reqVerifService.failExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 8);
	CHECK(response.packetType == 0); // packet type(TM = 0, TC = 1)
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == 2); // packet version number
	CHECK(response.readEnumerated(1) == 1); // packet type
	CHECK(response.readBits(1) == true); // secondary header flag
	CHECK(response.readEnumerated(11) == 2); // application process ID
	CHECK(response.readEnumerated(2) == 2); // sequence flags
	CHECK(response.readBits(14) == 10); // packet sequence count
	CHECK(response.readEnum16() == 6); // error code
}

TEST_CASE("TM[1,10]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 10, Message::TC, 3);
	receivedMessage.appendEnumerated(3, ECSS_PUS_VERSION); // packet version number
	receivedMessage.appendEnumerated(1, Message::TC); // packet type
	receivedMessage.appendBits(1, static_cast<uint8_t >(true)); // secondary header flag
	receivedMessage.appendEnumerated(11, 2); // application process ID
	receivedMessage.appendEnumerated(2, 2); // sequence Flags
	receivedMessage.appendBits(14, 10); // packet sequence count
	receivedMessage.appendEnum16(7); // error code
	reqVerifService.failRoutingVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 10);
	CHECK(response.packetType == 0); // packet type(TM = 0, TC = 1)
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == 2); // packet version number
	CHECK(response.readEnumerated(1) == 1); // packet type
	CHECK(response.readBits(1) == true); // secondary header flag
	CHECK(response.readEnumerated(11) == 2); // application process ID
	CHECK(response.readEnumerated(2) == 2); // sequence flags
	CHECK(response.readBits(14) == 10); // packet sequence count
	CHECK(response.readEnum16() == 7); // error code
}
