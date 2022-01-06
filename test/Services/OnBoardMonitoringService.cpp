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
	Parameter<uint32_t> parameter4 = Parameter<uint32_t>(2);

	onBoardMonitoringService.parameterMonitoringFunctionStatus = true;
	onBoardMonitoringService.maximumTransitionReportingDelay = 0;

	onBoardMonitoringService.RepetitionNumber.insert({0, 10});
	onBoardMonitoringService.RepetitionNumber.insert({1, 2});
	onBoardMonitoringService.RepetitionNumber.insert({2, 5});
	onBoardMonitoringService.RepetitionNumber.insert({3, 6});

	onBoardMonitoringService.RepetitionCounter.insert({0, 0});
	onBoardMonitoringService.RepetitionCounter.insert({1, 0});
	onBoardMonitoringService.RepetitionCounter.insert({2, 0});
	onBoardMonitoringService.RepetitionCounter.insert({3, 0});

	onBoardMonitoringService.ParameterMonitoringList.insert({0, parameter1});
	onBoardMonitoringService.ParameterMonitoringList.insert({1, parameter2});
	onBoardMonitoringService.ParameterMonitoringList.insert({2, parameter3});
	onBoardMonitoringService.ParameterMonitoringList.insert({3, parameter3});

	onBoardMonitoringService.MonitoredParameterIds.insert({0, 0});
	onBoardMonitoringService.MonitoredParameterIds.insert({1, 1});
	onBoardMonitoringService.MonitoredParameterIds.insert({2, 2});
	onBoardMonitoringService.MonitoredParameterIds.insert({3, 3});

	onBoardMonitoringService.ParameterMonitoringStatus.insert({0, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({1, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({2, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({3, true});

	onBoardMonitoringService.ParameterMonitoringCheckingStatus.insert({0, OnBoardMonitoringService::Unchecked});
	onBoardMonitoringService.ParameterMonitoringCheckingStatus.insert({1, OnBoardMonitoringService::Unchecked});
	onBoardMonitoringService.ParameterMonitoringCheckingStatus.insert({2, OnBoardMonitoringService::Unchecked});
	onBoardMonitoringService.ParameterMonitoringCheckingStatus.insert({3, OnBoardMonitoringService::Unchecked});

	onBoardMonitoringService.ParameterMonitoringCheckTypes.insert({0, onBoardMonitoringService.ExpectedValueCheck});
	onBoardMonitoringService.ParameterMonitoringCheckTypes.insert({1, onBoardMonitoringService.LimitCheck});
	onBoardMonitoringService.ParameterMonitoringCheckTypes.insert({2, onBoardMonitoringService.DeltaCheck});

	struct OnBoardMonitoringService::ExpectedValueCheck expectedValueCheck = {};
	onBoardMonitoringService.ExpectedValueCheckParameters.insert({0, expectedValueCheck});
	struct OnBoardMonitoringService::LimitCheck limitCheck = {};
	onBoardMonitoringService.LimitCheckParameters.insert({1, limitCheck});
	struct OnBoardMonitoringService::DeltaCheck deltaCheck = {};
	onBoardMonitoringService.DeltaCheckParameters.insert({2, deltaCheck});
};

void clearAllMaps() {
	onBoardMonitoringService.ParameterMonitoringList.clear();
	onBoardMonitoringService.MonitoredParameterIds.clear();
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
	SECTION("3 valid requests to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.parameterMonitoringFunctionStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)) == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)) == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)) == true);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(0)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(1)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(2)) == 0);
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}
	SECTION("3 valid requests to enable Parameter Monitoring Definitions and 1 invalid") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 4;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 1, 2, 10};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));
		request.appendEnum16(PMONIds.at(3));

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.parameterMonitoringFunctionStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)) == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)) == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)) == true);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(0)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(1)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(2)) == 0);
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Disable Parameter Monitoring Definitions") {
	SECTION("3 valid requests to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(0)) ==
		      OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(1)) ==
		      OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(2)) ==
		      OnBoardMonitoringService::Unchecked);

		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}
	SECTION("3 valid requests to enable Parameter Monitoring Definitions and 1 invalid") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DisableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 4;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 1, 2, 10};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));
		request.appendEnum16(PMONIds.at(3));

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(0)) ==
		      OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(1)) ==
		      OnBoardMonitoringService::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(2)) ==
		      OnBoardMonitoringService::Unchecked);
		clearAllMaps();
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

		CHECK(onBoardMonitoringService.ParameterMonitoringList.empty());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.empty());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.empty());
		CHECK(onBoardMonitoringService.RepetitionCounter.empty());
		CHECK(onBoardMonitoringService.RepetitionNumber.empty());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.empty());
		CHECK(onBoardMonitoringService.CheckTransitionList.empty());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.empty());
		CHECK(onBoardMonitoringService.LimitCheckParameters.empty());
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.empty());
		CHECK(onBoardMonitoringService.DeltaCheckParameters.empty());
	}
	SECTION("Invalid request to delete all Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteAllParameterMonitoringDefinitions, Message::TC, 0);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::InvalidRequestToDeleteAllParameterMonitoringDefinitionsError) == 1);

		CHECK(!onBoardMonitoringService.ParameterMonitoringList.empty());
		CHECK(!onBoardMonitoringService.MonitoredParameterIds.empty());
		CHECK(!onBoardMonitoringService.ParameterMonitoringCheckingStatus.empty());
		CHECK(!onBoardMonitoringService.RepetitionCounter.empty());
		CHECK(!onBoardMonitoringService.RepetitionNumber.empty());
		CHECK(!onBoardMonitoringService.ParameterMonitoringStatus.empty());
		CHECK(!onBoardMonitoringService.CheckTransitionList.empty());
		CHECK(!onBoardMonitoringService.ParameterMonitoringCheckTypes.empty());
		CHECK(!onBoardMonitoringService.LimitCheckParameters.empty());
		CHECK(!onBoardMonitoringService.ExpectedValueCheckParameters.empty());
		CHECK(!onBoardMonitoringService.DeltaCheckParameters.empty());
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
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(2)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(0)) !=
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(1)) !=
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(2)) !=
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(0) == 0));
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(1) == 0));
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(2) == 0));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)) == repetitionNumbers.at(0));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(1)) == repetitionNumbers.at(1));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(2)) == repetitionNumbers.at(2));
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)) == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(0)) ==
		      onBoardMonitoringService.ExpectedValueCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(1)) ==
		      onBoardMonitoringService.LimitCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(2)) ==
		      onBoardMonitoringService.DeltaCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(0)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(1)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(2)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(0))->second.expectedValue ==
		      expectedValue);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(0))->second.mask ==
		      expetedValueCheckMask);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(0))->second.notExpectedValueEvent ==
		      onBoardMonitoringService.NotExpectedValueEvent);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(1))->second.lowLimit == lowLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(1))->second.highLimit == highLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(1))->second.belowLowLimitEvent ==
		      onBoardMonitoringService.BelowLowLimitEvent);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(1))->second.aboveHighLimitEvent ==
		      onBoardMonitoringService.AboveHighLimitEvent);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(PMONIds.at(2))->second.lowDeltaThreshold ==
		      lowDeltaThreshold);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(PMONIds.at(2))->second.highDeltaThreshold ==
		      highDeltaThreshold);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(PMONIds.at(2))->second.belowLowThresholdEvent ==
		      onBoardMonitoringService.BelowLowThresholdEvent);
		CHECK(onBoardMonitoringService.DeltaCheckParameters.find(PMONIds.at(2))->second.aboveHighThresholdEvent ==
		      onBoardMonitoringService.AboveHighThresholdEvent);
		CHECK(
		    onBoardMonitoringService.DeltaCheckParameters.find(PMONIds.at(2))->second.numberOfConsecutiveDeltaChecks ==
		    numberOfConsecutiveDeltaChecks);
	}

	SECTION("Parameter Monitoring List is full") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 4;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendEnum16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ParameterMonitoringListIsFull) == 1);
		clearAllMaps();
	}

	SECTION("Parameter Monitoring Definition already exists") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendEnum16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
		clearAllMaps();
	}

	SECTION("Add Parameter Monitoring Definition with a non-existing parameter") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 4;
		uint16_t monitoredParameterId = 10;
		uint16_t repetitionNumber = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendEnum16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
		clearAllMaps();
	}

	SECTION("High limit is lower than low limit") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;
		uint16_t lowLimit = 8;
		uint16_t highLimit = 2;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.LimitCheck);
		request.appendUint16(lowLimit);
		request.appendEnum8(onBoardMonitoringService.BelowLowLimitEvent);
		request.appendUint16(highLimit);
		request.appendEnum8(onBoardMonitoringService.AboveHighLimitEvent);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
	}

	SECTION("High threshold is lower than low threshold") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;
		uint16_t lowDeltaThreshold = 8;
		uint16_t highDeltaThreshold = 2;
		uint16_t numberOfConsecutiveDeltaChecks = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.DeltaCheck);
		request.appendUint16(lowDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.BelowLowThresholdEvent);
		request.appendUint16(highDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.AboveHighThresholdEvent);
		request.appendUint16(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);
	}
}

