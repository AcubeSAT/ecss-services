#include "catch2/catch.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("Parameter Report Subservice") {
	ParameterService pserv;

	SECTION("Faulty Instruction Ignorance Test") {
		//TODO: Find a better criterion than checking the first 16 bits

		Message request(20, 1, Message::TC, 1);
		Message report(20, 2, Message::TM, 1);

		request.appendUint16(2);      // number of requested IDs
		request.appendUint16(34672);  // faulty ID in this context
		request.appendUint16(3);      // valid

		pserv.reportParameterIds(request);
		report = ServiceTests::get(0);
		request.resetRead();

		uint16_t repIdCount = report.readUint16();
		uint16_t reqIdCount = request.readUint16();

		REQUIRE(repIdCount < reqIdCount);
		// in case there are ignored IDs the number of IDs in the report
		// will be smaller than the original
	}

	SECTION("Wrong Message Type Handling Test") {
		Message falseRequest(15, 3, Message::TM, 1);   // a totally wrong message

		pserv.reportParameterIds(falseRequest);
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == 2);
		CHECK(response.serviceType == 20);
		CHECK(response.packetType == Message::TM);
		CHECK(response.readPosition == 0);   // if empty, this should't change from 0
	}
}

TEST_CASE("Parameter Setting Subservice") {
	ParameterService pserv;

	SECTION("Faulty Instruction Handling Test") {
		Message setRequest(20, 3, Message::TC, 1);
		Message reportRequest(20, 1, Message::TC, 1);

		setRequest.appendUint16(2);           // correct number of IDs
		setRequest.appendUint16(3);           // correct ID
		setRequest.appendUint32(3735928559);  // 0xDEADBEEF in hex (new setting)
		setRequest.appendUint16(16742);       // faulty ID in this context
		setRequest.appendUint32(3131746989);  // 0xBAAAAAAD (this shouldn't be found in the report)

		reportRequest.appendUint16(2);
		reportRequest.appendUint16(16742);
		reportRequest.appendUint16(3);

		pserv.reportParameterIds(reportRequest);
		Message before = ServiceTests::get(0);

		pserv.setParameterIds(setRequest);

		pserv.reportParameterIds(reportRequest);
		Message after = ServiceTests::get(1);

		before.readUint16();
		after.readUint16();                    // skip the number of IDs

		while (after.readPosition <= after.dataSize) {
			CHECK(before.readUint16() == after.readUint16());   //check if all IDs are present
			CHECK(after.readUint32() != 0xBAAAAAAD);      //check if any settings are BAAAAAAD :P
		}
	}
}
