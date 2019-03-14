#include <iostream>
#include <cxxabi.h>
#include <ErrorHandler.hpp>
#include "Services/RequestVerificationService.hpp"


// TODO: Use service singleton, as soon as singletons are ready
static RequestVerificationService requestVerificationService;

template<>
void ErrorHandler::reportError(const Message &message, AcceptanceErrorType errorCode) {
	requestVerificationService.failAcceptanceVerification(message, errorCode);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, ExecutionStartErrorType errorCode) {
	requestVerificationService.failStartExecutionVerification(message, errorCode);

	logError(message, errorCode);
}

void ErrorHandler::reportProgressError(const Message &message, ExecutionProgressErrorType
errorCode, uint8_t stepID) {
	requestVerificationService.failProgressExecutionVerification(message, errorCode, stepID);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, ExecutionCompletionErrorType errorCode) {
	requestVerificationService.failCompletionExecutionVerification(message, errorCode);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, RoutingErrorType errorCode) {
	requestVerificationService.failRoutingVerification(message, errorCode);

	logError(message, errorCode);
}

void ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType errorCode) {
	logError(errorCode);
}
