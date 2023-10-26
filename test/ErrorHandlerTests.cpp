#include <ErrorHandler.hpp>
#include <catch2/catch_all.hpp>
#include "Services/RequestVerificationService.hpp"
#include "Services/ServiceTests.hpp"

TEST_CASE("Error: Failed Acceptance", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 47);
	ErrorHandler::reportError(failedMessage, ErrorHandler::MessageTooShort);

	REQUIRE(ServiceTests::hasOneMessage());
	CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooShort));
	Message report = ServiceTests::get(0);

	// Check that a TM[1,2] message was returned
	CHECK(report.serviceType == RequestVerificationService::ServiceType);
	CHECK(report.messageType == RequestVerificationService::MessageType::FailedAcceptanceReport);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == CCSDSPacketVersion);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == 1);
	CHECK(report.readBits(11) == 47);
	CHECK(report.readBits(2) == ECSSSequenceFlags);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == ErrorHandler::MessageTooShort);
}

TEST_CASE("Error: Failed Execution Start", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 56);
	ErrorHandler::reportError(failedMessage, ErrorHandler::UnknownExecutionStartError);

	REQUIRE(ServiceTests::hasOneMessage());
	CHECK(ServiceTests::thrownError(ErrorHandler::UnknownExecutionStartError));
	Message report = ServiceTests::get(0);

	// Check that a TM[1,3] message was returned
	CHECK(report.serviceType == RequestVerificationService::ServiceType);
	CHECK(report.messageType == RequestVerificationService::MessageType::FailedStartOfExecution);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == CCSDSPacketVersion);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == 1);
	CHECK(report.readBits(11) == 56);
	CHECK(report.readBits(2) == ECSSSequenceFlags);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == ErrorHandler::UnknownExecutionStartError);
}

TEST_CASE("Error: Failed Execution Progress", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 56);
	ErrorHandler::reportProgressError(failedMessage, ErrorHandler::UnknownExecutionProgressError, 0);

	REQUIRE(ServiceTests::hasOneMessage());
	CHECK(ServiceTests::thrownError(ErrorHandler::UnknownExecutionProgressError));
	Message report = ServiceTests::get(0);

	// Check that a TM[1,6] message was returned
	CHECK(report.serviceType == RequestVerificationService::ServiceType);
	CHECK(report.messageType == RequestVerificationService::MessageType::FailedProgressOfExecution);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 7);

	CHECK(report.readBits(3) == CCSDSPacketVersion);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == 1);
	CHECK(report.readBits(11) == 56);
	CHECK(report.readBits(2) == ECSSSequenceFlags);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == ErrorHandler::UnknownExecutionProgressError);
	CHECK(report.readByte() == 0); // stepID
}

TEST_CASE("Error: Failed Execution Completion", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 56);
	ErrorHandler::reportError(failedMessage, ErrorHandler::UnknownExecutionCompletionError);

	REQUIRE(ServiceTests::hasOneMessage());
	CHECK(ServiceTests::thrownError(ErrorHandler::UnknownExecutionCompletionError));
	Message report = ServiceTests::get(0);

	// Check that a TM[1,8] message was returned
	CHECK(report.serviceType == RequestVerificationService::ServiceType);
	CHECK(report.messageType == RequestVerificationService::MessageType::FailedCompletionOfExecution);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == CCSDSPacketVersion);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == 1);
	CHECK(report.readBits(11) == 56);
	CHECK(report.readBits(2) == ECSSSequenceFlags);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == ErrorHandler::UnknownExecutionCompletionError);
}

TEST_CASE("Error: Failed Routing", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 71);
	ErrorHandler::reportError(failedMessage, ErrorHandler::UnknownRoutingError);

	REQUIRE(ServiceTests::hasOneMessage());
	CHECK(ServiceTests::thrownError(ErrorHandler::UnknownRoutingError));
	Message report = ServiceTests::get(0);

	// Check that a TM[1,8] message was returned
	CHECK(report.serviceType == RequestVerificationService::ServiceType);
	CHECK(report.messageType == RequestVerificationService::MessageType::FailedRoutingReport);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == CCSDSPacketVersion);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == 1);
	CHECK(report.readBits(11) == 71);
	CHECK(report.readBits(2) == ECSSSequenceFlags);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == ErrorHandler::UnknownRoutingError);
}
