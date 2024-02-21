#include <iostream>
#include "Message.hpp"
#include "ServiceTests.hpp"
#include "Services/HousekeepingService.hpp"
#include "Time/TimeStamp.hpp"
#include "catch2/catch_all.hpp"
#include "etl/algorithm.h"

HousekeepingService& housekeepingService = Services.housekeeping;

/**
 * Helper function that forms the message that's going to be sent as request to create a new housekeeping structure.
 */
void buildRequest(Message& request, ParameterReportStructureId idToCreate) {
	CollectionInterval interval = 7;
	uint16_t numOfSimplyCommutatedParams = 3;
	etl::vector<ParameterId, 3> simplyCommutatedIds = {8, 4, 5};

	request.append<ParameterReportStructureId>(idToCreate);
	request.append<CollectionInterval>(interval);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id: simplyCommutatedIds) {
		request.append<ParameterId>(id);
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
	ParameterReportStructureId ids[3] = {0, 4, 6};
	CollectionInterval interval = 7;
	etl::vector<ParameterId, 3> simplyCommutatedIds = {8, 4, 5};

	HousekeepingStructure structures[3];
	int i = 0;
	for (auto& newStructure: structures) {
		newStructure.structureId = ids[i];
		newStructure.collectionInterval = interval;
		newStructure.periodicGenerationActionStatus = false;
		for (ParameterId parameterId: simplyCommutatedIds) {
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
void storeSamplesToParameters(ParameterId id1, ParameterId id2, ParameterId id3) {
	static_cast<Parameter<uint16_t>&>(Services.parameterManagement.getParameter(id1)->get()).setValue(33);
	static_cast<Parameter<uint8_t>&>(Services.parameterManagement.getParameter(id2)->get()).setValue(77);
	static_cast<Parameter<uint32_t>&>(Services.parameterManagement.getParameter(id3)->get()).setValue(99);
}

/**
 * Helper function that forms the request to append new parameters to a housekeeping structure
 */
void appendNewParameters(Message& request, ParameterReportStructureId idToAppend) {
	uint16_t numOfSimplyCommutatedParams = 7;
	etl::vector<ParameterId, 7> simplyCommutatedIds = {8, 4, 5, 9, 11, 10, 220};

	request.append<ParameterReportStructureId>(idToAppend);
	request.appendUint16(numOfSimplyCommutatedParams);
	for (auto& id: simplyCommutatedIds) {
		request.append<ParameterId>(id);
	}
}

TEST_CASE("Create housekeeping structure") {
	SECTION("Valid structure creation request") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		ParameterReportStructureId idToCreate = 2;
		CollectionInterval interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::array<ParameterId, 3> simplyCommutatedIds = {4, 5, 8};

		request.append<ParameterReportStructureId>(idToCreate);
		request.append<CollectionInterval>(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);
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
		ParameterReportStructureId structureId = 9;
		HousekeepingStructure structure1;
		structure1.structureId = structureId;
		housekeepingService.housekeepingStructures.insert({structureId, structure1});

		ParameterReportStructureId idToCreate = 9;
		request.append<ParameterReportStructureId>(idToCreate);

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

		ParameterReportStructureId idsToCreate[16] = {1, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<ParameterId, 3> simplyCommutatedIds = {8, 4, 5};
		CollectionInterval interval = 12;
		REQUIRE(housekeepingService.housekeepingStructures.size() == 0);

		for (auto& structId: idsToCreate) {
			request.append<ParameterReportStructureId>(structId);
			request.append<CollectionInterval>(interval);
			request.appendUint16(numOfSimplyCommutatedParams);
			for (auto& parameterId: simplyCommutatedIds) {
				request.append<ParameterId>(parameterId);
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
		ParameterReportStructureId idToCreate = 2;
		CollectionInterval interval = 7;
		uint16_t numOfSimplyCommutatedParams = 9;
		etl::vector<ParameterId, 9> simplyCommutatedIds = {8, 4, 5, 4, 8, 8, 5, 4, 11};

		request.append<ParameterReportStructureId>(idToCreate);
		request.append<CollectionInterval>(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);
		}

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 5);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		REQUIRE(newStruct.simplyCommutatedParameterIds.size() == 4);
		ParameterId existingParameterIds[4] = {8, 4, 5, 11};
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

		ParameterReportStructureId numOfStructs = 5;
		ParameterReportStructureId ids[5] = {2, 3, 4, 7, 8};
		request.appendUint8(numOfStructs);
		for (auto& id: ids) {
			request.append<ParameterReportStructureId>(id);
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
		ParameterReportStructureId structureId = 4;
		request.appendUint8(numOfStructs);
		request.append<ParameterReportStructureId>(structureId);

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
		Message request2(HousekeepingService::ServiceType, HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		uint8_t numOfStructs = 5;
		ParameterReportStructureId idsToEnable[5] = {1, 3, 4, 6, 7};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToEnable) {
			request2.append<ParameterReportStructureId>(id);
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
		ParameterReportStructureId idsToDisable[4] = {0, 1, 4, 6};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToDisable) {
			request2.append<ParameterReportStructureId>(id);
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
		ParameterReportStructureId idsToReport[3] = {9, 4, 2};
		request2.appendUint8(numOfStructs);
		for (auto& id: idsToReport) {
			request2.append<ParameterReportStructureId>(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      2);

		Message report = ServiceTests::get(1); // Both 0 and 2 are the error messages because only id=4 was valid.

		ParameterReportStructureId validId = 4;
		CHECK(report.read<ParameterReportStructureId>() == validId);
		CHECK(not report.readBoolean());               // periodic status
		CHECK(report.read<CollectionInterval>() == 7); // interval
		CHECK(report.readUint16() == 3);               // number of simply commutated ids
		CHECK(report.read<ParameterId>() == 8);
		CHECK(report.read<ParameterId>() == 4); // ids
		CHECK(report.read<ParameterId>() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping structures without a TC message as argument") {
	SECTION("Check the report message generated by the non-TC function") {
		initializeHousekeepingStructures();
		ParameterReportStructureId structureId = 4;

		housekeepingService.housekeepingStructureReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingStructuresReport);
		CHECK(report.read<ParameterReportStructureId>() == structureId);
		CHECK(not report.readBoolean());
		CHECK(report.read<CollectionInterval>() == 7);
		CHECK(report.readUint16() == 3);
		CHECK(report.read<ParameterId>() == 8);
		CHECK(report.read<ParameterId>() == 4);
		CHECK(report.read<ParameterId>() == 5);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Reporting of housekeeping parameters") {
	SECTION("Valid structure request") {
		storeSamplesToParameters(8, 4, 5);
		initializeHousekeepingStructures();
		ParameterReportStructureId structId = 6;

		housekeepingService.housekeepingParametersReport(structId);

		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);
		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.read<ParameterReportStructureId>() == structId);
		CHECK(report.readUint16() == 33);
		CHECK(report.readUint8() == 77);
		CHECK(report.readUint32() == 99);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure request") {
		ParameterReportStructureId structId = 8;
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
		ParameterReportStructureId structureId = 6;

		housekeepingService.housekeepingParametersReport(structureId);
		CHECK(ServiceTests::count() == 1);
		Message report = ServiceTests::get(0);

		REQUIRE(report.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report.read<ParameterReportStructureId>() == structureId);
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
		ParameterReportStructureId structIds[5] = {0, 4, 7, 8, 11};
		request2.appendUint8(numOfStructs);
		for (auto& id: structIds) {
			request2.append<ParameterReportStructureId>(id);
		}
		MessageParser::execute(request2);

		CHECK(ServiceTests::count() == 5);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);
		Message report1 = ServiceTests::get(0);
		Message report2 = ServiceTests::get(1);

		REQUIRE(report1.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);
		REQUIRE(report2.messageType == HousekeepingService::MessageType::HousekeepingParametersReport);

		REQUIRE(report1.read<ParameterReportStructureId>() == 0);
		CHECK(report1.readUint16() == 33);
		CHECK(report1.readUint8() == 77);
		CHECK(report1.readUint32() == 99);

		REQUIRE(report2.read<ParameterReportStructureId>() == 4);
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
		ParameterReportStructureId structId = 2;
		request1.append<ParameterReportStructureId>(structId);
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
		request.append<ParameterReportStructureId>(0);
		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType,
		                 HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		ParameterReportStructureId structId = 0;
		request2.append<ParameterReportStructureId>(structId);
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
		ParameterReportStructureId structId = 6;
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
		ParameterReportStructureId structId = 6;
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);

		uint16_t numOfSimplyCommutatedParams = 34;

		etl::vector<ParameterId, 34> simplyCommutatedIds;
		for (uint16_t i = 0; i < 34; i++) {
			simplyCommutatedIds.push_back(i);
		}

		request.append<ParameterReportStructureId>(structId);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);
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
		ParameterReportStructureId numOfStructs = 4;
		ParameterReportStructureId structIds[4] = {0, 4, 9, 10};
		CollectionInterval intervals[4] = {12, 21, 32, 17};
		request.appendUint8(numOfStructs);
		int i = 0;
		for (auto& id: structIds) {
			request.append<ParameterReportStructureId>(id);
			request.append<CollectionInterval>(intervals[i++]);
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
		ParameterReportStructureId structIds[6] = {0, 4, 1, 6, 9, 10};
		request.appendUint8(numOfStructs);
		for (auto& id: structIds) {
			request.append<ParameterReportStructureId>(id);
		}
		housekeepingService.housekeepingStructures[0].periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures[4].collectionInterval = 24;
		housekeepingService.housekeepingStructures[6].collectionInterval = 13;

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 4);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) ==
		      3);

		Message report = ServiceTests::get(3);
		CHECK(report.readUint8() == 3);                        // Number of valid ids
		CHECK(report.read<ParameterReportStructureId>() == 0); // Id
		CHECK(report.readBoolean() == true);                   // Periodic status
		CHECK(report.read<CollectionInterval>() == 7);         // Interval
		CHECK(report.read<ParameterReportStructureId>() == 4); // Id
		CHECK(report.readBoolean() == false);                  // Periodic status
		CHECK(report.read<CollectionInterval>() == 24);        // Interval
		CHECK(report.read<ParameterReportStructureId>() == 6); // Id
		CHECK(report.readBoolean() == false);                  // Periodic status
		CHECK(report.read<CollectionInterval>() == 13);        // Interval

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Periodically reporting Housekeeping Structures") {
	Time::DefaultCUC nextCollection(0);
	Time::DefaultCUC currentTime(0);
	Time::DefaultCUC previousTime(0);
	SECTION("Non existent structures") {
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(ServiceTests::count() == 0);
		CHECK(nextCollection == Time::DefaultCUC((std::numeric_limits<uint32_t>::max()) * Time::DefaultCUC::Ratio::num / Time::DefaultCUC::Ratio ::den)); // NOLINT(misc-const-correctness)
	}
	SECTION("Collection Intervals set to max") {
		initializeHousekeepingStructures();
		for (auto& housekeepingStructure: housekeepingService.housekeepingStructures) {
			housekeepingStructure.second.collectionInterval = std::numeric_limits<CollectionInterval>::max();
		}
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(ServiceTests::count() == 0);
		CHECK(nextCollection == Time::DefaultCUC((std::numeric_limits<uint32_t>::max()) * Time::DefaultCUC::Ratio::num / Time::DefaultCUC::Ratio ::den)); // NOLINT(misc-const-correctness)
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
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(currentTime.asTAIseconds() == 900);
		CHECK(ServiceTests::count() == 0);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(currentTime.asTAIseconds() == 1000);
		CHECK(ServiceTests::count() == 1);
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + 6);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(currentTime.asTAIseconds() == 1800);
		CHECK(ServiceTests::count() == 2);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(ServiceTests::count() == 3);
		CHECK(currentTime.asTAIseconds() == 2000);
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + 15);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(ServiceTests::count() == 4);
		CHECK(currentTime.asTAIseconds() == 2700);
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		previousTime = currentTime;
		currentTime = Time::DefaultCUC(currentTime.asTAIseconds() + nextCollection.asTAIseconds());
		CHECK(ServiceTests::count() == 6);
		CHECK(currentTime.asTAIseconds() == 3000);
	}
	SECTION("Collection Intervals set to 0") {
		for (auto& housekeepingStructure: housekeepingService.housekeepingStructures) {
			housekeepingStructure.second.periodicGenerationActionStatus = true;
			housekeepingStructure.second.collectionInterval = 0;
		}
		nextCollection = housekeepingService.reportPendingStructures(currentTime, previousTime, nextCollection);
		CHECK(nextCollection == Time::DefaultCUC(0));
	}
}

