#include <catch2/catch.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/array.h>
#include <etl/String.hpp>
#include <ServicePool.hpp>

OnBoardMonitoringService& onBoardMonitoringService = Services.onBoardMonitoringService;

struct Fixtures {
	PMONExpectedValueCheck monitoringDefinition1 = PMONExpectedValueCheck(7, 5, 10, 8, 0);
	PMONLimitCheck monitoringDefinition2 = PMONLimitCheck(7, 5, 2, 1, 9, 2);
	PMONDeltaCheck monitoringDefinition3 = PMONDeltaCheck(7, 5, 5, 3, 3, 11, 4);
	PMONDeltaCheck monitoringDefinition4 = PMONDeltaCheck(7, 5, 5, 3, 3, 11, 4);

	/*
	 * Constructor to modify monitoring definitions if needed
	 */
	Fixtures() {
		monitoringDefinition1.monitoringEnabled = true;
	}
};

Fixtures fixtures;

void initialiseParameterMonitoringDefinitions() {
	// Reset fixtures to the defaults set up by the constructor
	new (&fixtures) Fixtures();

	onBoardMonitoringService.addPMONDefinition(0, fixtures.monitoringDefinition1);
	onBoardMonitoringService.addPMONDefinition(1, fixtures.monitoringDefinition2);
	onBoardMonitoringService.addPMONDefinition(2, fixtures.monitoringDefinition3);
	onBoardMonitoringService.addPMONDefinition(3, fixtures.monitoringDefinition4);
}

TEST_CASE("Enable Parameter Monitoring Definitions") {
	SECTION("3 valid requests to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK((onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == true));
		CHECK((onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled == true));
		CHECK((onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == true));
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().repetitionCounter == 0);

		ServiceTests::reset();
		Services.reset();
	}
	SECTION("2 valid requests to enable Parameter Monitoring Definitions and 1 invalid") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 4;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 10, 1};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK((onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == true));
		CHECK((onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == true));
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().repetitionCounter == 0);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disable Parameter Monitoring Definitions") {
	SECTION("3 valid requests to disable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);
		onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled = true;

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().checkingStatus == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().checkingStatus == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().checkingStatus == PMONBase::Unchecked);

		ServiceTests::reset();
		Services.reset();
	}
	SECTION("3 valid requests to disable Parameter Monitoring Definitions and 1 invalid") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 4;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 10, 1, 2};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);
		request.appendEnum16(PMONIds[3]);
		onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled = true;
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().checkingStatus == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().checkingStatus == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().checkingStatus == PMONBase::Unchecked);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Change Maximum Transition Reporting Delay") {
	initialiseParameterMonitoringDefinitions();
	Message request =
	    Message(OnBoardMonitoringService::ServiceType,
	            OnBoardMonitoringService::MessageType::ChangeMaximumTransitionReportingDelay, Message::TC, 0);
	uint16_t newMaximumTransitionReportingDelay = 10;
	request.appendUint16(newMaximumTransitionReportingDelay);
	MessageParser::execute(request);
	CHECK(ServiceTests::count() == 0);
	CHECK(onBoardMonitoringService.maximumTransitionReportingDelay == newMaximumTransitionReportingDelay);

	ServiceTests::reset();
	Services.reset();
}

TEST_CASE("Delete all Parameter Monitoring Definitions") {
	SECTION("Valid request to delete all Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		onBoardMonitoringService.parameterMonitoringFunctionStatus = false;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions, Message::TC, 0);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.isPMONListEmpty());

		ServiceTests::reset();
		Services.reset();
	}
	SECTION("Invalid request to delete all Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		onBoardMonitoringService.parameterMonitoringFunctionStatus = true;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions, Message::TC, 0);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidRequestToDeleteAllParameterMonitoringDefinitions) ==
		      1);

		CHECK(!onBoardMonitoringService.isPMONListEmpty());

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Report Parameter Monitoring Definitions") {
	SECTION("Valid request to report Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 3;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport);
		CHECK(report.readUint16() == onBoardMonitoringService.maximumTransitionReportingDelay);
		CHECK(report.readUint16() == numberOfIds);
		CHECK(report.readEnum16() == PMONIds.at(0));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(0)));
		CHECK(report.readEnum8() == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)));
		CHECK(report.readEnum8() == onBoardMonitoringService.ExpectedValueCheck);
		CHECK(report.readUint8() == onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).mask);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).expectedValue);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).notExpectedValueEvent);
		CHECK(report.readEnum16() == PMONIds.at(1));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(1)));
		CHECK(report.readEnum8() == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(1)));
		CHECK(report.readEnum8() == onBoardMonitoringService.LimitCheck);
		CHECK(report.readUint16() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).lowLimit);
		CHECK(report.readEnum8() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).belowLowLimitEvent);
		CHECK(report.readUint16() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).highLimit);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).aboveHighLimitEvent);
		CHECK(report.readEnum16() == PMONIds.at(2));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(2)));
		CHECK(report.readEnum8() == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(2)));
		CHECK(report.readEnum8() == onBoardMonitoringService.DeltaCheck);
		CHECK(report.readUint16() == onBoardMonitoringService.DeltaCheckParameters.at(PMONIds.at(2)).lowDeltaThreshold);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.DeltaCheckParameters.at(PMONIds.at(2)).belowLowThresholdEvent);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.DeltaCheckParameters.at(PMONIds.at(2)).highDeltaThreshold);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.DeltaCheckParameters.at(PMONIds.at(2)).aboveHighThresholdEvent);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.DeltaCheckParameters.at(PMONIds.at(2)).numberOfConsecutiveDeltaChecks);
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid request to report Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 5;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("One invalid and one valid request to report Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 2;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 5};
		request.appendUint16(PMONIds.at(0));
		request.appendUint16(PMONIds.at(1));
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions);
		CHECK(report.readEnum16() == PMONIds.at(0));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(0)));
		CHECK(report.readEnum8() == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)));
		CHECK(report.readUint8() == onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).mask);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).expectedValue);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).notExpectedValueEvent);
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}
}