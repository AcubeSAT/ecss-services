#include "catch2/catch.hpp"
#include "ServicePool.hpp"
#include "ServiceTests.hpp"

struct Tester {
	static auto returnMap(HousekeepingService& hsService) {
		return hsService.housekeepingStructureList;
	}
};

HousekeepingService& housekeepingService = Services.housekeeping;
ParameterService& paramService = Services.parameterManagement;

TEST_CASE("Create housekeeping structures and generate periodic parameter reports", "[service][st03]") {
	// create a request TC[3,1]
	Message request1(3, 1, Message::TC, 0);
	request1.appendByte(0); // housekeeping ID = 0
	request1.appendWord(1); // collection interval = 1
	request1.appendByte(2); // number of param IDs = 2
	request1.appendHalfword(1); // paramID = 1
	request1.appendHalfword(2); // paramID = 2

	MessageParser::execute(request1);

	// check the map with the housekeeping id and the structure
	//Tester test;
	auto map = Tester::returnMap(housekeepingService);
	auto it = map.find(0);
	CHECK(map.size() == 1);
	CHECK(it->first == 0);
	CHECK(it->second.collectionInterval == 1);
	CHECK(it->second.paramId.size() == 2);
	CHECK(it->second.paramId[0] == 1);
	CHECK(it->second.paramId[1] == 2);
	CHECK_FALSE(it->second.isPeriodic);

	Message request2(3, 1, Message::TC, 0);
	request2.appendByte(1); // housekeeping ID = 1
	request2.appendWord(2); // collection interval = 2
	request2.appendByte(3); // number of param IDs = 3
	request2.appendHalfword(3); // paramID = 3
	request2.appendHalfword(4); // paramID = 4
	request2.appendHalfword(5); // paramID = 5

	MessageParser::execute(request2);

	map = Tester::returnMap(housekeepingService);
	it = map.find(0);
	CHECK(map.find(1) != map.end());
	CHECK(map.size() == 2);
	CHECK(it->first == 0);
	CHECK(it->second.collectionInterval == 1);
	CHECK(it->second.paramId.size() == 2);
	CHECK(it->second.paramId[0] == 1);
	CHECK(it->second.paramId[1] == 2);
	CHECK_FALSE(it->second.isPeriodic);
	it = map.find(1);
	CHECK(it->first == 1);
	CHECK(it->second.collectionInterval == 2);
	CHECK(it->second.paramId.size() == 3);
	CHECK(it->second.paramId[0] == 3);
	CHECK(it->second.paramId[1] == 4);
	CHECK(it->second.paramId[2] == 5);
	CHECK_FALSE(it->second.isPeriodic);

	// now we have 2 housekeeping structures with IDs 0 and 1 and collection intervals 1 and 2 respectively

	// Add parameters
	Parameter param0 = Parameter(1, 2, 3);
	Parameter param1 = Parameter(4, 5, 6);
	Parameter param2 = Parameter(7, 8, 9);
	Parameter param3 = Parameter(10, 11, 12);
	Parameter param4 = Parameter(13, 14, 15);

	paramService.addNewParameter(1, param0);
	paramService.addNewParameter(2, param1);
	paramService.addNewParameter(3, param2);
	paramService.addNewParameter(4, param3);
	paramService.addNewParameter(5, param4);

	// now we have a full parameter's map with param IDs and values

	// lets try to generate reports based on collection interval and current time

	map = Tester::returnMap(housekeepingService);
	it = map.find(1);
	CHECK_FALSE(it->second.isPeriodic);
	it = map.find(0);
	CHECK_FALSE(it->second.isPeriodic);

	// enable structures
	Message enable = Message(3, 5, Message::TC, 0);
	enable.appendByte(2);
	enable.appendByte(0);
	enable.appendByte(1);
	MessageParser::execute(enable);

	map = Tester::returnMap(housekeepingService);
	it = map.find(1);
	CHECK(it->second.isPeriodic);
	it = map.find(0);
	CHECK(it->second.isPeriodic);

	TimeAndDate timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 10);
	housekeepingService.checkAndSendHousekeepingReports(timeUtc);

	Message response = ServiceTests::get(0);
	CHECK(response.readUint8() == 0); // housekeeping ID
	CHECK(response.readUint32() == 3); // value of paramID = 1
	CHECK(response.readUint32() == 6); // value of paramID = 2

	// same time

	housekeepingService.checkAndSendHousekeepingReports(timeUtc);

	CHECK(ServiceTests::count() == 2);
	response = ServiceTests::get(1);
	CHECK(response.readUint8() == 1); // housekeeping ID
	CHECK(response.readUint32() == 9); // value of paramID = 3
	CHECK(response.readUint32() == 12); // value of paramID = 4
	CHECK(response.readUint32() == 15); // value of paramID = 5

	// disable structures
	Message disable = Message(3, 6, Message::TC, 0);
	disable.appendByte(2);
	disable.appendByte(1);
	disable.appendByte(0);
	MessageParser::execute(disable);

	map = Tester::returnMap(housekeepingService);
	it = map.find(1);
	CHECK_FALSE(it->second.isPeriodic);
	it = map.find(0);
	CHECK_FALSE(it->second.isPeriodic);

	timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 20); // increment the time
	housekeepingService.checkAndSendHousekeepingReports(timeUtc); // param reports should not be generated
	CHECK(ServiceTests::count() == 2);
	CHECK(map.size() == 2);

	// enable structures
	Message enableStruct = Message(3, 5, Message::TC, 0);
	enableStruct.appendByte(2);
	enableStruct.appendByte(1);
	enableStruct.appendByte(0);
	MessageParser::execute(enableStruct);

	map = Tester::returnMap(housekeepingService);
	it = map.find(1);
	CHECK(it->second.isPeriodic);
	it = map.find(0);
	CHECK(it->second.isPeriodic);

	// delete structure with id = 0
	Message deleteStruct = Message(3, 3, Message::TC, 0);
	deleteStruct.appendByte(1);
	deleteStruct.appendByte(0);
	MessageParser::execute(deleteStruct);

	timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 25);
	housekeepingService.checkAndSendHousekeepingReports(timeUtc);
	housekeepingService.checkAndSendHousekeepingReports(timeUtc);

	CHECK(ServiceTests::count() == 3);
	response = ServiceTests::get(2);
	CHECK(response.readUint8() == 1);
	CHECK(response.readUint32() == 9); // value of paramID = 3
	CHECK(response.readUint32() == 12); // value of paramID = 4
	CHECK(response.readUint32() == 15); // value of paramID = 5
}

