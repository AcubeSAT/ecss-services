#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"

HousekeepingService& housekeepingService = Services.housekeeping;

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

	SECTION("Housekeeping parameter reporting") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingParameters,Message::TC,1);

	}
}