TEST_CASE("Check getPeriodicGenerationActionStatus function") {
	SECTION("Returns periodic generation status") {
		initializeHousekeepingStructures();
		housekeepingService.housekeepingStructures.at(4).periodicGenerationActionStatus = true;

		CHECK(housekeepingService.getPeriodicGenerationActionStatus(0) == false);
		CHECK(housekeepingService.getPeriodicGenerationActionStatus(4) == true);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure ID in request") {
		initializeHousekeepingStructures();

		housekeepingService.getPeriodicGenerationActionStatus(1);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check getStruct function") {
	SECTION("Returns periodic generation status") {
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		ParameterReportStructureId idToCreate = 2;
		CollectionInterval interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::array<ParameterId, 3> simplyCommutatedIds = {4, 5, 8};

		request.append<ParameterReportStructureId>(idToCreate);
		request.append<CollectionInterval>(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		CHECK(std::addressof(housekeepingService.housekeepingStructures.at(2)) == std::addressof(housekeepingService.getStruct(2)->get()));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure ID in request") {
		initializeHousekeepingStructures();

		housekeepingService.getStruct(1);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check getCollectionInterval function") {
	SECTION("Returns Collection Interval") {
		initializeHousekeepingStructures();

		CHECK(housekeepingService.getCollectionInterval(0) == 7);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure ID in request") {
		initializeHousekeepingStructures();

		housekeepingService.getCollectionInterval(1);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check setPeriodicGenerationActionStatus function") {
	SECTION("Set Periodic Generation Action Status") {
		initializeHousekeepingStructures();

		housekeepingService.setPeriodicGenerationActionStatus(0, true);

		CHECK(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus == true);
		CHECK(housekeepingService.housekeepingStructures.at(4).periodicGenerationActionStatus == false);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure ID in request") {
		initializeHousekeepingStructures();

		housekeepingService.setPeriodicGenerationActionStatus(1, true);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check setCollectionInterval function") {
	SECTION("Sets Collection Interval") {
		initializeHousekeepingStructures();

		housekeepingService.setCollectionInterval(0, 8);

		CHECK(housekeepingService.housekeepingStructures.at(0).collectionInterval == 8);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid structure ID in request") {
		initializeHousekeepingStructures();

		housekeepingService.setCollectionInterval(1, 8);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check structExists function") {
	SECTION("Check if it returns correct boolean") {
		initializeHousekeepingStructures();

		CHECK(housekeepingService.structExists(0) == true);
		CHECK(housekeepingService.structExists(1) == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasNonExistingStructExecutionError function") {
	SECTION("Check if it returns correct boolean") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		initializeHousekeepingStructures();

		CHECK(housekeepingService.hasNonExistingStructExecutionError(0, request) == false);
		CHECK(housekeepingService.hasNonExistingStructExecutionError(1, request) == true);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedNonExistingStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasNonExistingStructError function") {
	SECTION("Check if it returns correct boolean") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		initializeHousekeepingStructures();

		CHECK(housekeepingService.hasNonExistingStructError(0, request) == false);
		CHECK(housekeepingService.hasNonExistingStructError(1, request) == true);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::RequestedNonExistingStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasNonExistingStructInternalError function") {
	SECTION("Check if it returns correct boolean") {
		initializeHousekeepingStructures();

		CHECK(housekeepingService.hasNonExistingStructInternalError(0) == false);
		CHECK(housekeepingService.hasNonExistingStructInternalError(1) == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InternalErrorType::NonExistentHousekeeping) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasAlreadyExistingParameterError function") {
	SECTION("Check if it returns correct boolean") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		ParameterReportStructureId idToCreate = 2;
		CollectionInterval interval = 7;
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::array<ParameterId, 3> simplyCommutatedIds = {4, 5, 8};

		request.append<ParameterReportStructureId>(idToCreate);
		request.append<CollectionInterval>(interval);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);
		}

		MessageParser::execute(request);
		HousekeepingStructure newStruct = housekeepingService.housekeepingStructures[idToCreate];

		auto& housekeepingStructure = housekeepingService.getStruct(idToCreate)->get();

		CHECK(housekeepingService.hasAlreadyExistingParameterError(housekeepingStructure, 6, request) == false);
		CHECK(housekeepingService.hasAlreadyExistingParameterError(housekeepingStructure, 5, request) == true);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::AlreadyExistingParameter) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasAlreadyExistingStructError function") {
	SECTION("Check if it returns correct boolean") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		initializeHousekeepingStructures();

		CHECK(housekeepingService.hasAlreadyExistingStructError(1, request) == false);
		CHECK(housekeepingService.hasAlreadyExistingStructError(0, request) == true);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedAlreadyExistingStructure) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasExceededMaxNumOfHousekeepingStructsError function") {
	SECTION("Check if it returns correct boolean") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::CreateHousekeepingReportStructure, Message::TC, 1);
		ParameterReportStructureId idsToCreate[11] = {1, 3, 5, 7, 8, 9, 10, 11, 12, 13, 14};
		uint16_t numOfSimplyCommutatedParams = 3;
		etl::vector<ParameterId, 3> simplyCommutatedIds = {8, 4, 5};
		CollectionInterval interval = 12;

		REQUIRE(housekeepingService.housekeepingStructures.size() == 0);

		for (auto& structId: idsToCreate) {
			request.append<ParameterReportStructureId>(structId);
			request.append<CollectionInterval>(interval);
			request.appendUint16(numOfSimplyCommutatedParams);
			for (auto& parameterId: simplyCommutatedIds) {
				request.append<ParameterId>(parameterId);
			}
			MessageParser::execute(request);

			if (housekeepingService.housekeepingStructures.size() < 9) {
				CHECK(housekeepingService.hasExceededMaxNumOfHousekeepingStructsError(request) == false);
			}
		}

		REQUIRE(housekeepingService.housekeepingStructures.size() == 10);

		CHECK(housekeepingService.hasExceededMaxNumOfHousekeepingStructsError(request) == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfHousekeepingStructures) == 2);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasRequestedAppendToEnabledHousekeepingError function") {
	SECTION("Error exists") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		// Enable 1 periodic struct with id=0
		HousekeepingStructure newStruct;
		newStruct.structureId = 0;
		newStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({0, newStruct});

		request.appendUint8(1);
		request.append<ParameterReportStructureId>(0);
		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType, HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		ParameterReportStructureId structId = 0;
		request2.append<ParameterReportStructureId>(structId);
		MessageParser::execute(request2);

		CHECK(housekeepingService.hasRequestedAppendToEnabledHousekeepingError(newStruct, request) == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedAppendToEnabledHousekeeping) == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Error doesn't exist") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport, Message::TC, 1);
		// Enable 1 periodic struct with id=0
		HousekeepingStructure newStruct;
		newStruct.structureId = 0;
		newStruct.periodicGenerationActionStatus = false;
		housekeepingService.housekeepingStructures.insert({0, newStruct});

		request.appendUint8(1);
		request.append<ParameterReportStructureId>(0);
		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures.at(0).periodicGenerationActionStatus);
		Message request2(HousekeepingService::ServiceType, HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);
		ParameterReportStructureId structId = 0;
		request2.append<ParameterReportStructureId>(structId);
		MessageParser::execute(request2);


		CHECK(housekeepingService.hasRequestedAppendToEnabledHousekeepingError(newStruct, request) == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasRequestedDeletionOfEnabledHousekeepingError function") {
	SECTION("Error exists") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = true;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		uint8_t numOfStructs = 1;
		ParameterReportStructureId structureId = 4;
		request.appendUint8(numOfStructs);
		request.append<ParameterReportStructureId>(structureId);

		MessageParser::execute(request);


		CHECK(housekeepingService.hasRequestedDeletionOfEnabledHousekeepingError(4, request) == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::RequestedDeletionOfEnabledHousekeeping) == 2);

		ServiceTests::reset();
		Services.reset();
	}
	SECTION("Error doesn't exists") {
		Message request(HousekeepingService::ServiceType, HousekeepingService::MessageType::DeleteHousekeepingReportStructure, Message::TC, 1);
		HousekeepingStructure periodicStruct;
		periodicStruct.structureId = 4;
		periodicStruct.periodicGenerationActionStatus = false;
		housekeepingService.housekeepingStructures.insert({4, periodicStruct});

		uint8_t numOfStructs = 1;
		ParameterReportStructureId structureId = 4;
		request.appendUint8(numOfStructs);
		request.append<ParameterReportStructureId>(structureId);

		MessageParser::execute(request);


		CHECK(housekeepingService.hasRequestedDeletionOfEnabledHousekeepingError(4, request) == false);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check hasExceededMaxNumOfSimplyCommutatedParamsError function") {
	SECTION("Check if it returns correct boolean") {
		initializeHousekeepingStructures();
		ParameterReportStructureId structId = 6;
		Message request(HousekeepingService::ServiceType,
		                HousekeepingService::MessageType::AppendParametersToHousekeepingStructure, Message::TC, 1);

		uint16_t numOfSimplyCommutatedParams = 34;

		etl::vector<ParameterId, 34> simplyCommutatedIds;
		for (uint16_t i = 0; i < 34; i++) {
			simplyCommutatedIds.push_back(i);
		}

		request.append<ParameterReportStructureId>(structId);
		request.appendUint16(numOfSimplyCommutatedParams);
		for (auto& id: simplyCommutatedIds) {
			request.append<ParameterId>(id);

			if (housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() < 30) {
				CHECK(housekeepingService.hasExceededMaxNumOfSimplyCommutatedParamsError(housekeepingService.housekeepingStructures[structId], request) == false);
			}
		}
		REQUIRE(housekeepingService.housekeepingStructures.find(structId) != housekeepingService.housekeepingStructures.end());
		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 3);

		MessageParser::execute(request);

		REQUIRE(housekeepingService.housekeepingStructures[structId].simplyCommutatedParameterIds.size() == 30);

		CHECK(housekeepingService.hasExceededMaxNumOfSimplyCommutatedParamsError(housekeepingService.housekeepingStructures[structId], request) == true);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::ExceededMaxNumberOfSimplyCommutatedParameters) == 2);

		ServiceTests::reset();
		Services.reset();
	}
}
