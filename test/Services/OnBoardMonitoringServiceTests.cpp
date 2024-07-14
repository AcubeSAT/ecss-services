#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <catch2/catch_all.hpp>
#include <etl/array.h>
#include "ServiceTests.hpp"
#include "etl/functional.h"

OnBoardMonitoringService& onBoardMonitoringService = Services.onBoardMonitoringService;

struct Fixtures {
	PMONExpectedValueCheck monitoringDefinition1 = PMONExpectedValueCheck(8, 5, 10, 8, 0);
	PMONLimitCheck monitoringDefinition2 = PMONLimitCheck(6, 5, 2, 1, 9, 2);
	PMONDeltaCheck monitoringDefinition3 = PMONDeltaCheck(9, 5, 5, 3, 3, 11, 4);
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

	onBoardMonitoringService.addPMONExpectedValueCheck(0, etl::ref(fixtures.monitoringDefinition1));
	onBoardMonitoringService.addPMONLimitCheck(1, etl::ref(fixtures.monitoringDefinition2));
	onBoardMonitoringService.addPMONDeltaCheck(2, etl::ref(fixtures.monitoringDefinition3));
	onBoardMonitoringService.addPMONDeltaCheck(3, etl::ref(fixtures.monitoringDefinition4));
}

TEST_CASE("Enable Parameter Monitoring Definitions") {
	SECTION("3 valid requests to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<ParameterId, 3> PMONIds = {0, 1, 2};
		request.append<ParameterId>(PMONIds[0]);
		request.append<ParameterId>(PMONIds[1]);
		request.append<ParameterId>(PMONIds[2]);

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
		etl::array<ParameterId, 4> PMONIds = {0, 10, 1};
		request.append<ParameterId>(PMONIds[0]);
		request.append<ParameterId>(PMONIds[1]);
		request.append<ParameterId>(PMONIds[2]);

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
		etl::array<ParameterId, 3> PMONIds = {0, 1, 2};
		request.append<ParameterId>(PMONIds[0]);
		request.append<ParameterId>(PMONIds[1]);
		request.append<ParameterId>(PMONIds[2]);
		onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled = true;

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().checkingStatus == PMON::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().checkingStatus == PMON::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().checkingStatus == PMON::Unchecked);

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
		etl::array<ParameterId, 4> PMONIds = {0, 10, 1, 2};
		request.append<ParameterId>(PMONIds[0]);
		request.append<ParameterId>(PMONIds[1]);
		request.append<ParameterId>(PMONIds[2]);
		request.append<ParameterId>(PMONIds[3]);
		onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled = true;
		onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled = true;
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled == false);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().checkingStatus == PMON::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().checkingStatus == PMON::Unchecked);
		CHECK(onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().checkingStatus == PMON::Unchecked);

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
		uint16_t numberOfIds = 1;
		request.appendUint16(numberOfIds);
		ParameterId PMONId = 0;
		ParameterId monitoredParameterId = 0;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONBitMask expectedValueCheckMask = 2;
		PMONExpectedValue expectedValue = 10;
		EventDefinitionId unexpectedValueEvent = 5;
		PMONLimit lowLimit = 3;
		EventDefinitionId belowLowLimitEvent = 8;
		PMONLimit highLimit = 8;
		EventDefinitionId aboveHighLimitEvent = 10;

		request.appendEnum16(PMONId);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEvent);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEvent);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);
		CHECK(onBoardMonitoringService.getCount(PMONId) == 1);

		auto definition = onBoardMonitoringService.getPMONDefinition(PMONId);
		auto& pmon = definition.get();

		CHECK(pmon.getRepetitionNumber() == repetitionNumber);
		CHECK(pmon.isMonitoringEnabled() == false);
		CHECK(pmon.getCheckingStatus() == PMON::Unchecked);

		auto& limitCheck = static_cast<PMONLimitCheck&>(pmon);
		CHECK(limitCheck.getLowLimit() == lowLimit);
		CHECK(limitCheck.getBelowLowLimitEvent() == belowLowLimitEvent);
		CHECK(limitCheck.getHighLimit() == highLimit);
		CHECK(limitCheck.getAboveHighLimitEvent() == aboveHighLimitEvent);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameter Monitoring List is full") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId monitoredParameterId = 5;
		PMONRepetitionNumber repetitionNumber = 5;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::ExpectedValue);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ParameterMonitoringListIsFull) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameter Monitoring Definition already exists") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId monitoredParameterId = 0;
		PMONRepetitionNumber repetitionNumber = 0;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::ExpectedValue);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Add Parameter Monitoring Definition with a non-existing parameter") {
		uint16_t numberOfIds = 1;
		PMONRepetitionNumber repetitionNumber = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::ExpectedValue);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High limit is lower than low limit") {
		uint16_t numberOfIds = 1;
		ParameterId PMONId = 0;
		ParameterId monitoredParameterId = 0;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 6;
		PMONLimit highLimit = 2;
		EventDefinitionId belowLowLimitEventId = 1;
		EventDefinitionId aboveHighLimitEventId = 2;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONId);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEventId);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEventId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High threshold is lower than low threshold") {
		uint16_t numberOfIds = 1;
		ParameterId PMONId = 0;
		ParameterId monitoredParameterId = 0;
		PMONRepetitionNumber repetitionNumber = 5;
		DeltaThreshold lowDeltaThreshold = 8;
		DeltaThreshold highDeltaThreshold = 2;
		EventDefinitionId belowLowThresholdEventId = 1;
		EventDefinitionId aboveHighThresholdEventId = 2;
		NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Delta);
		request.append<DeltaThreshold>(lowDeltaThreshold);
		request.append<EventDefinitionId>(belowLowThresholdEventId);
		request.append<DeltaThreshold>(highDeltaThreshold);
		request.append<EventDefinitionId>(aboveHighThresholdEventId);
		request.append<NumberOfConsecutiveDeltaChecks>(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delete Parameter Monitoring Definitions") {
	SECTION("Valid request to Delete Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		const uint16_t numberOfIds = 2;
		etl::array<uint16_t, numberOfIds> PMONIds = {2, 3};

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);

		MessageParser::execute(request);

		CHECK(onBoardMonitoringService.getCount(PMONIds[0]) == 0);
		CHECK(onBoardMonitoringService.getCount(PMONIds[1]) == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Invalid request to Delete Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		const uint16_t numberOfIds = 1;
		uint16_t PMONId = 0;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteParameterMonitoringDefinitions, Message::TC, 0);

		request.appendUint16(numberOfIds);
		request.appendEnum16(PMONId);

		MessageParser::execute(request);

		CHECK(onBoardMonitoringService.getCount(PMONId) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("One valid and one invalid request to Delete Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		const uint16_t numberOfIds = 2;
		etl::array<uint16_t, numberOfIds> PMONIds = {0, 1};

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::DeleteParameterMonitoringDefinitions, Message::TC, 0);

		request.appendUint16(numberOfIds);
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::InvalidRequestToDeleteParameterMonitoringDefinition) == 1);

		CHECK(onBoardMonitoringService.getCount(PMONIds[0]) == 1);
		CHECK(onBoardMonitoringService.getCount(PMONIds[1]) == 0);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Modify Parameter Monitoring Definitions") {
	SECTION("Valid Request to Modify Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId PMONId = 1;
		ParameterId monitoredParameterId = 6;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONId);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEvent);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEvent);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 0);

		CHECK(onBoardMonitoringService.getCount(PMONId) == 1);

		auto& baseDefinition = onBoardMonitoringService.getPMONDefinition(PMONId).get();
		auto& modifiedDefinition = static_cast<PMONLimitCheck&>(baseDefinition);

		CHECK(modifiedDefinition.repetitionNumber == repetitionNumber);
		CHECK(modifiedDefinition.lowLimit == lowLimit);
		CHECK(modifiedDefinition.highLimit == highLimit);
		CHECK(modifiedDefinition.belowLowLimitEvent == belowLowLimitEvent);
		CHECK(modifiedDefinition.aboveHighLimitEvent == aboveHighLimitEvent);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Modify Parameter Not In The Parameter Monitoring List") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId PMONIdNotInList = 100;
		ParameterId monitoredParameterId = 7;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONIdNotInList);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEvent);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEvent);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ModifyParameterNotInTheParameterMonitoringList) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Different Parameter Monitoring Definition And Monitored Parameter") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId existingPMONId = 1;
		ParameterId wrongMonitoredParameterId = 999;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(existingPMONId);
		request.append<ParameterId>(wrongMonitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEvent);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEvent);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DifferentParameterMonitoringDefinitionAndMonitoredParameter) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High Limit Is Lower Than Low Limit") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId PMONId = 1;
		ParameterId monitoredParameterId = 6;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 10;
		PMONLimit highLimit = 4;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONId);
		request.append<ParameterId>(monitoredParameterId);
		request.append<PMONRepetitionNumber>(repetitionNumber);
		request.append<PMON::CheckType> (PMON::CheckType::Limit);
		request.append<PMONLimit>(lowLimit);
		request.append<EventDefinitionId>(belowLowLimitEvent);
		request.append<PMONLimit>(highLimit);
		request.append<EventDefinitionId>(aboveHighLimitEvent);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High Threshold Is Lower Than Low Threshold") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		ParameterId PMONId = 1;
		ParameterId monitoredParameterId = 6;
		PMONRepetitionNumber repetitionNumber = 5;
		DeltaThreshold lowDeltaThreshold = 8;
		DeltaThreshold highDeltaThreshold = 2;
		EventDefinitionId belowLowThresholdEventId = 1;
		EventDefinitionId aboveHighThresholdEventId = 2;
		NumberOfConsecutiveDeltaChecks numberOfConsecutiveDeltaChecks = 5;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<ParameterId>(PMONId);
		request.appendUint16(monitoredParameterId);
		request.appendUint16(repetitionNumber);
		request.append<PMON::CheckType>(PMON::CheckType::Delta);
		request.append<DeltaThreshold>(lowDeltaThreshold);
		request.append<EventDefinitionId>(belowLowThresholdEventId);
		request.append<DeltaThreshold>(highDeltaThreshold);
		request.append<EventDefinitionId>(aboveHighThresholdEventId);
		request.append<NumberOfConsecutiveDeltaChecks>(numberOfConsecutiveDeltaChecks);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Report Parameter Monitoring Definitions") {
	SECTION("Valid request to report Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 4;
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::ReportParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		etl::array<uint16_t, 4> PMONIds = {0, 1, 2, 3};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		request.appendEnum16(PMONIds[2]);
		request.appendEnum16(PMONIds[3]);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport);
		CHECK(report.readUint16() == onBoardMonitoringService.maximumTransitionReportingDelay);
		CHECK(report.readUint16() == numberOfIds);

		auto& pmon0 = onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get();

		CHECK(report.readEnum16() == PMONIds[0]);
		CHECK(report.read<ParameterId>() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled);
		CHECK(report.read<PMONRepetitionNumber>() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().repetitionNumber);

		REQUIRE(pmon0.getCheckType() == PMON::CheckType::ExpectedValue);
		auto expectedValueCheck = static_cast<PMONExpectedValueCheck&>(pmon0);
		CHECK(report.read<PMON::CheckType>() == (PMON::CheckType::ExpectedValue));
		CHECK(report.read<PMONBitMask>() == expectedValueCheck.getMask());
		CHECK(report.read<PMONExpectedValue>() == expectedValueCheck.getExpectedValue());
		CHECK(report.read<EventDefinitionId>() == expectedValueCheck.getUnexpectedValueEvent());

		auto& pmon1 = onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get();

		CHECK(report.readEnum16() == PMONIds[1]);
		CHECK(report.read<ParameterId>() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().monitoringEnabled);
		CHECK(report.read<PMONRepetitionNumber>() == onBoardMonitoringService.getPMONDefinition(PMONIds[1]).get().repetitionNumber);

		REQUIRE(pmon1.getCheckType() == PMON::CheckType::Limit);
		auto limitCheck = static_cast<PMONLimitCheck&>(pmon1);
		CHECK(report.read<PMON::CheckType>() == (PMON::CheckType::Limit));
		CHECK(report.read<PMONLimit>() == limitCheck.getLowLimit());
		CHECK(report.read<EventDefinitionId>() == limitCheck.getBelowLowLimitEvent());
		CHECK(report.read<PMONLimit>() == limitCheck.getHighLimit());
		CHECK(report.read<EventDefinitionId>() == limitCheck.getAboveHighLimitEvent());

		auto& pmon2 = onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get();
		CHECK(report.readEnum16() == PMONIds[2]);
		CHECK(report.read<ParameterId>() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().monitoringEnabled);
		CHECK(report.read<PMONRepetitionNumber>() == onBoardMonitoringService.getPMONDefinition(PMONIds[2]).get().repetitionNumber);

		REQUIRE(pmon2.getCheckType() == PMON::CheckType::Delta);
		auto deltaCheck = static_cast<PMONDeltaCheck&>(pmon2);
		CHECK(report.read<PMON::CheckType>() == (PMON::CheckType::Delta));
		CHECK(report.read<DeltaThreshold>() == deltaCheck.getLowDeltaThreshold());
		CHECK(report.read<EventDefinitionId>() == deltaCheck.getBelowLowThresholdEvent());
		CHECK(report.read<DeltaThreshold>() == deltaCheck.getHighDeltaThreshold());
		CHECK(report.read<EventDefinitionId>() == deltaCheck.getAboveHighThresholdEvent());
		CHECK(report.read<NumberOfConsecutiveDeltaChecks>() == deltaCheck.getNumberOfConsecutiveDeltaChecks());

		auto& pmon3 = onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get();
		CHECK(report.readEnum16() == PMONIds[3]);
		CHECK(report.read<ParameterId>() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().monitoringEnabled);
		CHECK(report.read<PMONRepetitionNumber>() == onBoardMonitoringService.getPMONDefinition(PMONIds[3]).get().repetitionNumber);

		REQUIRE(pmon3.getCheckType() == PMON::CheckType::Delta);
		auto deltaCheck1 = static_cast<PMONDeltaCheck&>(pmon3);
		CHECK(report.read<PMON::CheckType>() == (PMON::CheckType::Delta));
		CHECK(report.read<DeltaThreshold>() == deltaCheck1.getLowDeltaThreshold());
		CHECK(report.read<EventDefinitionId>() == deltaCheck1.getBelowLowThresholdEvent());
		CHECK(report.read<DeltaThreshold>() == deltaCheck1.getHighDeltaThreshold());
		CHECK(report.read<EventDefinitionId>() == deltaCheck1.getAboveHighThresholdEvent());
		CHECK(report.read<NumberOfConsecutiveDeltaChecks>() == deltaCheck1.getNumberOfConsecutiveDeltaChecks());

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
		request.appendEnum16(PMONId);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);
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
		etl::array<uint16_t, 2> PMONIds = {0, 5};
		request.appendEnum16(PMONIds[0]);
		request.appendEnum16(PMONIds[1]);
		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ReportParameterNotInTheParameterMonitoringList) == 1);

		Message report = ServiceTests::get(1);

		CHECK(report.serviceType == OnBoardMonitoringService::ServiceType);
		CHECK(report.messageType == OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport);
		CHECK(report.readUint16() == onBoardMonitoringService.maximumTransitionReportingDelay);
		CHECK(report.readUint16() == numberOfIds);

		auto definitionOpt0 = onBoardMonitoringService.getPMONDefinition(PMONIds[0]);
		auto& pmon0 = definitionOpt0.get();

		CHECK(report.readEnum16() == PMONIds[0]);
		CHECK(report.read<ParameterId>() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoredParameterId);
		CHECK(report.readEnum8() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().monitoringEnabled);
		CHECK(report.read<PMONRepetitionNumber>() == onBoardMonitoringService.getPMONDefinition(PMONIds[0]).get().repetitionNumber);

		REQUIRE(pmon0.getCheckType() == PMON::CheckType::ExpectedValue);
		auto expectedValueCheck = static_cast<PMONExpectedValueCheck&>(pmon0);
		CHECK(report.read<PMON::CheckType>() == (PMON::CheckType::ExpectedValue));
		CHECK(report.read<PMONBitMask>() == expectedValueCheck.getMask());
		CHECK(report.read<PMONExpectedValue>() == expectedValueCheck.getExpectedValue());
		CHECK(report.read<EventDefinitionId>() == expectedValueCheck.getUnexpectedValueEvent());

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Limit Check Behavior") {
	SECTION("Value within limits") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition2;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::WithinLimits);
		CHECK(pmon.getRepetitionCounter() == 1);

		pmon.performCheck();
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Value below low limit") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition2;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(1);
		REQUIRE(param.getValue() == 1);

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowLimit);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Value above high limit") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition2;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		REQUIRE(param.getValue() == 10);

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::AboveHighLimit);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Repetition Counter Resets on Status Change") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition2;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::WithinLimits);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getRepetitionCounter() == 2);

		param.setValue(1);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowLimit);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(1);
		pmon.performCheck();
		CHECK(pmon.getRepetitionCounter() == 2);

		param.setValue(10);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::AboveHighLimit);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::WithinLimits);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Continuous Monitoring Within Limits") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition2;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		for (unsigned char i = 2; i <= 8; ++i) {
			param.setValue(i);
			pmon.performCheck();
			CHECK(pmon.getCheckingStatus() == PMON::WithinLimits);
		}

		param.setValue(1);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowLimit);

		param.setValue(10);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::AboveHighLimit);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Expected Value Check Behavior") {
	SECTION("Value matches expected value") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		pmon.mask = 0xFF;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::ExpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		pmon.performCheck();
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Value does not match expected value") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(5);
		pmon.mask = 0xFF;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		pmon.performCheck();
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Status Changes from Unexpected to Expected") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(5);
		pmon.mask = 0xFF;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(10);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::ExpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Repetition Counter Resets on Status Change") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		pmon.mask = 0xFF;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::ExpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(10);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::ExpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Edge Cases for Parameter Values") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(0);
		pmon.mask = 0xFF;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(255);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Mask with zeros (no bits set)") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		pmon.mask = 0x00;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Mask with some bits set to zero") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition1;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(0xFF);
		pmon.mask = 0b1111'0000;
		pmon.expectedValue = 0b1111'0000;

		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::ExpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		param.setValue(0b0000'1111);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::UnexpectedValue);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delta Check Perform Check") {
	SECTION("Delta threshold checks including negative delta") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition3;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 0));
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::Invalid);
		CHECK(pmon.getRepetitionCounter() == 1);


		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 15));
		param.setValue(180);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::AboveHighThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 30));
		param.setValue(5);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Repetition Counter Behavior") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition3;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 0));
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::Invalid);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 15));
		param.setValue(20);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 30));
		param.setValue(50);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowThreshold);
		CHECK(pmon.getRepetitionCounter() == 2);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 45));
		param.setValue(100);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::WithinThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Repetition Counter Resets on Status Change") {
		initialiseParameterMonitoringDefinitions();
		auto& pmon = fixtures.monitoringDefinition3;
		auto& param = static_cast<Parameter<unsigned char>&>(pmon.monitoredParameter.get());

		param.setValue(10);
		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 0));
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::Invalid);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 15));
		param.setValue(20);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::BelowLowThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::setMockTime(UTCTimestamp(2024, 4, 10, 10, 15, 30));
		param.setValue(100);
		pmon.performCheck();
		CHECK(pmon.getCheckingStatus() == PMON::WithinThreshold);
		CHECK(pmon.getRepetitionCounter() == 1);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Check All Behavior") {
    SECTION("monitoringDefinition1 and 2 enabled") {
        initialiseParameterMonitoringDefinitions();
        auto& pmon1 = onBoardMonitoringService.getPMONDefinition(0).get();
    	auto& pmonExpected = static_cast<PMONExpectedValueCheck&>(pmon1);
        auto& pmon2 = onBoardMonitoringService.getPMONDefinition(1).get();
    	auto& pmonLimit = static_cast<PMONLimitCheck&>(pmon2);
        auto& pmon3 = onBoardMonitoringService.getPMONDefinition(2).get();
    	auto& pmonDelta = static_cast<PMONDeltaCheck&>(pmon3);

        auto& param1 = static_cast<Parameter<unsigned char>&>(pmon1.monitoredParameter.get());
        auto& param2 = static_cast<Parameter<unsigned char>&>(pmon2.monitoredParameter.get());
        auto& param3 = static_cast<Parameter<unsigned char>&>(pmon3.monitoredParameter.get());

        param1.setValue(10);
        pmonExpected.mask = 0xFF;
        param2.setValue(5);
    	pmonLimit.monitoringEnabled = true;
        param3.setValue(100);

    	CHECK(pmonExpected.getCheckingStatus() == PMON::Unchecked);
    	CHECK(pmonLimit.getCheckingStatus() == PMON::Unchecked);
    	CHECK(pmonDelta.getCheckingStatus() == PMON::Unchecked);

    	onBoardMonitoringService.checkAll();

    	CHECK(pmonExpected.getCheckingStatus() == PMON::ExpectedValue);
    	CHECK(pmonLimit.getCheckingStatus() == PMON::WithinLimits);
    	CHECK(pmonDelta.getCheckingStatus() == PMON::Unchecked);

        ServiceTests::reset();
        Services.reset();
    }

	SECTION("Monitoring definitions initially disabled, then enabled") {
    	initialiseParameterMonitoringDefinitions();
    	auto& pmon1 = onBoardMonitoringService.getPMONDefinition(0).get();
    	auto& pmonExpected = static_cast<PMONExpectedValueCheck&>(pmon1);
    	auto& pmon2 = onBoardMonitoringService.getPMONDefinition(1).get();
    	auto& pmonLimit = static_cast<PMONLimitCheck&>(pmon2);

    	auto& param1 = static_cast<Parameter<unsigned char>&>(pmon1.monitoredParameter.get());
    	auto& param2 = static_cast<Parameter<unsigned char>&>(pmon2.monitoredParameter.get());

    	param1.setValue(10);
    	pmonExpected.mask = 0xFF;
    	param2.setValue(5);
    	pmonExpected.monitoringEnabled = false;
    	pmonLimit.monitoringEnabled = false;

    	onBoardMonitoringService.checkAll();

    	CHECK(pmonExpected.getCheckingStatus() == PMON::Unchecked);
    	CHECK(pmonLimit.getCheckingStatus() == PMON::Unchecked);

    	pmonExpected.monitoringEnabled = true;
    	pmonLimit.monitoringEnabled = true;

    	onBoardMonitoringService.checkAll();

    	CHECK(pmonExpected.getCheckingStatus() == PMON::ExpectedValue);
    	CHECK(pmonLimit.getCheckingStatus() == PMON::WithinLimits);

    	ServiceTests::reset();
    	Services.reset();
    }

	SECTION("Multiple consecutive calls to checkAll") {
    	initialiseParameterMonitoringDefinitions();
    	auto& pmon1 = onBoardMonitoringService.getPMONDefinition(0).get();
    	auto& pmonExpected = static_cast<PMONExpectedValueCheck&>(pmon1);

    	auto& param1 = static_cast<Parameter<unsigned char>&>(pmon1.monitoredParameter.get());

    	param1.setValue(10);
    	pmonExpected.mask = 0xFF;

    	onBoardMonitoringService.checkAll();
    	CHECK(pmonExpected.getCheckingStatus() == PMON::ExpectedValue);

    	param1.setValue(5);
    	onBoardMonitoringService.checkAll();
    	CHECK(pmonExpected.getCheckingStatus() == PMON::UnexpectedValue);

    	param1.setValue(10);
    	onBoardMonitoringService.checkAll();
    	CHECK(pmonExpected.getCheckingStatus() == PMON::ExpectedValue);

    	ServiceTests::reset();
    	Services.reset();
    }
}

