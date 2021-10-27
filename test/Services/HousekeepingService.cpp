#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"

HousekeepingService& housekeepingService = Services.housekeeping;

/**
 * Helper function that forms the message that's going to be sent as request to create a new housekeeping structure.
 */
void buildRequest(Message& request, uint16_t idToCreate) {
	uint16_t interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector <uint16_t, 50> simplyCommutatedIds = {1, 4, 5};
	uint16_t numOfSets = 2;
	etl::vector <std::pair<uint16_t, etl::vector <uint16_t, 5>>, 2> superCommutatedIds;
	etl::vector <uint16_t, 5> temp1 = {2, 3};
	etl::vector <uint16_t, 5> temp2 = {6, 7, 8};
	superCommutatedIds.push_back(std::make_pair(4, temp1));     //Num of samples followed by the list of IDs
	superCommutatedIds.push_back(std::make_pair(9, temp2));

	request.appendUint16(idToCreate);
	request.appendUint16(interval);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto &id : simplyCommutatedIds) {
		request.appendUint16(id);
	}
	request.appendUint16(numOfSets);
	for (auto &set : superCommutatedIds) {
		request.appendUint16(set.first);
		request.appendUint16(set.second.size());
		for (auto &id : set.second) {
			request.appendUint16(id);
		}
	}
}

TEST_CASE("Housekeeping Reporting Sub-service") {
	SECTION("Create housekeeping structure") {
		// Valid structure creation request
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t idToCreate = 2;
		uint16_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector <uint16_t, 50> simplyCommutatedIds = {1, 4, 5};
		uint16_t numOfSets = 2;
		etl::vector <std::pair<uint16_t, etl::vector <uint16_t, 5>>, 2> superCommutatedIds;
		etl::vector <uint16_t, 5> temp1 = {2, 3};
		etl::vector <uint16_t, 5> temp2 = {6, 7, 8};
		superCommutatedIds.push_back(std::make_pair(4, temp1));     //Num of samples followed by the list of IDs
		superCommutatedIds.push_back(std::make_pair(9, temp2));

		request.appendUint16(idToCreate);
		request.appendUint16(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto &id : simplyCommutatedIds) {
			request.appendUint16(id);
		}
		request.appendUint16(numOfSets);
		for (auto &set : superCommutatedIds) {
			request.appendUint16(set.first);
			request.appendUint16(set.second.size());
			for (auto &id : set.second) {
				request.appendUint16(id);
			}
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];
		uint16_t allIds[8] = {1, 2, 3, 4, 5, 6, 7, 8};

		CHECK(ServiceTests::count() == 0);
		CHECK(newStruct.structureId == idToCreate);
		CHECK(newStruct.numOfSimplyCommutatedParams == numOfSimplyCommutatedParams);
		CHECK(newStruct.numOfSuperCommutatedParameterSets == numOfSets);
		CHECK(newStruct.collectionInterval == interval);
		for (auto &id : allIds) {
			CHECK(housekeepingService.existsInVector(id, newStruct.containedParameterIds));
		}
		for (auto &id : newStruct.simplyCommutatedIds) {
			CHECK(housekeepingService.existsInVector(id, simplyCommutatedIds));
		}
		for (int set = 0; set < newStruct.numOfSuperCommutatedParameterSets; set++) {
			CHECK(newStruct.superCommutatedIds[set].first == superCommutatedIds[set].first);
			for(int id = 0; id < newStruct.superCommutatedIds[set].second.size(); id++) {
				CHECK(newStruct.superCommutatedIds[set].second[id] == superCommutatedIds[set].second[id]);
			}
		}
		// Invalid structure creation request
		Message request2(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t idToCreate2 = 2;
		request2.appendUint16(idToCreate2);
		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 1);
	}

	SECTION("Delete housekeeping structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure,Message::TC,1);
		uint16_t numOfStructs = 5;
		uint16_t ids[5] = {2, 3, 4, 7, 8};
		request.appendUint16(numOfStructs);
		for (auto &id : ids) {
			request.appendUint16(id);
		}
		//Add periodic structure
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 5);  //4 + 1 from previous test which did not reset the system.
		CHECK(ServiceTests::countThrownErrors
		      (ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) == 3);
		CHECK(ServiceTests::countThrownErrors
		      (ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfPeriodicStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enable periodic generation of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t ids[3] = {0, 4, 6};
		for (auto &id : ids) {              //Create 3 structures first, these will be used by next test cases as well.
			buildRequest(request, id);
			MessageParser::execute(request);
		}
		CHECK(housekeepingService.housekeepingStructures.size() == 3);
		Message request2(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,Message::TC,1);
		uint16_t numOfStructs = 5;
		uint16_t idsToEnable[5] = {1, 3, 4, 6, 7};
		request2.appendUint16(numOfStructs);
		for (auto &id : idsToEnable) {
			request2.appendUint16(id);
		}
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) == 3);

		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);
	}

	SECTION("Disable periodic generation of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,Message::TC,1);
		uint16_t numOfStructs = 4;
		uint16_t idsToDisable[4] = {0, 1, 4, 6};
		request.appendUint16(numOfStructs);
		for (auto &id : idsToDisable) {
			request.appendUint16(id);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 4);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);
	}

	SECTION("Reporting of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingStructures,Message::TC,1);
		uint16_t numOfStructs = 3;
		uint16_t idsToReport[3] = {9, 4, 2};
		request.appendUint16(numOfStructs);
		for (auto &id : idsToReport) {
			request.appendUint16(id);
		}
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 7);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure)== 6);

		Message report = ServiceTests::get(5); //Both 4 and 6 are the error messages because only id=4 was valid.
		uint16_t validId = 4;
		CHECK(report.readUint16() == validId);
		CHECK(not report.readBoolean());    //periodic status
		CHECK(report.readUint16() == 7);    //interval
		CHECK(report.readUint16() == 3);    //number of simply commutated ids
		CHECK(report.readUint16() == 1);    //ids
		CHECK(report.readUint16() == 4);
		CHECK(report.readUint16() == 5);
		CHECK(report.readUint16() == 2);    //number of super sets
		//Set-1
		CHECK(report.readUint16() == 4);    //number of samples
		CHECK(report.readUint16() == 2);    //number of ids
		CHECK(report.readUint16() == 2);    //ids
		CHECK(report.readUint16() == 3);
		//Set-2
		CHECK(report.readUint16() == 9);    //number of samples
		CHECK(report.readUint16() == 3);    //number of ids
		CHECK(report.readUint16() == 6);    //ids
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 8);
	}
}