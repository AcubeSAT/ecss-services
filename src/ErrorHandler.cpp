#include "ECSS_Configuration.hpp"
#include <ErrorHandler.hpp>
#include <ServicePool.hpp>
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

void ErrorHandler::reportProgressError(const Message& message, ExecutionProgressErrorType errorCode, uint8_t stepID) {
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
