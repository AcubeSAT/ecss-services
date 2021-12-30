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

	onBoardMonitoringService.RepetitionNumber.insert({parameter1, 10});
	onBoardMonitoringService.RepetitionNumber.insert({parameter2, 2});
	onBoardMonitoringService.RepetitionNumber.insert({parameter3, 5});
	onBoardMonitoringService.RepetitionNumber.insert({parameter4, 6});

	onBoardMonitoringService.ParameterMonitoringList.insert({0, parameter1});
	onBoardMonitoringService.ParameterMonitoringList.insert({1, parameter2});
	onBoardMonitoringService.ParameterMonitoringList.insert({2, parameter3});
	onBoardMonitoringService.ParameterMonitoringList.insert({3, parameter3});

	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter1, 0});
	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter2, 1});
	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter3, 2});
	onBoardMonitoringService.ParameterMonitoringIds.insert({parameter4, 3});

	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter1, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter2, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter3, false});
	onBoardMonitoringService.ParameterMonitoringStatus.insert({parameter4, true});
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
/**
 * Function to return the parameters initialised in this file.
 * @param parameterId
 */
std::optional<std::reference_wrapper<ParameterBase>> getParameter(uint16_t parameterId){
	auto parameter = onBoardMonitoringService.ParameterMonitoringList.find(parameterId);

	if (parameter != onBoardMonitoringService.ParameterMonitoringList.end()) {
		return parameter->second;
	} else {
		return {};
	}
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
		CHECK(ServiceTests::count() == 0);

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
		CHECK(ServiceTests::count() == 0);
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
		CHECK(onBoardMonitoringService.CheckTransitionList.empty());
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
		CHECK(ServiceTests::count() == 0);

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

	SECTION("Parameter Monitoring List is full") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 3;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ParameterMonitoringListIsFull) == 1);
		clearAllMaps();
	}

	SECTION("Parameter Monitoring Definition already exists") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendUint16(PMONId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
		clearAllMaps();
	}

	SECTION("High limit is lower than low limit") {
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
		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinitionError) == 1);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(0))->get()) ==
		    onBoardMonitoringService.ParameterMonitoringIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(1))->get()) ==
		    onBoardMonitoringService.ParameterMonitoringIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(3)) !=
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(3))->get()) !=
		    onBoardMonitoringService.ParameterMonitoringIds.end());
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
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(0)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(0))->get()) ==
		    onBoardMonitoringService.ParameterMonitoringIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(1)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(1))->get()) ==
		    onBoardMonitoringService.ParameterMonitoringIds.end());
		CHECK(onBoardMonitoringService.ParameterMonitoringList.find(PMONIds.at(2)) ==
		      onBoardMonitoringService.ParameterMonitoringList.end());
		CHECK(
		    onBoardMonitoringService.ParameterMonitoringIds.find(getParameter(PMONIds.at(2))->get()) ==
		    onBoardMonitoringService.ParameterMonitoringIds.end());
	}
}