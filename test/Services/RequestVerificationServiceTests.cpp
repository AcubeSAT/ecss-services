#include <Message.hpp>
#include <Services/RequestVerificationService.hpp>
#include <catch2/catch_all.hpp>
#include "ServiceTests.hpp"


RequestVerificationService& reqVerifService = Services.requestVerification;

TEST_CASE("TM[1,1]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::SuccessfulAcceptanceReport, Message::TC, 3);
	reqVerifService.successAcceptanceVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::SuccessfulAcceptanceReport);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
}

TEST_CASE("TM[1,2]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedAcceptanceReport, Message::TC, 3);
	reqVerifService.failAcceptanceVerification(receivedMessage, ErrorHandler::UnknownAcceptanceError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::FailedAcceptanceReport);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<ErrorCode>() == ErrorHandler::UnknownAcceptanceError);                           // error code
}

TEST_CASE("TM[1,3]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::SuccessfulStartOfExecution, Message::TC, 3);
	reqVerifService.successStartExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::SuccessfulStartOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
}

TEST_CASE("TM[1,4]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedAcceptanceReport, Message::TC, 3);
	reqVerifService.failStartExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionStartError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::FailedStartOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<ErrorCode>() == ErrorHandler::UnknownExecutionStartError);                       // error code
}

TEST_CASE("TM[1,5]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::SuccessfulProgressOfExecution, Message::TC, 3);
	reqVerifService.successProgressExecutionVerification(receivedMessage, 0);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::SuccessfulProgressOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 5); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<StepId>() == 0);                                                                // step ID
}

TEST_CASE("TM[1,6]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::SuccessfulProgressOfExecution, Message::TC, 3);
	reqVerifService.failProgressExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionProgressError, 0);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::FailedProgressOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 7); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<StepId>() == 0);                                                                // step ID
	CHECK(response.read<ErrorCode>() == ErrorHandler::UnknownExecutionProgressError);
}

TEST_CASE("TM[1,7]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::SuccessfulCompletionOfExecution, Message::TC, 3);
	reqVerifService.successCompletionExecutionVerification(receivedMessage);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::SuccessfulCompletionOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 4); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
}

TEST_CASE("TM[1,8]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedCompletionOfExecution, Message::TC, 3);
	reqVerifService.failCompletionExecutionVerification(receivedMessage, ErrorHandler::UnknownExecutionCompletionError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::FailedCompletionOfExecution);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<ErrorCode>() == ErrorHandler::UnknownExecutionCompletionError);                  // error code
}

TEST_CASE("TM[1,10]", "[service][st01]") {
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedRoutingReport, Message::TC, 3);
	reqVerifService.failRoutingVerification(receivedMessage, ErrorHandler::UnknownRoutingError);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	// Checks for the data-members of the object response
	CHECK(response.serviceType == RequestVerificationService::ServiceType);
	CHECK(response.messageType == RequestVerificationService::MessageType::FailedRoutingReport);
	CHECK(response.packetType == Message::TM); // packet type
	CHECK(response.applicationId == ApplicationId);
	REQUIRE(response.dataSize == 6); // dataSize is the number of bytes of data array
	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(response.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(response.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(response.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(response.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(response.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(response.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count
	CHECK(response.read<ErrorCode>() == ErrorHandler::UnknownRoutingError);                              // error code
}

TEST_CASE("assembleReportMessage", "[service][st01]"){
	Message receivedMessage = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedRoutingReport, Message::TC, 3);
	Message report = Message(RequestVerificationService::ServiceType, RequestVerificationService::MessageType::FailedRoutingReport, Message::TC, 3);

	reqVerifService.assembleReportMessage(receivedMessage, report);


	// Check for the value that is stored in <<data>> array(data-member of object response)
	CHECK(report.readEnumerated(reqVerifService.CCSDSPacketVersionBits) == CCSDSPacketVersion);   // packet version number
	CHECK(report.readEnumerated(reqVerifService.PacketTypeBits) == Message::TC);                  // packet type
	CHECK(report.readBits(reqVerifService.SecondaryHeaderFlagBits) == 1);                         // secondary header flag
	CHECK(report.readEnumerated(reqVerifService.ApplicationIdBits) == 3);                         // application process ID
	CHECK(report.readEnumerated(reqVerifService.ECSSSequenceFlagsBits) == ECSSSequenceFlags);     // sequence flags
	CHECK(report.readBits(reqVerifService.PacketSequenceCountBits) == 0);                         // packet sequence count

}