TEST_CASE("Delete Parameter Monitoring Definitions") {
	SECTION("Valid request to delete Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 3> PMONIds = {0, 1, 2};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
	}

	SECTION("Two invalid requests to delete a Parameter Monitoring Definition and two valid ones") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 4;
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 1, 3, 4};
		request.appendEnum16(PMONIds.at(0));
		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(PMONIds.at(2));
		request.appendEnum16(PMONIds.at(3));
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinitionError) ==
		      1);

		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(onBoardMonitoringService.MonitoredParameterIds.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.MonitoredParameterIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.ParameterMonitoringCheckingStatus.end());
		CHECK(onBoardMonitoringService.RepetitionCounter.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.RepetitionCounter.end());
		CHECK(onBoardMonitoringService.RepetitionNumber.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.RepetitionNumber.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringStatus.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.ParameterMonitoringStatus.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.ParameterMonitoringCheckTypes.end());
	}
}

TEST_CASE("Modify Parameter Monitoring Definitions") {
	SECTION("Valid Request to modify Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
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
		request.appendEnum8(onBoardMonitoringService.LimitCheck);
		request.appendUint16(lowLimit);
		request.appendEnum8(onBoardMonitoringService.BelowLowLimitEvent);
		request.appendUint16(highLimit);
		request.appendEnum8(onBoardMonitoringService.AboveHighLimitEvent);

		request.appendEnum16(PMONIds.at(1));
		request.appendEnum16(monitoredParameterIds.at(1));
		request.appendUint16(repetitionNumbers.at(1));
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
		request.appendUint8(expetedValueCheckMask);
		// TODO: Find out how to append deduced values.
		request.appendEnum16(expectedValue);
		request.appendEnum8(onBoardMonitoringService.NotExpectedValueEvent);

		request.appendEnum16(PMONIds.at(2));
		request.appendEnum16(monitoredParameterIds.at(2));
		request.appendUint16(repetitionNumbers.at(2));
		request.appendEnum8(onBoardMonitoringService.ExpectedValueCheck);
		request.appendUint8(expetedValueCheckMask);
		// TODO: Find out how to append deduced values.
		request.appendEnum16(expectedValue);
		request.appendEnum8(onBoardMonitoringService.NotExpectedValueEvent);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(0)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(1)) == 0);
		CHECK(onBoardMonitoringService.RepetitionCounter.at(PMONIds.at(2)) == 0);
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)) == repetitionNumbers.at(0));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(1)) == repetitionNumbers.at(1));
		CHECK(onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(2)) == repetitionNumbers.at(2));
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(0)) ==
		      onBoardMonitoringService.LimitCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(1)) ==
		      onBoardMonitoringService.ExpectedValueCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckTypes.at(PMONIds.at(2)) ==
		      onBoardMonitoringService.ExpectedValueCheck);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(0)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(1)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringCheckingStatus.at(PMONIds.at(2)) ==
		      onBoardMonitoringService.Unchecked);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(0))->second.lowLimit == lowLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(0))->second.highLimit == highLimit);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(0))->second.belowLowLimitEvent ==
		      onBoardMonitoringService.BelowLowLimitEvent);
		CHECK(onBoardMonitoringService.LimitCheckParameters.find(PMONIds.at(0))->second.aboveHighLimitEvent ==
		      onBoardMonitoringService.AboveHighLimitEvent);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(1))->second.expectedValue ==
		      expectedValue);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(1))->second.mask ==
		      expetedValueCheckMask);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(1))->second.notExpectedValueEvent ==
		      onBoardMonitoringService.NotExpectedValueEvent);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(2))->second.expectedValue ==
		      expectedValue);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(2))->second.mask ==
		      expetedValueCheckMask);
		CHECK(onBoardMonitoringService.ExpectedValueCheckParameters.find(PMONIds.at(2))->second.notExpectedValueEvent ==
		      onBoardMonitoringService.NotExpectedValueEvent);
	}

	SECTION("Modify parameter not in the Parameter Monitoring List") {
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 4;
		uint16_t monitoredParameterId = 4;
		uint16_t repetitionNumber = 10;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ModifyParameterNotInTheParameterMonitoringList) == 1);
	}

	SECTION("Monitored parameter ID does not match the Parameter Monitoring Definition") {
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 1;
		uint16_t repetitionNumber = 10;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(
		          ErrorHandler::DifferentParameterMonitoringDefinitionAndMonitoredParameter) == 1);
	}
	SECTION("High limit is lower than low limit") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;
		uint16_t lowLimit = 8;
		uint16_t highLimit = 2;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.LimitCheck);
		request.appendUint16(lowLimit);
		request.appendEnum8(onBoardMonitoringService.BelowLowLimitEvent);
		request.appendUint16(highLimit);
		request.appendEnum8(onBoardMonitoringService.AboveHighLimitEvent);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
	}

	SECTION("High threshold is lower than low threshold") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		uint16_t monitoredParameterId = 0;
		uint16_t repetitionNumber = 5;
		uint16_t lowDeltaThreshold = 8;
		uint16_t highDeltaThreshold = 2;
		uint16_t numberOfConsecutiveDeltaChecks = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.appendEnum8(onBoardMonitoringService.DeltaCheck);
		request.appendUint16(lowDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.BelowLowThresholdEvent);
		request.appendUint16(highDeltaThreshold);
		request.appendEnum8(onBoardMonitoringService.AboveHighThresholdEvent);
		request.appendUint16(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);
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
		request.appendUint16(PMONIds.at(0));
		request.appendUint16(PMONIds.at(1));
		request.appendUint16(PMONIds.at(2));
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions);
		CHECK(report.readEnum16() == PMONIds.at(0));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(0)));
		CHECK(report.readEnumerated(1) == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)));
		CHECK(report.readUint8() == onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).mask);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).expectedValue);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).notExpectedValueEvent);
		CHECK(report.readEnum16() == PMONIds.at(1));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(1)));
		CHECK(report.readEnumerated(1) == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(1)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(1)));
		CHECK(report.readUint16() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).lowLimit);
		CHECK(report.readEnum8() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).belowLowLimitEvent);
		CHECK(report.readUint16() == onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).highLimit);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.LimitCheckParameters.at(PMONIds.at(1)).aboveHighLimitEvent);
		CHECK(report.readEnum16() == PMONIds.at(2));
		CHECK(report.readEnum16() == onBoardMonitoringService.MonitoredParameterIds.at(PMONIds.at(2)));
		CHECK(report.readEnumerated(1) == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(2)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(2)));
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
		CHECK(report.readEnumerated(1) == onBoardMonitoringService.ParameterMonitoringStatus.at(PMONIds.at(0)));
		CHECK(report.readEnum16() == onBoardMonitoringService.RepetitionNumber.at(PMONIds.at(0)));
		CHECK(report.readUint8() == onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).mask);
		CHECK(report.readUint16() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).expectedValue);
		CHECK(report.readEnum8() ==
		      onBoardMonitoringService.ExpectedValueCheckParameters.at(PMONIds.at(0)).notExpectedValueEvent);
		clearAllMaps();
	}
}

TEST_CASE("Report status of Parameter Monitoring Definitions") {
	initialiseParameterMonitoringDefinitions();
	Message request =
	    Message(OnBoardMonitoringService::ServiceType,
	            OnBoardMonitoringService::MessageType::ReportStatusOfParameterMonitoringDefinition, Message::TC, 0);
	MessageParser::execute(request);
	CHECK(ServiceTests::count() == 1);
	Message report = ServiceTests::get(0);
	CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
	CHECK(report.messageType == OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionStatusReport);
	CHECK(report.readUint16() == onBoardMonitoringService.ParameterMonitoringList.size());
	CHECK(report.readUint16() == 0);
	CHECK(report.readEnumerated(1) == false);
	CHECK(report.readUint16() == 1);
	CHECK(report.readEnumerated(1) == false);
	CHECK(report.readUint16() == 2);
	CHECK(report.readEnumerated(1) == false);
	CHECK(report.readUint16() == 3);
	CHECK(report.readEnumerated(1) == false);
	clearAllMaps();
}