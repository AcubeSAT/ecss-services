#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <catch2/catch.hpp>
#include <etl/array.h>
#include "ServiceTests.hpp"

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

TEST_CASE("Add Parameter Monitoring Definitions") {
	SECTION("Valid Request to add Parameter Monitoring Definitions") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		etl::array<uint16_t, 3> monitoredParameterIds = {0, 1, 2};
		etl::array<uint16_t, 3> repetitionNumbers = {5, 3, 8};
		uint8_t expetedValueCheckMask = 2;
		uint16_t expectedValue = 10;
		uint16_t unexpectedValueEvent = 5;
		uint16_t lowLimit = 3;
		uint16_t belowLowLimitEvent = 8;
		uint16_t highLimit = 8;
		uint16_t aboveHighLimitEvent = 10;
		uint16_t lowDeltaThreshold = 4;
		uint16_t belowLowThresholdEvent = 2;
		uint16_t highDeltaThreshold = 10;
		uint16_t aboveHighThresholdEvent = 1;
		uint16_t numberOfConsecutiveDeltaChecks = 5;

		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(monitoredParameterIds[0]);
		request.appendUint16(repetitionNumbers[0]);
		request.appendEnum8(PMONBase::ExpectedValueCheck);
		request.appendUint64(expetedValueCheckMask);
		// TODO: Find out how to append deduced values.
		request.appendDouble(expectedValue);
		request.appendEnum16(unexpectedValueEvent);

		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(monitoredParameterIds[1]);
		request.appendUint16(repetitionNumbers[1]);
		request.appendEnum8(PMONBase::LimitCheck);
		request.appendDouble(lowLimit);
		request.appendEnum16(belowLowLimitEvent);
		request.appendDouble(highLimit);
		request.appendEnum16(aboveHighLimitEvent);

		request.appendEnum16(PMONIds[2]);
		request.appendEnum16(monitoredParameterIds[2]);
		request.appendUint16(repetitionNumbers[2]);
		request.appendEnum8(PMONBase::DeltaCheck);
		request.appendDouble(lowDeltaThreshold);
		request.appendEnum16(belowLowThresholdEvent);
		request.appendDouble(highDeltaThreshold);
		request.appendEnum16(aboveHighThresholdEvent);
		request.appendUint16(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.getCount(PMONIds[0]) == 1);
		CHECK(onBoardMonitoringService.getCount(PMONIds[1]) == 1);
		CHECK(onBoardMonitoringService.getCount(PMONIds[2]) == 1);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getRepetitionCounter() == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getRepetitionCounter() == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getRepetitionCounter() == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getRepetitionNumber() == repetitionNumbers[0]);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getRepetitionNumber() == repetitionNumbers[1]);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getRepetitionNumber() == repetitionNumbers[2]);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().isMonitoringEnabled() == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().isMonitoringEnabled() == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().isMonitoringEnabled() == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getCheckType() == PMONBase::ExpectedValueCheck);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getCheckType() == PMONBase::LimitCheck);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getCheckType() == PMONBase::DeltaCheck);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getCheckingStatus() == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getCheckingStatus() == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getCheckingStatus() == PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getExpectedValue() == expectedValue);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getMask() == expetedValueCheckMask);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getUnexpectedValueEvent() == unexpectedValueEvent);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getLowLimit() == lowLimit);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getBelowLowLimitEvent() == belowLowLimitEvent);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getHighLimit() == highLimit);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getAboveHighLimitEvent() == aboveHighLimitEvent);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getLowDeltaThreshold() == lowDeltaThreshold);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getBelowLowThresholdEvent() == belowLowThresholdEvent);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getHighDeltaThreshold() == highDeltaThreshold);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getAboveHighThresholdEvent() == aboveHighThresholdEvent);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getNumberOfConsecutiveDeltaChecks() == numberOfConsecutiveDeltaChecks);
		ServiceTests::reset();
		Services.reset();
	}

	//	SECTION("Parameter Monitoring List is full") {
	//		initialiseParameterMonitoringDefinitions();
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 5;
	//		uint16_t monitoredParameterId = 5;
	//		uint16_t repetitionNumber = 5;
	//
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		request.appendEnum16(monitoredParameterId);
	//		request.appendUint16(repetitionNumber);
	//		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
	//
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ParameterMonitoringListIsFull) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//
	//	SECTION("Parameter Monitoring Definition already exists") {
	//		initialiseParameterMonitoringDefinitions();
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 0;
	//		uint16_t monitoredParameterId = 0;
	//		uint16_t repetitionNumber = 5;
	//
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		request.appendEnum16(monitoredParameterId);
	//		request.appendUint16(repetitionNumber);
	//		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
	//
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//
	//	SECTION("Add Parameter Monitoring Definition with a non-existing parameter") {
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 4;
	//		uint16_t monitoredParameterId = 100;
	//		uint16_t repetitionNumber = 5;
	//
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		request.appendEnum16(monitoredParameterId);
	//		request.appendUint16(repetitionNumber);
	//		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
	//
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//
	//	SECTION("High limit is lower than low limit") {
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 0;
	//		uint16_t monitoredParameterId = 0;
	//		uint16_t repetitionNumber = 5;
	//		uint16_t lowLimit = 8;
	//		uint16_t highLimit = 2;
	//
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		request.appendUint16(monitoredParameterId);
	//		request.appendUint16(repetitionNumber);
	//		request.appendEnum8(onBoardMonitoringService.LimitCheck);
	//		request.appendUint16(lowLimit);
	//		request.appendEnum8(onBoardMonitoringService.BelowLowLimitEvent);
	//		request.appendUint16(highLimit);
	//		request.appendEnum8(onBoardMonitoringService.AboveHighLimitEvent);
	//
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//
	//	SECTION("High threshold is lower than low threshold") {
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 0;
	//		uint16_t monitoredParameterId = 0;
	//		uint16_t repetitionNumber = 5;
	//		uint16_t lowDeltaThreshold = 8;
	//		uint16_t highDeltaThreshold = 2;
	//		uint16_t numberOfConsecutiveDeltaChecks = 5;
	//
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		request.appendUint16(monitoredParameterId);
	//		request.appendUint16(repetitionNumber);
	//		request.appendEnum8(onBoardMonitoringService.DeltaCheck);
	//		request.appendUint16(lowDeltaThreshold);
	//		request.appendEnum8(onBoardMonitoringService.BelowLowThresholdEvent);
	//		request.appendUint16(highDeltaThreshold);
	//		request.appendEnum8(onBoardMonitoringService.AboveHighThresholdEvent);
	//		request.appendUint16(numberOfConsecutiveDeltaChecks);
	//
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
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
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport);
		CHECK(report.readUint16() == onBoardMonitoringService.maximumTransitionReportingDelay);
		CHECK(report.readUint16() == numberOfIds);

		CHECK(report.readEnum16() == PMONIds[0]);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().repetitionNumber);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().checkType);
		CHECK(report.readUint64() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getMask());
		CHECK(report.readDouble() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getExpectedValue());
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().getUnexpectedValueEvent());

		CHECK(report.readEnum16() == PMONIds[1]);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().repetitionNumber);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().checkType);
		CHECK(report.readDouble() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getLowLimit());
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getBelowLowLimitEvent());
		CHECK(report.readDouble() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getHighLimit());
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().getAboveHighLimitEvent());

		CHECK(report.readEnum16() == PMONIds[2]);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().repetitionNumber);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().checkType);
		CHECK(report.readDouble() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getLowDeltaThreshold());
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getBelowLowThresholdEvent());
		CHECK(report.readDouble() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getHighDeltaThreshold());
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getAboveHighThresholdEvent());
		CHECK(report.readUint16() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().getNumberOfConsecutiveDeltaChecks());

		CHECK(report.readEnum16() == PMONIds[3]);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled);
		CHECK(report.readEnum16() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().repetitionNumber);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().checkType);
		CHECK(report.readUint16() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().getLowDeltaThreshold());
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().getBelowLowThresholdEvent());
		CHECK(report.readUint16() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().getHighDeltaThreshold());
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().getAboveHighThresholdEvent());
		CHECK(report.readUint16() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().getNumberOfConsecutiveDeltaChecks());

		ServiceTests::reset();
		Services.reset();
	}
}
	//
	//	SECTION("Invalid request to report Parameter Monitoring Definitions") {
	//		initialiseParameterMonitoringDefinitions();
	//		uint16_t numberOfIds = 1;
	//		uint16_t PMONId = 5;
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		request.appendUint16(PMONId);
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 1);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//
	//	SECTION("One invalid and one valid request to report Parameter Monitoring Definitions") {
	//		initialiseParameterMonitoringDefinitions();
	//		uint16_t numberOfIds = 2;
	//		Message request =
	//		    Message(OnBoardMonitoringService::ServiceType,
	//		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
	//		request.appendUint16(numberOfIds);
	//		etl::array<uint16_t, 3> PMONIds = {0, 5};
	//		request.appendUint16(PMONIds[0]);
	//		request.appendUint16(PMONIds[1]);
	//		MessageParser::execute(request);
	//		CHECK(ServiceTests::count() == 2);
	//		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);
	//
	//		Message report = ServiceTests::get(0);
	//		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
	//		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions);
	//		CHECK(report.readEnum16() == PMONIds[0]);
	//		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds[0]));
	//		CHECK(report.readEnum8() == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds[0]));
	//		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds[0]));
	//		CHECK(report.readUint8() == onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds[0]).mask);
	//		CHECK(report.readUint16() ==
	//		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds[0]).expectedValue);
	//		CHECK(report.readEnum8() ==
	//		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds[0]).notExpectedValueEvent);
	//		clearAllMaps();
	//		ServiceTests::reset();
	//		Services.reset();
	//	}
	//}