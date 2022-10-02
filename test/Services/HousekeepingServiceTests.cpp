#include <iostream>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"
#include "catch2/catch_all.hpp"
#include "etl/algorithm.h"

HousekeepingService& housekeepingService = Services.housekeeping;

/**
 * Helper function that forms the message that's going to be sent as request to create a new housekeeping structure.
 */
void buildRequest(Message& request, uint8_t idToCreate) {
	uint32_t interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};

	request.appendUint8(idToCreate);
	request.appendUint32(interval);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id: simplyCommutatedIds) {
		request.appendUint16(id);
	}
}

/**
 * Stub function to define the HousekeepingService constructor during tests
 */
void HousekeepingService::initializeHousekeepingStructures() {
}

/**
 * Initializes 3 housekeeping structures with IDs = {0, 4, 6}
 */
void initializeHousekeepingStructures() {
	uint8_t ids[3] = {0, 4, 6};
	uint32_t interval = 7;
	etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};

	HousekeepingStructure structures[3];
	int i = 0;
	for (auto& newStructure: structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (uint16_t parameterId: simplyCommutatedIds) {
			newStructure.simplyCommutatedParameterIds.push_back(parameterId);
		}
		housekeepingService.housekeepingStructures.insert({ids[i], newStructure});
		i++;
	}

	REQUIRE(housekeepingService.housekeepingStructures.size() == 3);
	REQUIRE(housekeepingService.housekeepingStructures.find(0) != housekeepingService.housekeepingStructures.end());
	REQUIRE(housekeepingService.housekeepingStructures.find(4) != housekeepingService.housekeepingStructures.end());
	REQUIRE(housekeepingService.housekeepingStructures.find(6) != housekeepingService.housekeepingStructures.end());
}

/**
 * Helper function that stores samples into simply commutated parameters of different data type each.
 */
