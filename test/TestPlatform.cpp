#define CATCH_CONFIG_EXTERNAL_INTERFACES


#include <Logger.hpp>
#include <Message.hpp>
#include <Service.hpp>
#include <catch2/catch_all.hpp>
#include <cxxabi.h>
#include "Helpers/Parameter.hpp"
#include "Helpers/TimeGetter.hpp"
#include "Parameters/PlatformParameters.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/ServiceTests.hpp"

UTCTimestamp TimeGetter::getCurrentTimeUTC() {
	UTCTimestamp currentTime(2020, 4, 10, 10, 15, 0);
	return currentTime;
}

Time::DefaultCUC TimeGetter::getCurrentTimeDefaultCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	Time::DefaultCUC timeCUC(timeUTC);
	return timeCUC;
}

// Explicit template specializations for the logError() function
template void ErrorHandler::logError(const Message&, ErrorHandler::AcceptanceErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionStartErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionProgressErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::ExecutionCompletionErrorType);
template void ErrorHandler::logError(const Message&, ErrorHandler::RoutingErrorType);
template void ErrorHandler::logError(ErrorHandler::InternalErrorType);

// Initialisation of ServiceTests properties
std::vector<Message> ServiceTests::queuedMessages = std::vector<Message>();
std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool> ServiceTests::thrownErrors =
    std::multimap<std::pair<ErrorHandler::ErrorSource, uint16_t>, bool>();
bool ServiceTests::expectingErrors = false;

void Service::storeMessage(Message& message) {
	// Just add the message to the queue
	ServiceTests::queue(message);
}

template <typename ErrorType>
void ErrorHandler::logError(const Message& message, ErrorType errorType) {
	logError(errorType);
}

template <typename ErrorType>
void ErrorHandler::logError(ErrorType errorType) {
	ServiceTests::addError(ErrorHandler::findErrorSource(errorType), errorType);

	auto errorCategory = abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr);
	auto errorNumber = std::underlying_type_t<ErrorType>(errorType);

	LOG_ERROR << "Error " << errorCategory << " with number " << errorNumber;
}

void Logger::log(Logger::LogLevel level, etl::istring& message) {
	// Logs while testing are passed on to Catch2, if they are important enough
	if (level >= Logger::warning) {
		UNSCOPED_INFO(message.c_str());
	}
}

struct ServiceTestsListener : Catch::EventListenerBase {
	using EventListenerBase::EventListenerBase; // inherit constructor

	void sectionEnded(Catch::SectionStats const& sectionStats) override {
		// Make sure we don't have any errors
		if (not ServiceTests::isExpectingErrors()) {
			// An Error was thrown with this Message. If you expected this to happen, please call a
			// corresponding assertion function from ServiceTests to silence this message.
			UNSCOPED_INFO("Found " << ServiceTests::countErrors() << " errors at end of section: ");
			for (auto error: ServiceTests::getThrownErrors()) {
				UNSCOPED_INFO("  Error " << error.second << " (type " << error.first << ")");
			}
			CHECK(ServiceTests::hasNoErrors());
		}

		ServiceTests::resetErrors();
	}

	void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override {
		// Tear-down after a test case is run
		ServiceTests::reset();
	}
};

/**
 * Extension of x86's specific \ref PlatformParameters namespace.
 * The parameters declared below are only used for testing purposes.
 */
