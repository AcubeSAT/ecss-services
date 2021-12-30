#include <catch2/catch.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/array.h>
#include "Services/ParameterService.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <ServicePool.hpp>

OnBoardMonitoringService& onBoardMonitoringService = Services.onBoardMonitoringService;

ParameterService parameterService = ParameterService();

void initialiseParameterMonitoringDefinitions() {
	Parameter<uint8_t> parameter1 = Parameter<uint8_t>(3);
	Parameter<uint16_t> parameter2 = Parameter<uint16_t>(7);
	Parameter<uint32_t> parameter3 = Parameter<uint32_t>(9);

	onBoardMonitoringService.parameterMonitoringFunctionStatus = true;
	onBoardMonitoringService.maximumTransitionReportingDelay = 0;

	onBoardMonitoringService.RepetitionNumber.insert({parameter1, 10});
	onBoardMonitoringService.RepetitionNumber.insert({parameter2, 2});
	onBoardMonitoringService.RepetitionNumber.insert({parameter3, 5});

	onBoardMonitoringService.ParameterMonitoringList.insert({0, parameter1});
	onBoardMonitoringService.ParameterMonitoringList.insert({1, parameter2});
	onBoardMonitoringService.ParameterMonitoringList.insert({2, parameter3});

	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter1, 0});
	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter2, 1});
	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter3, 2});

	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter1, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter2, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter3, false});
}

void clearAllMaps() {
	onBoardMonitoringService.ParameterMonitoringList.clear();
	onBoardMonitoringService.ParameterMonitoringIds.clear();
	onBoardMonitoringService.ParameterMonitoringCheckingStatus.clear();
	onBoardMonitoringService.RepetitionCounter.clear();
	onBoardMonitoringService.RepetitionNumber.clear();
	onBoardMonitoringService.ParameterMonitoringStatus.clear();
	onBoardMonitoringService.CheckTransitionList.clear();
	onBoardMonitoringService.ParameterMonitoringCheckTypes.clear();
	onBoardMonitoringService.LimitCheckParameters.clear();
	onBoardMonitoringService.ExpectedValueCheckParameters.clear();
	onBoardMonitoringService.DeltaCheckParameters.clear();
}

TEST_CASE("Enable Parameter Monitoring Definitions") {
	SECTION("Valid request to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint8(3);
		request.appendEnum16(0);
		request.appendEnum16(1);
		request.appendEnum16(2);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions);
		CHECK(onBoardMonitoringService.parameterMonitoringFunctionStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == 0);
		clearAllMaps();
	}
	SECTION("Request to enable Parameter MonitoringDefinitions with the last ID of the request exceeding the size of "
	        "the Parameter Monitoring List") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint8(4);
		request.appendEnum16(0);
		request.appendEnum16(1);
		request.appendEnum16(2);
		request.appendEnum16(3);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.parameterMonitoringFunctionStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == true);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == 0);
		clearAllMaps();
	}
}

TEST_CASE("Disable Parameter Monitoring Definitions") {
	SECTION("Valid request to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint8(3);
		request.appendEnum16(0);
		request.appendEnum16(1);
		request.appendEnum16(2);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		clearAllMaps();
	}
	SECTION("Request to disable Parameter MonitoringDefinitions with the last ID of the request exceeding the size of "
	        "the Parameter Monitoring List") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint8(4);
		request.appendEnum16(0);
		request.appendEnum16(1);
		request.appendEnum16(2);
		request.appendEnum16(3);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(0)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(1)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus
		          .find(onBoardMonitoringService.ParameterMonitoringList.find(2)->second)
		          ->second == OnBoardMonitoringService::Unchecked);
		clearAllMaps();
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
	CHECK(ServiceTests::count() == 1);

	Message report = ServiceTests::get(0);
	CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
	CHECK(report.messageType == OnBoardMonitoringService::MessageType::ChangeMaximumTransitionReportingDelay);
	CHECK(onBoardMonitoringService.maximumTransitionReportingDelay == newMaximumTransitionReportingDelay);
}

