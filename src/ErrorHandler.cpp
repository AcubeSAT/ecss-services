#include "ECSS_Configuration.hpp"
#include <ErrorHandler.hpp>
#include <ServicePool.hpp>
#include <etl/String.hpp>
#include "Services/RequestVerificationService.hpp"

template <>
void ErrorHandler::reportError(const Message& message, AcceptanceErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failAcceptanceVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, ExecutionStartErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failStartExecutionVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

void ErrorHandler::reportProgressError(const Message& message, ExecutionProgressErrorType errorCode, StepId stepID) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failProgressExecutionVerification(message, errorCode, stepID);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, ExecutionCompletionErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failCompletionExecutionVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

template <>
void ErrorHandler::reportError(const Message& message, RoutingErrorType errorCode) {
#ifdef SERVICE_REQUESTVERIFICATION
	Services.requestVerification.failRoutingVerification(message, errorCode);
#endif

	logError(message, errorCode);
}

void ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType errorCode) {
	logError(errorCode);
}

void ErrorHandler::reportAssertionFailure([[maybe_unused]] const char* location) {
#ifdef SERVICE_EVENTREPORT
	static bool inProgress = false;
	if (inProgress) {
		return;
	}
	inProgress = true;

	const char* suffix = (location == nullptr) ? "" : location;
	for (const char* cursor = suffix; *cursor != '\0'; ++cursor) {
		if ((*cursor == '/') || (*cursor == '\\')) {
			suffix = cursor + 1;
		}
	}

	Services.eventReport.highSeverityAnomalyReport(EventReportService::AssertionFail, String<ECSSEventDataAuxiliaryMaxSize>(suffix));
	inProgress = false;
#endif
}
