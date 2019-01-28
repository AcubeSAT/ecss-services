#include <catch2/catch.hpp>
#include <Services/RequestVerificationService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"

TEST_CASE("TM[1,1]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 1, Message::TC, 3);
	reqVerifService.successAcceptanceVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 1);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
}

TEST_CASE("TM[1,2]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 2, Message::TC, 3);
	reqVerifService.failAcceptanceVerification(receivedMessage,
	                                           ErrorHandler::UnknownAcceptanceError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 2);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownAcceptanceError); // error code
}

TEST_CASE("TM[1,3]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 3, Message::TC, 3);
	reqVerifService.successStartExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 3);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
}

TEST_CASE("TM[1,4]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 2, Message::TC, 3);
	reqVerifService.failStartExecutionVerification(receivedMessage,
	                                               ErrorHandler::UnknownExecutionStartError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 4);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownExecutionStartError); // error code
}

TEST_CASE("TM[1,5]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 5, Message::TC, 3);
	reqVerifService.successProgressExecutionVerification(receivedMessage,
	                                                     ErrorHandler::UnknownStepID);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 5);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownStepID); // step ID
}

TEST_CASE("TM[1,6]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 5, Message::TC, 3);
	reqVerifService.failProgressExecutionVerification(receivedMessage,
	                                                  ErrorHandler::UnknownExecutionProgressError,
	                                                  ErrorHandler::UnknownStepID);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 6);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 8); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownStepID); // step ID
	CHECK(response.readEnum16() == ErrorHandler::UnknownExecutionProgressError);
}

TEST_CASE("TM[1,7]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 7, Message::TC, 3);
	reqVerifService.successCompletionExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 7);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
}

TEST_CASE("TM[1,8]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 8, Message::TC, 3);
	reqVerifService.failCompletionExecutionVerification(receivedMessage,
		ErrorHandler::UnknownExecutionCompletionError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 8);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownExecutionCompletionError); // error code
}

TEST_CASE("TM[1,10]", "[service][st01]") {
	RequestVerificationService reqVerifService;

	Message receivedMessage = Message(1, 10, Message::TC, 3);
	reqVerifService.failRoutingVerification(receivedMessage, ErrorHandler::UnknownRoutingError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == 1);
	CHECK(response.messageType == 10);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == 0);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(3) == CCSDS_PACKET_VERSION); // packet version number
	CHECK(response.readEnumerated(1) == Message::TC); // packet type
	CHECK(response.readBits(1) == 0); // secondary header flag
	CHECK(response.readEnumerated(11) == 3); // application process ID
	CHECK(response.readEnumerated(2) == ECSS_SEQUENCE_FLAGS); // sequence flags
	CHECK(response.readBits(14) == 0); // packet sequence count
	CHECK(response.readEnum16() == ErrorHandler::UnknownRoutingError); // error code
}
