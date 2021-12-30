#include <catch2/catch.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <ServicePool.hpp>

OnBoardMonitoringService& onBoardMonitoringService = Services.onBoardMonitoringService;

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
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidRequestToDeleteAllParameterMonitoringDefinitionsError) == 1);
		CHECK(!onBoardMonitoringService.ParameterMonitoringList.empty());
		CHECK(!onBoardMonitoringService.CheckTransitionList.empty());
	}
}