TEST_CASE("TC[3,1] housekeeping structures", "[service][st03]") {
	SECTION("Create structures") {
		Services.reset();

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(0); // housekeeping ID = 0
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(2); // number of param IDs = 2
		request1.appendHalfword(1); // paramID = 1
		request1.appendHalfword(2); // paramID = 2

		Message request2(3, 1, Message::TC, 0);
		request2.appendByte(15); // housekeeping ID = 15
		request2.appendWord(4); // collection interval = 4
		request2.appendByte(4); // number of param IDs = 4
		request2.appendHalfword(3); // paramID = 3
		request2.appendHalfword(4); // paramID = 4
		request2.appendHalfword(5); // paramID = 5
		request2.appendHalfword(6); // paramID = 6

		Message request3(3, 1, Message::TC, 0);
		request3.appendByte(4); // housekeeping ID = 4
		request3.appendWord(4); // collection interval = 4
		request3.appendByte(5); // number of param IDs = 5
		request3.appendHalfword(3); // paramID = 3
		request3.appendHalfword(4); // paramID = 4
		request3.appendHalfword(5); // paramID = 5
		request3.appendHalfword(6); // paramID = 6
		request3.appendHalfword(7); // paramID = 7

		MessageParser::execute(request1);
		auto map = Tester::returnMap(housekeepingService);
		auto it = map.find(0);
		CHECK(map.size() == 1);
		CHECK(it->first == 0);
		CHECK(it->second.collectionInterval == 1);
		CHECK(it->second.paramId.size() == 2);
		CHECK(it->second.paramId[0] == 1);
		CHECK(it->second.paramId[1] == 2);

		MessageParser::execute(request2);
		map = Tester::returnMap(housekeepingService);
		it = map.find(15);
		CHECK(map.size() == 2);
		CHECK(it->first == 15);
		CHECK(it->second.collectionInterval == 4);
		CHECK(it->second.paramId.size() == 4);
		CHECK(it->second.paramId[0] == 3);
		CHECK(it->second.paramId[1] == 4);
		CHECK(it->second.paramId[2] == 5);
		CHECK(it->second.paramId[3] == 6);

		MessageParser::execute(request3);
		map = Tester::returnMap(housekeepingService);
		it = map.find(4);
		CHECK(map.size() == 3);
		CHECK(it->first == 4);
		CHECK(it->second.collectionInterval == 4);
		CHECK(it->second.paramId.size() == 5);
		CHECK(it->second.paramId[0] == 3);
		CHECK(it->second.paramId[1] == 4);
		CHECK(it->second.paramId[2] == 5);
		CHECK(it->second.paramId[3] == 6);
		CHECK(it->second.paramId[4] == 7);
	}

	SECTION("Trying to create a structure with the same id") {
		Services.reset();

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(1); // housekeeping ID = 1
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(2); // number of param IDs = 2
		request1.appendHalfword(1); // paramID = 1
		request1.appendHalfword(2); // paramID = 2

		Message request2(3, 1, Message::TC, 0);
		request2.appendByte(1); // housekeeping ID = 1
		request2.appendWord(2); // collection interval = 2
		request2.appendByte(3); // number of param IDs = 3
		request2.appendHalfword(3); // paramID = 3
		request2.appendHalfword(4); // paramID = 4
		request2.appendHalfword(5); // paramID = 5

		MessageParser::execute(request1);
		MessageParser::execute(request2);

		auto map = Tester::returnMap(housekeepingService);
		auto it = map.begin();
		CHECK(map.size() == 1);
		CHECK(it->first == 1);
		CHECK(it->second.collectionInterval == 1);
		CHECK(it->second.paramId.size() == 2);
		CHECK(it->second.paramId[0] == 1);
		CHECK(it->second.paramId[1] == 2);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UsedHousekeepingStructureId));
	}
}

