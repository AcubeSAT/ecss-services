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

void initialiseParameterMonitoringDefinitions() {
	auto parameter1 = Parameter<uint8_t>(3);
	auto parameter2 = Parameter<uint16_t>(7);
	auto parameter3 = Parameter<uint32_t>(9);
	auto parameter4 = Parameter<uint32_t>(2);

	etl::array<PMONExpectedValueCheck::CheckingStatus, 2> checkTransitionList = {};
	auto monitoringDefinition1 =
	    PMONExpectedValueCheck(parameter1, PMONExpectedValueCheck::ExpectedValueCheck, 0, 10, 0, false,
	                           PMONExpectedValueCheck::Unchecked, checkTransitionList, 5, 8, 1);
	auto monitoringDefinition2 = PMONLimitCheck(parameter2, PMONBase::DeltaCheck, 0, 10, 0, false, PMONBase::Unchecked,
	                                            checkTransitionList, 1, 1, 2, 2);
	auto monitoringDefinition3 = PMONDeltaCheck(parameter3, PMONBase::DeltaCheck, 0, 10, 0, false, PMONBase::Unchecked,
	                                            checkTransitionList, 5, 2, 1, 10, 2);
	auto monitoringDefinition4 = PMONDeltaCheck(parameter4, PMONBase::DeltaCheck, 0, 10, 0, true, PMONBase::Unchecked,
	                                            checkTransitionList, 5, 2, 1, 10, 2);

	onBoardMonitoringService.parameterMonitoringFunctionStatus = true;
	onBoardMonitoringService.maximumTransitionReportingDelay = 0;
	onBoardMonitoringService.ParameterMonitoringList.insert({0, monitoringDefinition1});
	onBoardMonitoringService.ParameterMonitoringList.insert({1, monitoringDefinition2});
	onBoardMonitoringService.ParameterMonitoringList.insert({2, monitoringDefinition3});
	onBoardMonitoringService.ParameterMonitoringList.insert({3, monitoringDefinition4});
};

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
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().repetitionCounter == 0);

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
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().monitoringStatus == true);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().repetitionCounter == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().repetitionCounter == 0);

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
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().checkingStatus ==
		      PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().checkingStatus ==
		      PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().checkingStatus ==
		      PMONBase::Unchecked);

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
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().monitoringStatus == false);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(0)).get().checkingStatus ==
		      PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(1)).get().checkingStatus ==
		      PMONBase::Unchecked);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.at(PMONIds.at(2)).get().checkingStatus ==
		      PMONBase::Unchecked);

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

		CHECK(onBoardMonitoringService.ParameterMonitoringList.empty());

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

		ServiceTests::reset();
		Services.reset();
	}
}
