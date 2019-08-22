#include "catch2/catch.hpp"
#include "ServicePool.hpp"
#include "ServiceTests.hpp"

struct Tester {
	auto returnMap(HousekeepingService& hsService) {
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
	Tester test;
	auto map = test.returnMap(housekeepingService);
	CHECK(map.find(0) != map.end());
	CHECK(map.size() == 1);
	auto it = map.begin();
	CHECK(it->first == 0);
	CHECK(it->second.collectInterval == 1);
	CHECK(it->second.paramId.size() == 2);
	CHECK(it->second.paramId[0] == 1);
	CHECK(it->second.paramId[1] == 2);
	CHECK_FALSE(it->second.periodicStatus);

	Message request2(3, 1, Message::TC, 0);
	request2.appendByte(1); // housekeeping ID = 1
	request2.appendWord(2); // collection interval = 2
	request2.appendByte(3); // number of param IDs = 3
	request2.appendHalfword(3); // paramID = 3
	request2.appendHalfword(4); // paramID = 4
	request2.appendHalfword(5); // paramID = 5


	MessageParser::execute(request2);

	map = test.returnMap(housekeepingService);
	CHECK(map.find(1) != map.end());
	CHECK(map.size() == 2);
	CHECK(it->first == 0);
	CHECK(it->second.collectInterval == 1);
	CHECK(it->second.paramId.size() == 2);
	CHECK(it->second.paramId[0] == 1);
	CHECK(it->second.paramId[1] == 2);
	CHECK_FALSE(it->second.periodicStatus);

	it++;

	CHECK(it->first == 1);
	CHECK(it->second.collectInterval == 2);
	CHECK(it->second.paramId.size() == 3);
	CHECK(it->second.paramId[0] == 3);
	CHECK(it->second.paramId[1] == 4);
	CHECK(it->second.paramId[2] == 5);
	CHECK_FALSE(it->second.periodicStatus);

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

	TimeAndDate timeUtc = TimeAndDate(2020, 4, 10, 10, 45, 10);
	//uint32_t timeSeconds = TimeHelper::utcToSeconds(timeUtc); // for fun
	housekeepingService.paramReport(timeUtc); // should generate param report based on the structure with id 0


	Message response = ServiceTests::get(0);

	CHECK(ServiceTests::hasOneMessage());
	CHECK(response.readUint8() == 0);
	CHECK(response.readUint32() == 3);
	CHECK(response.readUint32() == 6);

	// same time

	housekeepingService.paramReport(timeUtc); // should generate param report based on the structure with id 1

	response = ServiceTests::get(1);

	CHECK(response.readUint8() == 1);
	CHECK(response.readUint32() == 9);
	CHECK(response.readUint32() == 12);
	CHECK(response.readUint32() == 15);
}

TEST_CASE("TC[3,1] housekeeping structures", "[service][st03]") {
	SECTION("Trying to create a structure with the same id") {
		// do stuff
	}
}

TEST_CASE("TM[3,25] housekeeping parameter report", "[service][st03]") {
	SECTION("") {
		// do stuff
	}
}