TEST_CASE("TC[3,3] delete housekeeping structures", "[service][st03]") {
	SECTION("Delete existing structures") {
		// create structures
		Message request(3, 1, Message::TC, 0);
		request.appendByte(4); // housekeeping ID = 4
		request.appendWord(4); // collection interval = 4
		request.appendByte(5); // number of param IDs = 5
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(4); // paramID = 4
		request.appendHalfword(5); // paramID = 5
		request.appendHalfword(6); // paramID = 6
		request.appendHalfword(7); // paramID = 7
		MessageParser::execute(request);

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(5); // housekeeping ID = 5
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		auto map = Tester::returnMap(housekeepingService);
		CHECK(map.size() == 2);

		// create request to delete housekeeping structures
		Message request2(3, 3, Message::TC, 0);
		request2.appendByte(2); // number of structures that should be deleted
		request2.appendByte(4);
		request2.appendByte(5);
		MessageParser::execute(request2);

		map = Tester::returnMap(housekeepingService);
		CHECK(map.size() == 0);
	}

	SECTION("Try to delete an non-existent structure") {
		Services.reset();

		// create a structure
		Message request(3, 1, Message::TC, 0);
		request.appendByte(15); // housekeeping ID = 15
		request.appendWord(3); // collection interval = 3
		request.appendByte(1); // number of param IDs = 1
		request.appendHalfword(3); // paramID = 3
		MessageParser::execute(request);

		Message request1(3, 3, Message::TC, 0);
		request1.appendByte(1);
		request1.appendByte(1);
		MessageParser::execute(request1);

		auto map = Tester::returnMap(housekeepingService);
		CHECK(map.size() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::Unknownkey));
	}
}

