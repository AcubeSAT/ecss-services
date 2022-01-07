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

	onBoardMonitoringService.CheckTransitionList.insert({});

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
	onBoardMonitoringService.ParameterMonitoringCheckTypes.insert({3, onBoardMonitoringService.DeltaCheck});

	struct OnBoardMonitoringService::ExpectedValueCheck expectedValueCheck = {
	    5, 8, onBoardMonitoringService.NotExpectedValueEvent};
	onBoardMonitoringService.ExpectedValueCheckParameters.insert({0, expectedValueCheck});
	struct OnBoardMonitoringService::LimitCheck limitCheck = {1, onBoardMonitoringService.BelowLowLimitEvent, 2,
	                                                          onBoardMonitoringService.AboveHighLimitEvent};
	onBoardMonitoringService.LimitCheckParameters.insert({1, limitCheck});
	struct OnBoardMonitoringService::DeltaCheck deltaCheck = {5, 2, onBoardMonitoringService.BelowLowThresholdEvent, 10,
	                                                          onBoardMonitoringService.AboveHighThresholdEvent};
	onBoardMonitoringService.DeltaCheckParameters.insert({2, deltaCheck});
	onBoardMonitoringService.DeltaCheckParameters.insert({3, deltaCheck});
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
	clearAllMaps();
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
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
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
		clearAllMaps();
		ServiceTests::reset();
		Services.reset();
	}
}
