#include "catch2/catch.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"


TEST_CASE("Parameter Report Subservice") {
	ParameterService pserv;

	SECTION("Faulty Instruction Handling Test") {
		Message request(20, 1, Message::TC, 1);
		Message report(20, 2, Message::TM, 1);

		request.appendUint16(2);      // number of requested IDs
		request.appendUint16(34672);  // faulty ID in this context
		request.appendUint16(1);      // valid

		pserv.reportParameterIds(request);
		report = ServiceTests::get(0);
		request.resetRead();

		report.readUint16();
		request.readUint16();

		while (report.readPosition <= report.dataSize) {
			CHECK_FALSE(report.readUint16() == 34672);  //fail if faulty ID is present in report
			report.readUint32();                   //ignore the carried settings
		}
	}

	// **WARNING**
	// TODO: Update this test (and all tests in general) to use the error handler's output when
	//  checking for assertions.
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

		setRequest.appendUint16(2);           // total number of IDs
		setRequest.appendUint16(1);           // correct ID in this context
		setRequest.appendUint32(3735928559);  // 0xDEADBEEF in hex (new setting)
		setRequest.appendUint16(16742);       // faulty ID in this context
		setRequest.appendUint32(3131746989);  // 0xBAAAAAAD (this shouldn't be found in the report)

		reportRequest.appendUint16(2);
		reportRequest.appendUint16(16742);
		reportRequest.appendUint16(1);       // used to be 3, which pointed the bug with
		// numOfValidIds out, now is 1 in order to be a valid ID (a separate test for
		// numOfValidIds shall be introduced)

		pserv.reportParameterIds(reportRequest);
		Message before = ServiceTests::get(0);

		pserv.setParameterIds(setRequest);

		pserv.reportParameterIds(reportRequest);
		Message after = ServiceTests::get(1);

		before.readUint16();
		after.readUint16();                    // skip the number of IDs

		while (after.readPosition <= after.dataSize) {
			CHECK(before.readUint16() == after.readUint16());   // check if all IDs are present
			CHECK_FALSE(after.readUint32() == 0xBAAAAAAD);   // fail if any settings are BAAAAAAD :P
		}
	}
}
