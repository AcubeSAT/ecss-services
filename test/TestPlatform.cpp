#define CATCH_CONFIG_EXTERNAL_INTERFACES

#include <Logger.hpp>
#include <Message.hpp>
#include <Service.hpp>
#include <catch2/catch_all.hpp>
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

Time::CustomCUC_t TimeGetter::getCurrentTimeCustomCUC() {
	UTCTimestamp timeUTC = getCurrentTimeUTC();
	TimeStamp<Time::CUCSecondsBytes, Time::CUCFractionalBytes> timeCUC(timeUTC);
	Time::CustomCUC_t CUCtime = timeCUC.asCustomCUCTimestamp();
	return CUCtime;
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
}

void Logger::log(Logger::LogLevel level, etl::istring& message) {
	// Logs while testing are completely ignored
}

struct ServiceTestsListener : Catch::EventListenerBase {
	using EventListenerBase::EventListenerBase; // inherit constructor

	void sectionEnded(Catch::SectionStats const& sectionStats) override {
		// Make sure we don't have any errors
		if (not ServiceTests::isExpectingErrors()) {
			// An Error was thrown with this Message. If you expected this to happen, please call a
			// corresponding assertion function from ServiceTests to silence this message.

			CHECK(ServiceTests::hasNoErrors());
		}
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
} // namespace PlatformParameters

/**
 * Specific definition for \ref ParameterService's initialize function, for testing purposes.
 */
void ParameterService::initializeParameterMap() {
	parameters = {{static_cast<uint16_t>(0), PlatformParameters::parameter1},
	              {static_cast<uint16_t>(1), PlatformParameters::parameter2},
	              {static_cast<uint16_t>(2), PlatformParameters::parameter3},
	              {static_cast<uint16_t>(3), PlatformParameters::parameter4},
	              {static_cast<uint16_t>(4), PlatformParameters::parameter5},
	              {static_cast<uint16_t>(5), PlatformParameters::parameter6},
	              {static_cast<uint16_t>(6), PlatformParameters::parameter7},
	              {static_cast<uint16_t>(7), PlatformParameters::parameter8},
	              {static_cast<uint16_t>(8), PlatformParameters::parameter9},
	              {static_cast<uint16_t>(9), PlatformParameters::parameter10},
	              {static_cast<uint16_t>(10), PlatformParameters::parameter11},
	              {static_cast<uint16_t>(11), PlatformParameters::parameter12}};
}

void TimeBasedSchedulingService::taskNotifier() {}

void ParameterStatisticsService::initializeStatisticsMap() {
	statisticsMap = {};
}

CATCH_REGISTER_LISTENER(ServiceTestsListener)
