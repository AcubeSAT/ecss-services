#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/OnBoardMonitoringService.hpp>
#include <catch2/catch_all.hpp>
#include <etl/array.h>
#include "ServiceTests.hpp"
#include "etl/functional.h"

OnBoardMonitoringService& onBoardMonitoringService = Services.onBoardMonitoringService;

static constexpr PMONDefinitionId ExpectedValueCheckId = 0;
static constexpr ParameterId ExpectedValueCheckMonitoredParameterId = 8;
static constexpr PMONDefinitionId LimitCheckId = 1;
static constexpr ParameterId LimitCheckMonitoredParameterId = 6;
static constexpr PMONDefinitionId DeltaCheckId = 2;
static constexpr ParameterId DeltaCheckMonitoredParameterId = 9;

struct Fixtures {
	PMONExpectedValueCheck monitoringDefinition1 = PMONExpectedValueCheck(ExpectedValueCheckMonitoredParameterId, 5, 10, 8, 0);
	PMONLimitCheck monitoringDefinition2 = PMONLimitCheck(LimitCheckMonitoredParameterId, 5, 2, 1, 9, 2);
	PMONDeltaCheck monitoringDefinition3 = PMONDeltaCheck(DeltaCheckMonitoredParameterId, 5, 5, 3, 3, 11, 4);
	PMONDeltaCheck monitoringDefinition4 = PMONDeltaCheck(7, 5, 5, 3, 3, 11, 4);

	/*
	 * Constructor to modify monitoring definitions if needed
	 */
	Fixtures() {
		monitoringDefinition1.monitoringEnabled = true;
	}
	/**
	 * Singleton function to access the instance	
	 */
	static Fixtures& getInstance() {
		if (not instance.has_value()){
			instance.emplace();
		}
		return *instance;
	}
	/**
	 * Function to reset the singleton
	 */
	static void reset(){
		instance.reset();
	}
	private:
		/**
		* Singleton instance
		*/
		static std::optional<Fixtures> instance;
};

	std::optional<Fixtures> Fixtures::instance{};

void initialiseParameterMonitoringDefinitions() {
	// Reset fixtures to the defaults set up by the constructor
	Fixtures::reset();
	onBoardMonitoringService.addPMONExpectedValueCheck(ExpectedValueCheckId, etl::ref(Fixtures::getInstance().monitoringDefinition1));
	onBoardMonitoringService.addPMONLimitCheck(LimitCheckId, etl::ref(Fixtures::getInstance().monitoringDefinition2));
	onBoardMonitoringService.addPMONDeltaCheck(DeltaCheckId, etl::ref(Fixtures::getInstance().monitoringDefinition3));
	onBoardMonitoringService.addPMONDeltaCheck(3, etl::ref(Fixtures::getInstance().monitoringDefinition4));
}