namespace PlatformParameters {
	inline Parameter<uint32_t> parameter4(5);
	inline Parameter<uint8_t> parameter5(11);
	inline Parameter<uint32_t> parameter6(23);
	inline Parameter<uint32_t> parameter7(53);
	inline Parameter<uint8_t> parameter8(55);
	inline Parameter<uint16_t> parameter9(32);
	inline Parameter<uint32_t> parameter10(43);
	inline Parameter<uint32_t> parameter11(91);
	inline Parameter<uint8_t> parameter12(1);
	inline Parameter<uint8_t> parameter13(1);
	inline Parameter<uint8_t> parameter14(1);
	inline Parameter<uint8_t> parameter15(1);
	inline Parameter<uint8_t> parameter16(1);
	inline Parameter<uint8_t> parameter17(1);
	inline Parameter<uint8_t> parameter18(1);
	inline Parameter<uint8_t> parameter19(1);
	inline Parameter<uint8_t> parameter20(1);
	inline Parameter<uint8_t> parameter21(1);
	inline Parameter<uint8_t> parameter22(1);
	inline Parameter<uint8_t> parameter23(1);
	inline Parameter<uint8_t> parameter24(1);
	inline Parameter<uint8_t> parameter25(1);
	inline Parameter<uint8_t> parameter26(1);
	inline Parameter<uint8_t> parameter27(1);
	inline Parameter<uint8_t> parameter28(1);
	inline Parameter<uint8_t> parameter29(1);
	inline Parameter<uint8_t> parameter30(1);
	inline Parameter<uint8_t> parameter31(1);
	inline Parameter<uint8_t> parameter32(1);
	inline Parameter<uint8_t> parameter33(1);
	inline Parameter<uint8_t> parameter34(1);

} // namespace PlatformParameters

/**
 * Specific definition for \ref ParameterService's initialize function, for testing purposes.
 */
void ParameterService::initializeParameterMap() {
	parameters = {
	    {uint16_t{0}, PlatformParameters::parameter1},
	    {uint16_t{1}, PlatformParameters::parameter2},
	    {uint16_t{2}, PlatformParameters::parameter3},
	    {uint16_t{3}, PlatformParameters::parameter4},
	    {uint16_t{4}, PlatformParameters::parameter5},
	    {uint16_t{5}, PlatformParameters::parameter6},
	    {uint16_t{6}, PlatformParameters::parameter7},
	    {uint16_t{7}, PlatformParameters::parameter8},
	    {uint16_t{8}, PlatformParameters::parameter9},
	    {uint16_t{9}, PlatformParameters::parameter10},
	    {uint16_t{10}, PlatformParameters::parameter11},
	    {uint16_t{11}, PlatformParameters::parameter12},
	    {uint16_t{12}, PlatformParameters::parameter13},
	    {uint16_t{13}, PlatformParameters::parameter14},
	    {uint16_t{14}, PlatformParameters::parameter15},
	    {uint16_t{15}, PlatformParameters::parameter16},
	    {uint16_t{16}, PlatformParameters::parameter17},
	    {uint16_t{17}, PlatformParameters::parameter18},
	    {uint16_t{18}, PlatformParameters::parameter19},
	    {uint16_t{19}, PlatformParameters::parameter20},
	    {uint16_t{20}, PlatformParameters::parameter21},
	    {uint16_t{21}, PlatformParameters::parameter22},
	    {uint16_t{22}, PlatformParameters::parameter23},
	    {uint16_t{23}, PlatformParameters::parameter24},
	    {uint16_t{24}, PlatformParameters::parameter25},
	    {uint16_t{25}, PlatformParameters::parameter26},
	    {uint16_t{26}, PlatformParameters::parameter27},
	    {uint16_t{27}, PlatformParameters::parameter28},
	    {uint16_t{28}, PlatformParameters::parameter29},
	    {uint16_t{29}, PlatformParameters::parameter30},
	    {uint16_t{30}, PlatformParameters::parameter31},
	    {uint16_t{31}, PlatformParameters::parameter32},
	    {uint16_t{32}, PlatformParameters::parameter33},
	    {uint16_t{33}, PlatformParameters::parameter34}};
}

void TimeBasedSchedulingService::notifyNewActivityAddition() {}

void ParameterStatisticsService::initializeStatisticsMap() {
	statisticsMap = {};
}

CATCH_REGISTER_LISTENER(ServiceTestsListener)
