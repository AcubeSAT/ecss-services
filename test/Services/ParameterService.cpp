#include "catch2/catch.hpp"
#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"

ParameterService& pserv = Services.parameterManagement;

void foo(ValueType* bar) {  // sample function
	*bar = 0xDEADBEEF;
}

TEST_CASE("Parameter Report Subservice") {

	SECTION("Insertion test") {
		pserv.addNewParameter(3, 14);  // this one has ID 0
		pserv.addNewParameter(1, 7, 12);  // this one has 1
		pserv.addNewParameter(4, 12, 3, nullptr);  // this one has 2
		pserv.addNewParameter(12, 3, 6, &foo); // this one has 3
	}

	SECTION("ID checking") {
		Message request(20, 1, Message::TC, 1);
		Message report(20, 2, Message::TM, 1);

		request.appendUint16(2)
	}

	SECTION("Faulty Instruction Handling Test") {
		Message request(20, 1, Message::TC, 1);
		Message report(20, 2, Message::TM, 1);

		request.appendUint16(2); // number of requested IDs
		request.appendUint16(34672); // faulty ID in this context
		request.appendUint16(1); // valid

		MessageParser::execute(request);
		report = ServiceTests::get(0);
		request.resetRead();

		report.readUint16();
		request.readUint16();

		while (report.readPosition <= report.dataSize) {
			CHECK_FALSE(report.readUint16() == 34672); // fail if faulty ID is present in report
			report.readUint32(); // ignore the carried settings
		}
	}

	// **WARNING**
	// TODO: Update this test (and all tests in general) to use the error handler's output when
	//  checking for assertions.
	SECTION("Wrong Message Type Handling Test") {
		Message falseRequest(15, 3, Message::TM, 1); // a totally wrong message

		MessageParser::execute(falseRequest);
		Message errorNotif = ServiceTests::get(0);
		CHECK(errorNotif.messageType == 4); // check for proper failed start of
		// execution notification
		CHECK(errorNotif.serviceType == 1);

		Message response = ServiceTests::get(1);
		CHECK(response.messageType == 2);
		CHECK(response.serviceType == 20);
		CHECK(response.packetType == Message::TM);
		CHECK(response.readPosition == 0); // if empty, this should't change from 0
	}
}

TEST_CASE("Parameter Setting Subservice") {
	SECTION("Faulty Instruction Handling Test") {
		Message setRequest(20, 3, Message::TC, 1);
		Message reportRequest(20, 1, Message::TC, 1);

		setRequest.appendUint16(2); // total number of IDs
		setRequest.appendUint16(1); // correct ID in this context
		setRequest.appendUint32(3735928559); // 0xDEADBEEF in hex (new setting)
		setRequest.appendUint16(16742); // faulty ID in this context
		setRequest.appendUint32(3131746989); // 0xBAAAAAAD (this shouldn't be found in the report)

		reportRequest.appendUint16(2);
		reportRequest.appendUint16(16742);
		reportRequest.appendUint16(1); // used to be 3, which pointed the bug with
		// numOfValidIds out, now is 1 in order to be a valid ID (a separate test for
		// numOfValidIds shall be introduced)

		// Since every reporting and setting is called with the same (sometimes faulty) parameters,
		// and there are errors generated (as should be) it is important to catch and check for
		// them in order to preserve the integrity of the test.
		MessageParser::execute(reportRequest);
		Message errorNotif1 = ServiceTests::get(0);
		CHECK(errorNotif1.messageType == 4);
		CHECK(errorNotif1.serviceType == 1);

		Message before = ServiceTests::get(1);

		MessageParser::execute(setRequest);
		Message errorNotif2 = ServiceTests::get(2);
		CHECK(errorNotif2.messageType == 4);
		CHECK(errorNotif2.serviceType == 1);

		MessageParser::execute(reportRequest);
		Message errorNotif3 = ServiceTests::get(3);
		CHECK(errorNotif3.messageType == 4);
		CHECK(errorNotif3.serviceType == 1);

		Message after = ServiceTests::get(4);

		before.readUint16();
		after.readUint16(); // skip the number of IDs

		while (after.readPosition <= after.dataSize) {
			CHECK(before.readUint16() == after.readUint16()); // check if all IDs are present
			CHECK_FALSE(after.readUint32() == 0xBAAAAAAD); // fail if any settings are BAAAAAAD :P
		}
	}
}