void storeSamplesToParameters(uint16_t id1, uint16_t id2, uint16_t id3) {
	static_cast<Parameter<uint16_t>&>(Services.parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(Services.parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint32_t>&>(Services.parameterManagement.getParameter(id3)->get()).setValue(99);
}

/**
 * Helper function that forms the request to append new parameters to a housekeeping structure
 */
void appendNewParameters(Message& request, uint8_t idToAppend) {
	uint16_t numOfSimplyCommutatedParams = 7;
	etl::vector<uint16_t, 7> simplyCommutatedIds = {8, 4, 5, 9, 11, 10, 220};

	request.appendUint8(idToAppend);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id: simplyCommutatedIds) {
		request.appendUint16(id);
	}
}

TEST_CASE("Create housekeeping structure") {
	SECTION("Valid structure creation request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		uint8_t idToCreate = 2;
		uint32_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::array<uint16_t, 3> simplyCommutatedIds = {4, 5, 8};

		request.appendUint8(idToCreate);
		request.appendUint32(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.appendUint16(id);
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		CHECK(ServiceTests::count() == 0);
		CHECK(newStruct.structureId == idToCreate);
		CHECK(newStruct.simplyCommutatedParameterIds.size() == numOfSimplyCommutatedParams);
		CHECK(newStruct.collectionInterval == interval);
		CHECK(std::equal(newStruct.simplyCommutatedParameterIds.begin(), newStruct.simplyCommutatedParameterIds.end(),
		                 simplyCommutatedIds.begin()));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure creation request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		uint8_t structureId = 9;
		HousekeepingStructure structure1;
		structure1.structureId = structureId;
		housekeepingService.housekeepingStructures.insert({structureId, structure1});

		uint8_t idToCreate = 9;
		request.appendUint8(idToCreate);

		MessageParser::execute(request);

		CHECK(housekeepingService.housekeepingStructures.size() == 1);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 1);
		housekeepingService.housekeepingStructures.erase(structureId);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Exceeding max number of housekeeping structures") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);

		uint8_t idsToCreate[16] = {1, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<uint16_t, 3> simplyCommutatedIds = {8, 4, 5};
		uint32_t interval = 12;

		REQUIRE(housekeepingService.housekeepingStructures.size() == 0);

		for (auto& structId: idsToCreate) {
			request.appendUint8(structId);
			request.appendUint32(interval);
			request.appendUint16(numOfSimplyCommutatedParams);
			for (auto& parameterId: simplyCommutatedIds) {
				request.appendUint16(parameterId);
			}
			MessageParser::execute(request);
		}

		REQUIRE(housekeepingService.housekeepingStructures.size() == 10);
		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures) == 5);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Same simply commutated parameter twice in the request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		uint8_t idToCreate = 2;
		uint32_t interval = 7;
		uint16_t numOfSimplyCommutatedParams = 9;
		etl::vector<uint16_t, 9> simplyCommutatedIds = {8, 4, 5, 4, 8, 8, 5, 4, 11};

		request.appendUint8(idToCreate);
		request.appendUint32(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.appendUint16(id);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 5);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		REQUIRE(newStruct.simplyCommutatedParameterIds.size() == 4);
		uint16_t existingParameterIds[4] = {8, 4, 5, 11};
		for (auto parameterId: newStruct.simplyCommutatedParameterIds) {
			CHECK(std::find(std::begin(existingParameterIds), std::end(existingParameterIds), parameterId) !=
			      std::end(existingParameterIds));
		}

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delete housekeeping structure") {
	SECTION("One valid request, the rest invalid") {
		Message createStruct(HousekeepingService::ServiceType,
		                     HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);

		buildRequest(createStruct, 2);
		MessageParser::execute(createStruct);

		REQUIRE(housekeepingService.housekeepingStructures.size() == 1);
		REQUIRE(housekeepingService.housekeepingStructures.find(2) != housekeepingService.housekeepingStructures.end());
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);

		uint8_t numOfStructs = 5;
		uint8_t ids[5] = {2, 3, 4, 7, 8};
		request.appendUint8(numOfStructs);
		for (auto& id: ids) {
			request.appendUint8(id);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      4);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid request of periodic structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		uint8_t numOfStructs = 1;
		uint8_t structureId = 4;
		request.appendUint8(numOfStructs);
		request.appendUint8(structureId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfEnabledHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Enable the periodic generation of housekeeping structures") {
	SECTION("Both valid and invalid structure IDs in same request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		uint8_t numOfStructs = 5;
		uint8_t idsToEnable[5] = {1, 3, 4, 6, 7};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToEnable) {
			request2.appendUint8(id);
		}
		REQUIRE(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		REQUIRE(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		REQUIRE(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		MessageParser::execute(request2);
		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disable the periodic generation of housekeeping structures") {
	SECTION("Both valid and invalid structure IDs in request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport, Message::TC, 1);
		uint8_t numOfStructs = 4;
		uint8_t idsToDisable[4] = {0, 1, 4, 6};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToDisable) {
			request2.appendUint8(id);
		}
		housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus = true;

		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      1);
		CHECK(not housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[4].periodicGenerationActionStatus);
		CHECK(not housekeepingService.housekeepingStructures[6].periodicGenerationActionStatus);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structures") {
	SECTION("Both valid and invalid structure IDs in request") {
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::ReportHousekeepingStructures, Message::TC, 1);
		uint8_t numOfStructs = 3;
		uint8_t idsToReport[3] = {9, 4, 2};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToReport) {
			request2.appendUint8(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      2);

		Message report = ServiceTests::get(1); // Both 0 and 2 are the error messages because only id=4 was valid.

		uint8_t validId = 4;
		CHECK(report.readUint8() == validId);
		CHECK(not report.readBoolean()); // periodic status
		CHECK(report.readUint32() == 7); // interval
		CHECK(report.readUint16() == 3); // number of simply commutated ids
		CHECK(report.readUint16() == 8);
		CHECK(report.readUint16() == 4); // ids
		CHECK(report.readUint16() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structures without a TC message as argument") {
	SECTION("Check the report message generated by the non-TC function") {
		initializeHousekeepingStructures();
		uint8_t structureId = 4;

		housekeepingService.housekeepingStructureReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingStructuresReport);
		CHECK(report.readUint8() == structureId);
		CHECK(not report.readBoolean());
		CHECK(report.readUint32() == 7);
		CHECK(report.readUint16() == 3);
		CHECK(report.readUint16() == 8);
		CHECK(report.readUint16() == 4);
		CHECK(report.readUint16() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping parameters") {
	SECTION("Valid structure request") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		uint8_t structId = 6;

		housekeepingService.housekeepingParametersReport(structId);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.readUint8() == structId);
		CHECK(report.readUint16() == 33);
		CHECK(report.readUint8() == 77);
		CHECK(report.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure request") {
		uint8_t structId = 8;
		housekeepingService.housekeepingParametersReport(structId);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		structId = 12;
		housekeepingService.housekeepingParametersReport(structId);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 2);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping parameters without a TC request") {
	SECTION("Check the report message generated by the non-TC function") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		uint8_t structureId = 6;

		housekeepingService.housekeepingParametersReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.readUint8() == structureId);
		CHECK(report.readUint16() == 33);
		CHECK(report.readUint8() == 77);
		CHECK(report.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("One-shot housekeeping parameter report generation") {
	SECTION("Both valid and invalid structure IDs in request") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::GenerateOneShotHousekeepingReport, Message::TC, 1);
		uint8_t numOfStructs = 5;
		uint8_t structIds[5] = {0, 4, 7, 8, 11};
		request2.appendUint8(numOfStructs);
		for (auto& id: structIds) {
			request2.appendUint8(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);
		Message report1 = ServiceTests::get(0);
		Message report2 = ServiceTests::get(1);

		REQUIRE(report1.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report2.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);

		REQUIRE(report1.readUint8() == 0);
		CHECK(report1.readUint16() == 33);
		CHECK(report1.readUint8() == 77);
		CHECK(report1.readUint32() == 99);

		REQUIRE(report2.readUint8() == 4);
		CHECK(report2.readUint16() == 33);
		CHECK(report2.readUint8() == 77);
		CHECK(report2.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Append parameters in housekeeping report structure") {
	SECTION("Non existing structure") {
		initializeHousekeepingStructures();
		Message request1(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 2;
		request1.appendUint8(structId);
		MessageParser::execute(request1);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Enabled structure") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		// Enable 1 periodic struct with id=0
		HousekeepingStructure newStruct;
		newStruct.structureId = 0;
		newStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({0, newStruct});

		request.appendUint8(1);
		request.appendUint8(0);
		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 0;
		request2.appendUint8(structId);
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Valid request including both valid and invalid parameters") {
		initializeHousekeepingStructures();
		Message request3(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		uint8_t structId = 6;
		appendNewParameters(request3, structId);
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 3);

		MessageParser::execute(request3);
		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::GetNonExistingParameter) == 1);

		uint16_t currentlyExistingParameters[] = {8, 4, 5, 9, 10, 11};
		HousekeepingStructure structToCheck = housekeepingService.housekeepingStructures[structId];
		REQUIRE(structToCheck.simplyCommutatedParameterIds.size() == 6);
		for (auto& existingParameter: currentlyExistingParameters) {
			CHECK(std::find(std::begin(structToCheck.simplyCommutatedParameterIds),
			                std::end(structToCheck.simplyCommutatedParameterIds),
			                existingParameter) != std::end(structToCheck.simplyCommutatedParameterIds));
		}
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Exceeding the maximum number of simply commutated parameters for the specified structure") {
		initializeHousekeepingStructures();
		uint8_t structId = 6;
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);

		uint16_t numOfSimplyCommutatedParams = 34;

		etl::vector<uint16_t, 34> simplyCommutatedIds;
		for (uint16_t i = 0; i < 34; i++) {
			simplyCommutatedIds.push_back(i);
		}

		request.appendUint8(structId);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.appendUint16(id);
		}
		REQUIRE(housekeepingService.housekeepingStructures.find(structId) !=
		        housekeepingService.housekeepingStructures.end());
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 3);

		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 30);
		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 3);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Modification of housekeeping structures' interval") {
	SECTION("Both valid and invalid requests") {
		initializeHousekeepingStructures();
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ModifyCollectionIntervalOfStructures, Message::TC, 1);
		uint8_t numOfStructs = 4;
		uint8_t structIds[4] = {0, 4, 9, 10};
		uint32_t intervals[4] = {12, 21, 32, 17};
		request.appendUint8(numOfStructs);
		int i = 0;
		for (auto& id: structIds) {
			request.appendUint8(id);
			request.appendUint32(intervals[i++]);
		}
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      2);
		REQUIRE(housekeepingService.housekeepingStructures[0].collectionInterval == 12);
		REQUIRE(housekeepingService.housekeepingStructures[4].collectionInterval == 21);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structure periodic properties") {
	SECTION("Both valid and invalid structure IDs in request") {
		initializeHousekeepingStructures();
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::ReportHousekeepingPeriodicProperties, Message::TC, 1);
		uint8_t numOfStructs = 6;
		uint8_t structIds[6] = {0, 4, 1, 6, 9, 10};
		request.appendUint8(numOfStructs);
		for (auto& id: structIds) {
			request.appendUint8(id);
		}
		housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[4].collectionInterval = 24;
		housekeepingService.housekeepingStructures[6].collectionInterval = 13;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);

		Message report = ServiceTests::get(3);
		CHECK(report.readUint8() == 3);       // Number of valid ids
		CHECK(report.readUint8() == 0);       // Id
		CHECK(report.readBoolean() == true);  // Periodic status
		CHECK(report.readUint32() == 7);      // Interval
		CHECK(report.readUint8() == 4);       // Id
		CHECK(report.readBoolean() == false); // Periodic status
		CHECK(report.readUint32() == 24);     // Interval
		CHECK(report.readUint8() == 6);       // Id
		CHECK(report.readBoolean() == false); // Periodic status
		CHECK(report.readUint32() == 13);     // Interval

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Periodically reporting Housekeeping Structures") {
	uint32_t nextCollection = 0;
	uint32_t currentTime = 0;
	uint32_t previousTime = 0;
	SECTION("Non existent structures") {
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(ServiceTests::count() == 0);
		CHECK(nextCollection == std::numeric_limits<uint32_t>::max());
	}
	SECTION("Collection Intervals set to max") {
		initializeHousekeepingStructures();
		for (auto& housekeepingStructure: housekeepingService.housekeepingStructures) {
			housekeepingStructure.second.collectionInterval = std::numeric_limits<uint32_t>::max();
		}
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(ServiceTests::count() == 0);
		CHECK(nextCollection == std::numeric_limits<uint32_t>::max());
	}
	SECTION("Calculating properly defined collection intervals") {
		housekeepingService.housekeepingStructures.at(0).collectionInterval = 900;
		housekeepingService.housekeepingStructures.at(4).collectionInterval = 1000;
		housekeepingService.housekeepingStructures.at(6).collectionInterval = 2700;
		housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.at(4).periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.at(6).periodicGenerationActionStatus = true;

		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(currentTime == 900);
		CHECK(ServiceTests::count() == 0);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(currentTime == 1000);
		CHECK(ServiceTests::count() == 1);
		currentTime += 6;
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(currentTime == 1800);
		CHECK(ServiceTests::count() == 2);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(ServiceTests::count() == 3);
		CHECK(currentTime == 2000);
		currentTime += 15;
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(ServiceTests::count() == 4);
		CHECK(currentTime == 2700);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime += nextCollection;
		CHECK(ServiceTests::count() == 6);
		CHECK(currentTime == 3000);
	}
	SECTION("Collection Intervals set to 0") {
		for (auto& housekeepingStructure: housekeepingService.housekeepingStructures) {
			housekeepingStructure.second.periodicGenerationActionStatus = true;
			housekeepingStructure.second.collectionInterval = 0;
		}
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(nextCollection == 0);
	}
}