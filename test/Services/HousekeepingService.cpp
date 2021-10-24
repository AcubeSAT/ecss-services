#include <iostream>
#include "catch2/catch.hpp"
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"

TEST_CASE("Housekeeping Reporting Sub-service") {
	SECTION("Create housekeeping structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure,Message::TC,1);
		uint16_t idToCreate = 2;
		uint16_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		uint16_t simplyCommutatedIds[3] = {1, 4, 5};
		uint16_t numOfSets = 2;
		etl::map <uint16_t, etl::vector <uint16_t, 5>, 2> superCommutatedIds;
		superCommutatedIds.insert({4, {2, 3}});     //Num of samples followed by the list of IDs
		superCommutatedIds.insert({9, {6, 7, 8}});

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
		CHECK(ServiceTests::count() == 0);
//		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];
		std::cout<<housekeepingService.housekeepingStructures.size()<<std::endl;
//		for(auto &i : housekeepingService.housekeepingStructures){
//			std::cout<<i.second.get().structureId<<std::endl;
//		}

//		CHECK(housekeepingService.housekeepingStructures[idToCreate].structureId == idToCreate);
//		CHECK(newStruct.numOfSimplyCommutatedParams == numOfSimplyCommutatedParams);
//		CHECK(newStruct.numOfSuperCommutatedParameterSets == numOfSets);
		uint16_t allIds[8] = {1, 2, 3, 4, 5, 6, 7, 8};
//		for (auto &id : allIds) {
//			CHECK(housekeepingService.existsInVector(id, newStruct.containedParameterIds));
//		}
	}

//	SECTION("Housekeeping parameter reporting") {
//		Message request(HousekeepingService::ServiceType,
//		                HousekeepingService::MessageType::ReportHousekeepingParameters,Message::TC,1);
//
//	}
}