TEST_CASE("TC[3, 5] enable periodic generation of housekeeping structures", "[services][st03]") {
	SECTION("Enable existing housekeeping structures") {
		// create a structure
		Message request(3, 1, Message::TC, 0);
		request.appendByte(5); // housekeeping ID = 5
		request.appendWord(1); // collection interval = 1
		request.appendByte(3); // number of param IDs = 3
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(4); // paramID = 4
		request.appendHalfword(5); // paramID = 5
		MessageParser::execute(request);

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(2); // housekeeping ID = 2
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		Message request2(3, 1, Message::TC, 0);
		request2.appendByte(7); // housekeeping ID = 7
		request2.appendWord(1); // collection interval = 1
		request2.appendByte(3); // number of param IDs = 3
		request2.appendHalfword(3); // paramID = 3
		request2.appendHalfword(4); // paramID = 4
		request2.appendHalfword(5); // paramID = 5
		MessageParser::execute(request2);

		// create request to enable structure
		Message request3(3, 5, Message::TC, 0);
		request3.appendByte(3);
		request3.appendByte(5);
		request3.appendByte(7);
		request3.appendByte(2);
		MessageParser::execute(request3);

		auto map = Tester::returnMap(housekeepingService);
		auto it = map.find(5);
		CHECK(it->second.isPeriodic);
		it = map.find(2);
		CHECK(it->second.isPeriodic);
		it = map.find(7);
		CHECK(it->second.isPeriodic);
	}

	SECTION("Enable non-existent housekeeping structures") {
		Services.reset();

		// create request to enable structure to an empty map
		Message request = Message(3, 5, Message::TC, 0);
		request.appendByte(2);
		request.appendByte(0);
		request.appendByte(1);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2); // two report errors should be generated
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::Unknownkey));
		MessageParser::execute(request);

		// create a structure
		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(5); // housekeeping ID = 5
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		// create request to enable structure with non-existent Id
		Message request2 = Message(3, 5, Message::TC, 0);
		request2.appendByte(1);
		request2.appendByte(2);
		MessageParser::execute(request2);

		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::Unknownkey));
	}
}

TEST_CASE("TC[3, 6] disable periodic generation of housekeeping structures", "[services][st03]") {
	SECTION("Disable existing housekeeping structures") {
		// create a structure
		Message request(3, 1, Message::TC, 0);
		request.appendByte(5); // housekeeping ID = 5
		request.appendWord(1); // collection interval = 1
		request.appendByte(3); // number of param IDs = 3
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(4); // paramID = 4
		request.appendHalfword(5); // paramID = 5
		MessageParser::execute(request);

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(2); // housekeeping ID = 2
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		Message request2(3, 1, Message::TC, 0);
		request2.appendByte(7); // housekeeping ID = 7
		request2.appendWord(1); // collection interval = 1
		request2.appendByte(3); // number of param IDs = 3
		request2.appendHalfword(3); // paramID = 3
		request2.appendHalfword(4); // paramID = 4
		request2.appendHalfword(5); // paramID = 5
		MessageParser::execute(request2);

		// create request to enable structures
		Message request3(3, 5, Message::TC, 0);
		request3.appendByte(3);
		request3.appendByte(5);
		request3.appendByte(7);
		request3.appendByte(2);
		MessageParser::execute(request3);

		auto map = Tester::returnMap(housekeepingService);
		auto it = map.find(5);
		CHECK(it->second.isPeriodic);
		it = map.find(2);
		CHECK(it->second.isPeriodic);
		it = map.find(7);
		CHECK(it->second.isPeriodic);

		// create request to disable structures
		Message request4(3, 6, Message::TC, 0);
		request4.appendByte(2);
		request4.appendByte(5);
		request4.appendByte(2);
		MessageParser::execute(request4);

		map = Tester::returnMap(housekeepingService);
		it = map.find(5);
		CHECK_FALSE(it->second.isPeriodic);
		it = map.find(2);
		CHECK_FALSE(it->second.isPeriodic);
		it = map.find(7);
		CHECK(it->second.isPeriodic);
	}

	SECTION("Enable non-existent housekeeping structures") {
		Services.reset();

		// create request to enable structure to an empty map
		Message request = Message(3, 5, Message::TC, 0);
		request.appendByte(2);
		request.appendByte(0);
		request.appendByte(1);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2); // two report errors should be generated
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::Unknownkey));
		MessageParser::execute(request);

		// create a structure
		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(5); // housekeeping ID = 5
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		// create request to enable structure with non-existent Id
		Message request2 = Message(3, 5, Message::TC, 0);
		request2.appendByte(1);
		request2.appendByte(2);
		MessageParser::execute(request2);

		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::Unknownkey));
	}
}

