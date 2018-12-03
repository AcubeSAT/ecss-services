#include <catch2/catch.hpp>
#include <ErrorHandler.hpp>
#include "Services/ServiceTests.hpp"

TEST_CASE("Error: Failed Acceptance", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 47);
	ErrorHandler::reportError(failedMessage, ErrorHandler::MessageTooShort);

	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);

	// Check that a TM[1,2] message was returned
	CHECK(report.serviceType == 1);
	CHECK(report.messageType == 2);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == ECSS_PUS_VERSION);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == true);
	CHECK(report.readBits(11) == 47);
	CHECK(report.readBits(2) == ECSS_SEQUENCE_FLAGS);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == 1);
}

TEST_CASE("Error: Failed Execution Completion", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 56);
	ErrorHandler::reportError(failedMessage, ErrorHandler::UnknownExecutionError);

	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);

	// Check that a TM[1,8] message was returned
	CHECK(report.serviceType == 1);
	CHECK(report.messageType == 8);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == ECSS_PUS_VERSION);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == true);
	CHECK(report.readBits(11) == 56);
	CHECK(report.readBits(2) == ECSS_SEQUENCE_FLAGS);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == 0);
}

TEST_CASE("Error: Failed Routing", "[errors]") {
	Message failedMessage(38, 32, Message::TC, 71);
	ErrorHandler::reportError(failedMessage, ErrorHandler::UnknownRoutingError);

	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);

	// Check that a TM[1,8] message was returned
	CHECK(report.serviceType == 1);
	CHECK(report.messageType == 10);
	CHECK(report.packetType == Message::TM);
	REQUIRE(report.dataSize == 6);

	CHECK(report.readBits(3) == ECSS_PUS_VERSION);
	CHECK(report.readBits(1) == static_cast<uint16_t>(Message::TC));
	CHECK(report.readBits(1) == true);
	CHECK(report.readBits(11) == 71);
	CHECK(report.readBits(2) == ECSS_SEQUENCE_FLAGS);
	CHECK(report.readBits(14) == failedMessage.packetSequenceCount);
	CHECK(report.readEnum16() == 0);
}