TEST_CASE("Delete all Parameter Monitoring Definitions") {
	SECTION("Valid request to delete all Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		onBoardMonitoringService.parameterMonitoringFunctionStatus = false;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions, Message::TC, 0);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.empty());
		CHECK(onBoardMonitoringService.CheckTransitionList.empty());
	}
	SECTION("Invalid request to delete all Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions, Message::TC, 0);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::InvalidRequestToDeleteAllParameterMonitoringDefinitionsError) == 1);
		CHECK(!onBoardMonitoringService.ParameterMonitoringList.empty());
		CHECK(!onBoardMonitoringService.CheckTransitionList.empty());
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
		uint16_t lowLimit = 3;
		uint16_t highLimit = 8;
		uint16_t lowDeltaThreshold = 4;
		uint16_t highDeltaThreshold = 10;
		uint16_t numberOfConsecutiveDeltaChecks = 5;

		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(monitoredParameterIds.at(0));
		request.appendUint16(repetitionNumbers.at(0));
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
		request.appendUint8(expetedValueCheckMask);
		// TODO: Find out how to append deduced values.
		request.appendEnum16(expectedValue);
		request.appendEnum8(onBoardMonitoringService.NotExpectedValueEvent);

		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(monitoredParameterIds.at(1));
		request.appendUint16(repetitionNumbers.at(1));
		request.appendEnum8(onBoardMonitoringService.LimitCheck);
		request.appendUint16(lowLimit);
		request.appendEnum8(onBoardMonitoringService.BelowLowLimitEvent);
		request.appendUint16(highLimit);
		request.appendEnum8(onBoardMonitoringService.AboveHighLimitEvent);

		request.appendEnum16(PMONIds.at(2));
		request.appendEnum16(monitoredParameterIds.at(2));
		request.appendUint16(repetitionNumbers.at(2));
		request.appendEnum8(onBoardMonitoringService.DeltaCheck);
		request.appendUint16(lowDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.BelowLowThresholdEvent);
		request.appendUint16(highDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.AboveHighThresholdEvent);
		request.appendUint16(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(2)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringIds.at(parameterService.getParameter(PMONIds.at(0))->get()) ==
		      0);
		CHECK(onBoardMonitoringService.ParameterMonitoringIds.at(parameterService.getParameter(PMONIds.at(1))->get()) ==
		      1);
		CHECK(onBoardMonitoringService.ParameterMonitoringIds.at(parameterService.getParameter(PMONIds.at(2))->get()) ==
		      2);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(parameterService.getParameter(PMONIds.at(0))->get()) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(parameterService.getParameter(PMONIds.at(1))->get()) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(parameterService.getParameter(PMONIds.at(2))->get()) == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber.at(parameterService.getParameter(PMONIds.at(0))->get()) ==
		      repetitionNumbers.at(0));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(parameterService.getParameter(PMONIds.at(1))->get()) ==
		      repetitionNumbers.at(1));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(parameterService.getParameter(PMONIds.at(2))->get()) ==
		      repetitionNumbers.at(2));
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(
		          parameterService.getParameter(PMONIds.at(0))->get()) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(
		          parameterService.getParameter(PMONIds.at(1))->get()) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(
		          parameterService.getParameter(PMONIds.at(2))->get()) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(
		          parameterService.getParameter(PMONIds.at(0))->get()) == onBoardMonitoringService.ExpectedValueCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(
		          parameterService.getParameter(PMONIds.at(1))->get()) == onBoardMonitoringService.LimitCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(
		          parameterService.getParameter(PMONIds.at(2))->get()) == onBoardMonitoringService.DeltaCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(
		          parameterService.getParameter(PMONIds.at(0))->get()) == onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(
		          parameterService.getParameter(PMONIds.at(1))->get()) == onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(
		          parameterService.getParameter(PMONIds.at(2))->get()) == onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters
		          .find(parameterService.getParameter(PMONIds.at(0))->get())
		          ->second.expectedValue == expectedValue);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters
		          .find(parameterService.getParameter(PMONIds.at(0))->get())
		          ->second.mask == expetedValueCheckMask);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters
		          .find(parameterService.getParameter(PMONIds.at(0))->get())
		          ->second.notExpectedValueEvent == onBoardMonitoringService.NotExpectedValueEvent);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(parameterService.getParameter(PMONIds.at(1))->get())
		          ->second.lowLimit == lowLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(parameterService.getParameter(PMONIds.at(1))->get())
		          ->second.highLimit == highLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(parameterService.getParameter(PMONIds.at(1))->get())
		          ->second.belowLowLimitEvent == onBoardMonitoringService.BelowLowLimitEvent);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(parameterService.getParameter(PMONIds.at(1))->get())
		          ->second.aboveHighLimitEvent == onBoardMonitoringService.AboveHighLimitEvent);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(parameterService.getParameter(PMONIds.at(2))->get())
		          ->second.lowDeltaThreshold == lowDeltaThreshold);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(parameterService.getParameter(PMONIds.at(2))->get())
		          ->second.highDeltaThreshold == highDeltaThreshold);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(parameterService.getParameter(PMONIds.at(2))->get())
		          ->second.belowLowThresholdEvent == onBoardMonitoringService.BelowLowThresholdEvent);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(parameterService.getParameter(PMONIds.at(2))->get())
		          ->second.aboveHighThresholdEvent == onBoardMonitoringService.AboveHighThresholdEvent);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(parameterService.getParameter(PMONIds.at(2))->get())
		          ->second.numberOfConsecutiveDeltaChecks == numberOfConsecutiveDeltaChecks);
	}
}