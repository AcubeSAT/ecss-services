#include <catch2/catch.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[1,1]", "[service][st01]") {
	RequestVerificationService reqVerifService;
	reqVerifService.successAcceptanceVerification(Message::TC, true, 2, 2, 10);
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
	reqVerifService.failAcceptanceVerification(Message::TC, true, 2, 2, 10, 5);
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
	reqVerifService.successExecutionVerification(Message::TC, true, 2, 2, 10);
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
	reqVerifService.failExecutionVerification(Message::TC, true, 2, 2, 10, 6);
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
	reqVerifService.failRoutingVerification(Message::TC, true, 2, 2, 10, 7);
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