TEST_CASE("TM[3,25] housekeeping parameter report", "[service][st03]") {
	SECTION("Generation of param reports using one structure ID") {
		// create structure
		Message request(3, 1, Message::TC, 0);
		request.appendByte(4); // housekeeping ID = 4
		request.appendWord(4); // collection interval = 4
		request.appendByte(5); // number of param IDs = 5
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(4); // paramID = 4
		request.appendHalfword(5); // paramID = 5
		request.appendHalfword(6); // paramID = 6
		request.appendHalfword(7); // paramID = 7
		MessageParser::execute(request);

		// Add parameters
		Parameter param0 = Parameter(1, 2, 3);
		Parameter param1 = Parameter(4, 5, 6);
		Parameter param2 = Parameter(7, 8, 9);
		Parameter param3 = Parameter(10, 11, 12);
		Parameter param4 = Parameter(13, 14, 15);

		paramService.addNewParameter(3, param0); // paramID = 3, value = 3
		paramService.addNewParameter(4, param1); // paramID = 4, value = 6
		paramService.addNewParameter(5, param2); // paramID = 5, value = 9
		paramService.addNewParameter(6, param3); // paramID = 6, value = 12
		paramService.addNewParameter(7, param4); // paramID = 7, value = 15

		// enable structures
		Message enable = Message(3, 5, Message::TC, 0);
		enable.appendByte(1);
		enable.appendByte(4);
		MessageParser::execute(enable);

		TimeAndDate timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 10);
		housekeepingService.checkAndSendHousekeepingReports(timeUtc);

		CHECK(ServiceTests::count() == 1);
		Message response = ServiceTests::get(0);
		CHECK(response.readUint8() == 4); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);
		CHECK(response.readUint32() == 12);
		CHECK(response.readUint32() == 15);

		timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 12); // nothing should happen
		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 1); // the count of messages should be the same

		timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 14);
		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 2);

		response = ServiceTests::get(1);
		CHECK(response.readUint8() == 4); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);
		CHECK(response.readUint32() == 12);
		CHECK(response.readUint32() == 15);
	}

	SECTION("Request to fetch the value of a parameter  that doesn't exist") {
		Services.reset();
		ServiceTests::reset();

		// create structure
		Message request(3, 1, Message::TC, 0);
		request.appendByte(4); // housekeeping ID = 4
		request.appendWord(4); // collection interval = 4
		request.appendByte(2); // number of param IDs = 5
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(38); // paramID doesn't exist
		MessageParser::execute(request);

		// Add parameters
		Parameter param0 = Parameter(1, 2, 3);
		Parameter param1 = Parameter(4, 5, 6);
		Parameter param2 = Parameter(7, 8, 9);
		Parameter param3 = Parameter(10, 11, 12);
		Parameter param4 = Parameter(13, 14, 15);

		paramService.addNewParameter(3, param0); // paramID = 3, value = 3
		paramService.addNewParameter(4, param1); // paramID = 4, value = 6
		paramService.addNewParameter(5, param2); // paramID = 5, value = 9
		paramService.addNewParameter(6, param3); // paramID = 6, value = 12
		paramService.addNewParameter(7, param4); // paramID = 7, value = 15

		// enable structures
		Message enable = Message(3, 5, Message::TC, 0);
		enable.appendByte(1);
		enable.appendByte(4);
		MessageParser::execute(enable);

		TimeAndDate timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 10);
		housekeepingService.checkAndSendHousekeepingReports(timeUtc);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::Unknownkey));
		Message response = ServiceTests::get(1);
		CHECK(response.readUint8() == 4); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
	}

	SECTION("Generation of param report using multiple housekeeping IDs") {
		Services.reset();
		ServiceTests::reset();

		Message request(3, 1, Message::TC, 0);
		request.appendByte(4); // housekeeping ID = 4
		request.appendWord(4); // collection interval = 4
		request.appendByte(5); // number of param IDs = 5
		request.appendHalfword(3); // paramID = 3
		request.appendHalfword(4); // paramID = 4
		request.appendHalfword(5); // paramID = 5
		request.appendHalfword(6); // paramID = 6
		request.appendHalfword(7); // paramID = 7
		MessageParser::execute(request);

		Message request1(3, 1, Message::TC, 0);
		request1.appendByte(5); // housekeeping ID = 5
		request1.appendWord(1); // collection interval = 1
		request1.appendByte(3); // number of param IDs = 3
		request1.appendHalfword(3); // paramID = 3
		request1.appendHalfword(4); // paramID = 4
		request1.appendHalfword(5); // paramID = 5
		MessageParser::execute(request1);

		Message request2(3, 1, Message::TC, 0);
		request2.appendByte(15); // housekeeping ID = 15
		request2.appendWord(3); // collection interval = 3
		request2.appendByte(1); // number of param IDs = 1
		request2.appendHalfword(3); // paramID = 3
		MessageParser::execute(request2);

		// Add parameters
		Parameter param0 = Parameter(1, 2, 3);
		Parameter param1 = Parameter(4, 5, 6);
		Parameter param2 = Parameter(7, 8, 9);
		Parameter param3 = Parameter(10, 11, 12);
		Parameter param4 = Parameter(13, 14, 15);

		paramService.addNewParameter(3, param0); // paramID = 3, value = 3
		paramService.addNewParameter(4, param1); // paramID = 4, value = 6
		paramService.addNewParameter(5, param2); // paramID = 5, value = 9
		paramService.addNewParameter(6, param3); // paramID = 6, value = 12
		paramService.addNewParameter(7, param4); // paramID = 7, value = 15

		// enable structures
		Message enable = Message(3, 5, Message::TC, 0);
		enable.appendByte(3);
		enable.appendByte(4);
		enable.appendByte(5);
		enable.appendByte(15);
		MessageParser::execute(enable);

		TimeAndDate timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 10);
		housekeepingService.checkAndSendHousekeepingReports(timeUtc);

		// keep time the same and call the checkAndSendHousekeepingReports. All structures IDs should be used because
		// its their first time

		CHECK(ServiceTests::count() == 3);
		Message response = ServiceTests::get(0);
		CHECK(response.readUint8() == 4); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);
		CHECK(response.readUint32() == 12);
		CHECK(response.readUint32() == 15);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 3);
		response = ServiceTests::get(1);
		CHECK(response.readUint8() == 5); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 3);
		response = ServiceTests::get(2);
		CHECK(response.readUint8() == 15); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);

		// should do nothing because the time is the same

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 3);

		timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 11); // add 1 second since the initial time

		// only the param report of the housekeeping structure with collection interval 1 should be generated

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 4);
		response = ServiceTests::get(3);
		CHECK(response.readUint8() == 5); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 4);

		timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 13); // add 3 seconds since the initial time

		// the param reports of the housekeeping structures with collection interval 1 and 3 should be generated

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		response = ServiceTests::get(4);
		CHECK(response.readUint8() == 5); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		response = ServiceTests::get(5);
		CHECK(response.readUint8() == 15); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);

		timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 16); // add 6 seconds since the initial time

		// the param reports of all the housekeeping structure should be generated

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		response = ServiceTests::get(6);
		CHECK(response.readUint8() == 4); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);
		CHECK(response.readUint32() == 12);
		CHECK(response.readUint32() == 15);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		response = ServiceTests::get(7);
		CHECK(response.readUint8() == 5); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
		CHECK(response.readUint32() == 6);
		CHECK(response.readUint32() == 9);

		housekeepingService.checkAndSendHousekeepingReports(timeUtc);
		CHECK(ServiceTests::count() == 9);
		response = ServiceTests::get(8);
		CHECK(response.readUint8() == 15); // housekeeping ID
		// check param values
		CHECK(response.readUint32() == 3);
	}
}