TEST_CASE("Enable Parameter Monitoring Definitions") {
	SECTION("3 valid requests to enable Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::EnableParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 3;
		request.appendUint16(numberOfIds);
		etl::array<PMONDefinitionId, 3> PMONIds = {0, 1, 2};
		request.append<PMONDefinitionId>(PMONIds[0]);
		request.append<PMONDefinitionId>(PMONIds[1]);
		request.append<PMONDefinitionId>(PMONIds[2]);

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
		etl::array<PMONDefinitionId, 4> PMONIds = {0, 10, 1};
		request.append<PMONDefinitionId>(PMONIds[0]);
		request.append<PMONDefinitionId>(PMONIds[1]);
		request.append<PMONDefinitionId>(PMONIds[2]);

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
		etl::array<PMONDefinitionId, 3> PMONIds = {0, 1, 2};
		request.append<PMONDefinitionId>(PMONIds[0]);
		request.append<PMONDefinitionId>(PMONIds[1]);
		request.append<PMONDefinitionId>(PMONIds[2]);
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
		etl::array<PMONDefinitionId, 4> PMONIds = {0, 10, 1, 2};
		request.append<PMONDefinitionId>(PMONIds[0]);
		request.append<PMONDefinitionId>(PMONIds[1]);
		request.append<PMONDefinitionId>(PMONIds[2]);
		request.append<PMONDefinitionId>(PMONIds[3]);
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

static constexpr ParameterId ExistingParameterId = 0;
static constexpr ParameterId NonExistingParameterId = ECSSParameterCount + 1;
static constexpr PMONDefinitionId NewPMONId = 10;
static constexpr PMONDefinitionId SecondNewPMONId = 11;
static constexpr auto UnknownCheckType = static_cast<PMON::CheckType>(4);

static constexpr PMONRepetitionNumber NewRepetitionNumber = 7;
static constexpr PMONBitMask NewMask = 1;
static constexpr PMONExpectedValue NewExpectedValue = 2;
static constexpr EventDefinitionId NewUnexpectedValueEvent = 9;
static constexpr PMONLimit NewLowLimit = 4;
static constexpr EventDefinitionId NewBelowLowLimitEvent = 9;
static constexpr PMONLimit NewHighLimit = 10;
static constexpr EventDefinitionId NewAboveHighLimitEvent = 11;
static constexpr DeltaThreshold NewLowDeltaThreshold = 2;
static constexpr EventDefinitionId NewBelowLowThresholdEvent = 1;
static constexpr DeltaThreshold NewHighDeltaThreshold = 8;
static constexpr EventDefinitionId NewAboveHighThresholdEvent = 2;
static constexpr NumberOfConsecutiveDeltaChecks NewNumberOfConsecutiveDeltaChecks = 5;

static void appendCommonFields(Message& request, PMONDefinitionId PMONId, ParameterId monitoredParameterId, PMON::CheckType checkType) {
	request.append<PMONDefinitionId>(PMONId);
	request.append<ParameterId>(monitoredParameterId);
	request.append<PMONRepetitionNumber>(NewRepetitionNumber);
	request.append<PMON::CheckType>(checkType);
}

static void appendExpectedValueCheck(Message& request, PMONDefinitionId PMONId, ParameterId monitoredParameterId) {
	appendCommonFields(request, PMONId, monitoredParameterId, PMON::CheckType::ExpectedValue);
	request.append<PMONBitMask>(NewMask);
	request.append<PMONExpectedValue>(NewExpectedValue);
	request.append<EventDefinitionId>(NewUnexpectedValueEvent);
}

static void appendLimitCheck(Message& request, PMONDefinitionId PMONId, ParameterId monitoredParameterId, PMONLimit lowLimit, PMONLimit highLimit) {
	appendCommonFields(request, PMONId, monitoredParameterId, PMON::CheckType::Limit);
	request.append<PMONLimit>(lowLimit);
	request.append<EventDefinitionId>(NewBelowLowLimitEvent);
	request.append<PMONLimit>(highLimit);
	request.append<EventDefinitionId>(NewAboveHighLimitEvent);
}

static void appendDeltaCheck(Message& request, PMONDefinitionId PMONId, ParameterId monitoredParameterId, DeltaThreshold lowDeltaThreshold, DeltaThreshold highDeltaThreshold) {
	appendCommonFields(request, PMONId, monitoredParameterId, PMON::CheckType::Delta);
	request.append<DeltaThreshold>(lowDeltaThreshold);
	request.append<EventDefinitionId>(NewBelowLowThresholdEvent);
	request.append<DeltaThreshold>(highDeltaThreshold);
	request.append<EventDefinitionId>(NewAboveHighThresholdEvent);
	request.append<NumberOfConsecutiveDeltaChecks>(NewNumberOfConsecutiveDeltaChecks);
}

TEST_CASE("Add Parameter Monitoring Definitions") {
	SECTION("Valid Request to add Parameter Monitoring Definitions") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		uint16_t numberOfIds = 1;
		request.appendUint16(numberOfIds);
		PMONDefinitionId PMONId = 0;
		ParameterId monitoredParameterId = 0;
		PMONRepetitionNumber repetitionNumber = 5;
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
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendExpectedValueCheck(request, NewPMONId, ExistingParameterId);
		appendDeltaCheck(request, SecondNewPMONId, NonExistingParameterId, NewLowDeltaThreshold, NewHighDeltaThreshold);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ParameterMonitoringListIsFull) == 2);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameter Monitoring Definition already exists") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendExpectedValueCheck(request, NewPMONId, ExistingParameterId);
		appendExpectedValueCheck(request, NewPMONId, ExistingParameterId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Add Parameter Monitoring Definition with a non-existing parameter") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(1);
		appendExpectedValueCheck(request, NewPMONId, NonExistingParameterId);

		MessageParser::execute(request);
		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High limit is lower than low limit") {
		uint16_t numberOfIds = 1;
		PMONDefinitionId PMONId = 0;
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
		request.append<PMONDefinitionId>(PMONId);
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
		PMONDefinitionId PMONId = 0;
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
		request.append<PMONDefinitionId>(PMONId);
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

	SECTION("Valid Instructions After Rejected Instructions") {
		const PMONDefinitionId PMONIdWithNonExistingParameter = 10;
		const PMONDefinitionId PMONIdWithWrongLimits = 11;
		const PMONDefinitionId addedDeltaCheckId = 12;
		const PMONDefinitionId addedExpectedValueCheckId = 13;

		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(5);
		appendLimitCheck(request, PMONIdWithNonExistingParameter, NonExistingParameterId, NewLowLimit, NewHighLimit);
		appendLimitCheck(request, PMONIdWithWrongLimits, ExistingParameterId, NewHighLimit, NewLowLimit);
		appendDeltaCheck(request, addedDeltaCheckId, ExistingParameterId, NewLowDeltaThreshold, NewHighDeltaThreshold);
		appendExpectedValueCheck(request, addedDeltaCheckId, ExistingParameterId);
		appendExpectedValueCheck(request, addedExpectedValueCheckId, ExistingParameterId);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameterMonitoringDefinition) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::AddAlreadyExistingParameter) == 1);
		CHECK(onBoardMonitoringService.getCount(PMONIdWithNonExistingParameter) == 0);
		CHECK(onBoardMonitoringService.getCount(PMONIdWithWrongLimits) == 0);
		REQUIRE(onBoardMonitoringService.getCount(addedDeltaCheckId) == 1);
		REQUIRE(onBoardMonitoringService.getCount(addedExpectedValueCheckId) == 1);

		auto& deltaCheck = dynamic_cast<PMONDeltaCheck&>(onBoardMonitoringService.getPMONDefinition(addedDeltaCheckId).get());
		CHECK(deltaCheck.checkType == PMON::CheckType::Delta);
		CHECK(deltaCheck.getLowDeltaThreshold() == NewLowDeltaThreshold);
		CHECK(deltaCheck.getHighDeltaThreshold() == NewHighDeltaThreshold);

		auto& expectedValueCheck = dynamic_cast<PMONExpectedValueCheck&>(onBoardMonitoringService.getPMONDefinition(addedExpectedValueCheckId).get());
		CHECK(expectedValueCheck.checkType == PMON::CheckType::ExpectedValue);
		CHECK(expectedValueCheck.getMask() == NewMask);
		CHECK(expectedValueCheck.getExpectedValue() == NewExpectedValue);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Unknown Check Type Stops The Request") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendCommonFields(request, NewPMONId, NonExistingParameterId, UnknownCheckType);
		appendLimitCheck(request, SecondNewPMONId, ExistingParameterId, NewLowLimit, NewHighLimit);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::PMONCheckTypeInvalid) == 1);
		CHECK(onBoardMonitoringService.getCount(NewPMONId) == 0);
		CHECK(onBoardMonitoringService.getCount(SecondNewPMONId) == 0);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request That Declares More Instructions Than It Contains") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(100);
		appendLimitCheck(request, NewPMONId, ExistingParameterId, NewLowLimit, NewHighLimit);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MessageTooShort) == 1);
		CHECK(onBoardMonitoringService.getCount(NewPMONId) == 1);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request That Ends Inside An Instruction") {
		Message request =
		    Message(OnBoardMonitoringService::ServiceType,
		            OnBoardMonitoringService::MessageType::AddParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendLimitCheck(request, NewPMONId, ExistingParameterId, NewLowLimit, NewHighLimit);
		appendCommonFields(request, SecondNewPMONId, ExistingParameterId, PMON::CheckType::ExpectedValue);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MessageTooShort) == 1);
		CHECK(onBoardMonitoringService.getCount(NewPMONId) == 1);
		CHECK(onBoardMonitoringService.getCount(SecondNewPMONId) == 0);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Delete Parameter Monitoring Definitions") {
	SECTION("Valid request to Delete Parameter Monitoring Definitions") {
		initialiseParameterMonitoringDefinitions();
		const uint16_t numberOfIds = 2;
		etl::array<PMONDefinitionId, numberOfIds> PMONIds = {2, 3};

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
		PMONDefinitionId PMONId = 0;

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
		etl::array<PMONDefinitionId, numberOfIds> PMONIds = {0, 1};

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
		PMONDefinitionId PMONId = 1;
		ParameterId monitoredParameterId = 6;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<PMONDefinitionId>(PMONId);
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
		PMONDefinitionId PMONIdNotInList = 100;
		ParameterId monitoredParameterId = 7;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<PMONDefinitionId>(PMONIdNotInList);
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
		PMONDefinitionId existingPMONId = 1;
		ParameterId wrongMonitoredParameterId = 999;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 4;
		PMONLimit highLimit = 10;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<PMONDefinitionId>(existingPMONId);
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
		PMONDefinitionId PMONId = 1;
		ParameterId monitoredParameterId = 6;
		PMONRepetitionNumber repetitionNumber = 5;
		PMONLimit lowLimit = 10;
		PMONLimit highLimit = 4;
		EventDefinitionId belowLowLimitEvent = 9;
		EventDefinitionId aboveHighLimitEvent = 11;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(numberOfIds);
		request.append<PMONDefinitionId>(PMONId);
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

	SECTION("Check Type Different From The One In The Definition") {
		initialiseParameterMonitoringDefinitions();
		auto& definition = dynamic_cast<PMONExpectedValueCheck&>(onBoardMonitoringService.getPMONDefinition(ExpectedValueCheckId).get());
		const PMONExpectedValueCheck definitionBefore = definition;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(1);
		appendLimitCheck(request, ExpectedValueCheckId, ExpectedValueCheckMonitoredParameterId, NewLowLimit, NewHighLimit);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DifferentParameterMonitoringDefinitionAndCheckType) == 1);
		CHECK(definition.repetitionNumber == definitionBefore.repetitionNumber);
		CHECK(definition.expectedValue == definitionBefore.expectedValue);
		CHECK(definition.mask == definitionBefore.mask);
		CHECK(definition.unexpectedValueEvent == definitionBefore.unexpectedValueEvent);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("High Threshold Is Lower Than Low Threshold") {
		initialiseParameterMonitoringDefinitions();
		uint16_t numberOfIds = 1;
		PMONDefinitionId PMONId = DeltaCheckId;
		ParameterId monitoredParameterId = DeltaCheckMonitoredParameterId;
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
		request.append<PMONDefinitionId>(PMONId);
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

	SECTION("Valid Instruction After Rejected Instructions") {
		initialiseParameterMonitoringDefinitions();
		PMONDefinitionId PMONIdNotInList = 100;
		ParameterId wrongMonitoredParameterId = 999;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(7);
		appendExpectedValueCheck(request, PMONIdNotInList, ExpectedValueCheckMonitoredParameterId);
		appendLimitCheck(request, PMONIdNotInList, LimitCheckMonitoredParameterId, NewLowLimit, NewHighLimit);
		appendDeltaCheck(request, PMONIdNotInList, DeltaCheckMonitoredParameterId, NewLowDeltaThreshold, NewHighDeltaThreshold);
		appendLimitCheck(request, LimitCheckId, wrongMonitoredParameterId, NewLowLimit, NewHighLimit);
		appendLimitCheck(request, ExpectedValueCheckId, ExpectedValueCheckMonitoredParameterId, NewLowLimit, NewHighLimit);
		appendLimitCheck(request, LimitCheckId, LimitCheckMonitoredParameterId, NewHighLimit, NewLowLimit);
		appendDeltaCheck(request, DeltaCheckId, DeltaCheckMonitoredParameterId, NewLowDeltaThreshold, NewHighDeltaThreshold);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 6);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ModifyParameterNotInTheParameterMonitoringList) == 3);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DifferentParameterMonitoringDefinitionAndMonitoredParameter) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::DifferentParameterMonitoringDefinitionAndCheckType) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);

		auto& baseDefinition = onBoardMonitoringService.getPMONDefinition(DeltaCheckId).get();
		auto& modifiedDefinition = dynamic_cast<PMONDeltaCheck&>(baseDefinition);

		CHECK(modifiedDefinition.repetitionNumber == NewRepetitionNumber);
		CHECK(modifiedDefinition.lowDeltaThreshold == NewLowDeltaThreshold);
		CHECK(modifiedDefinition.highDeltaThreshold == NewHighDeltaThreshold);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Rejected Instruction Leaves The Definition Unchanged") {
		initialiseParameterMonitoringDefinitions();
		auto& limitCheck = dynamic_cast<PMONLimitCheck&>(onBoardMonitoringService.getPMONDefinition(LimitCheckId).get());
		auto& deltaCheck = dynamic_cast<PMONDeltaCheck&>(onBoardMonitoringService.getPMONDefinition(DeltaCheckId).get());
		limitCheck.repetitionCounter = 3;
		limitCheck.checkingStatus = PMON::WithinLimits;
		deltaCheck.repetitionCounter = 3;
		deltaCheck.checkingStatus = PMON::WithinThreshold;
		const PMONLimitCheck limitCheckBefore = limitCheck;
		const PMONDeltaCheck deltaCheckBefore = deltaCheck;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendLimitCheck(request, LimitCheckId, LimitCheckMonitoredParameterId, NewHighLimit, NewLowLimit);
		appendDeltaCheck(request, DeltaCheckId, DeltaCheckMonitoredParameterId, NewHighDeltaThreshold, NewLowDeltaThreshold);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighLimitIsLowerThanLowLimit) == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::HighThresholdIsLowerThanLowThreshold) == 1);
		CHECK(limitCheck.repetitionNumber == limitCheckBefore.repetitionNumber);
		CHECK(limitCheck.repetitionCounter == limitCheckBefore.repetitionCounter);
		CHECK(limitCheck.checkingStatus == limitCheckBefore.checkingStatus);
		CHECK(deltaCheck.repetitionNumber == deltaCheckBefore.repetitionNumber);
		CHECK(deltaCheck.repetitionCounter == deltaCheckBefore.repetitionCounter);
		CHECK(deltaCheck.checkingStatus == deltaCheckBefore.checkingStatus);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Unknown Check Type Stops The Request") {
		initialiseParameterMonitoringDefinitions();
		auto& definition = dynamic_cast<PMONLimitCheck&>(onBoardMonitoringService.getPMONDefinition(LimitCheckId).get());
		const PMONLimitCheck definitionBefore = definition;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(2);
		appendCommonFields(request, LimitCheckId, LimitCheckMonitoredParameterId, UnknownCheckType);
		appendLimitCheck(request, LimitCheckId, LimitCheckMonitoredParameterId, NewLowLimit, NewHighLimit);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::PMONCheckTypeInvalid) == 1);
		CHECK(definition.repetitionNumber == definitionBefore.repetitionNumber);
		CHECK(definition.lowLimit == definitionBefore.lowLimit);
		CHECK(definition.highLimit == definitionBefore.highLimit);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request That Declares More Instructions Than It Contains") {
		initialiseParameterMonitoringDefinitions();

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(100);
		appendLimitCheck(request, LimitCheckId, LimitCheckMonitoredParameterId, NewLowLimit, NewHighLimit);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MessageTooShort) == 1);
		CHECK(onBoardMonitoringService.getPMONDefinition(LimitCheckId).get().repetitionNumber == NewRepetitionNumber);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Request That Ends Inside An Instruction") {
		initialiseParameterMonitoringDefinitions();
		auto& definition = dynamic_cast<PMONExpectedValueCheck&>(onBoardMonitoringService.getPMONDefinition(ExpectedValueCheckId).get());
		const PMONExpectedValueCheck definitionBefore = definition;

		Message request = Message(OnBoardMonitoringService::ServiceType,
		                          OnBoardMonitoringService::MessageType::ModifyParameterMonitoringDefinitions, Message::TC, 0);
		request.appendUint16(1);
		appendCommonFields(request, ExpectedValueCheckId, ExpectedValueCheckMonitoredParameterId, PMON::CheckType::ExpectedValue);

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 1);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::MessageTooShort) == 1);
		CHECK(definition.repetitionNumber == definitionBefore.repetitionNumber);
		CHECK(definition.expectedValue == definitionBefore.expectedValue);
		CHECK(definition.mask == definitionBefore.mask);

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
		etl::array<PMONDefinitionId, 4> PMONIds = {0, 1, 2, 3};
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
		PMONDefinitionId PMONId = 5;
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
		etl::array<PMONDefinitionId, 2> PMONIds = {0, 5};
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition2;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition2;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition2;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition2;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition2;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition1;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition3;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition3;
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
		auto& pmon = Fixtures::getInstance().monitoringDefinition3;
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

