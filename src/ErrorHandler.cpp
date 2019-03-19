#include <iostream>
#include <cxxabi.h>
#include <ErrorHandler.hpp>
#include <ServicePool.hpp>
#include "Services/RequestVerificationService.hpp"


template<>
void ErrorHandler::reportError(const Message &message, AcceptanceErrorType errorCode) {
	Services.requestVerification.failAcceptanceVerification(message, errorCode);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, ExecutionStartErrorType errorCode) {
	Services.requestVerification.failStartExecutionVerification(message, errorCode);

	logError(message, errorCode);
}

void ErrorHandler::reportProgressError(const Message &message, ExecutionProgressErrorType
errorCode, uint8_t stepID) {
	Services.requestVerification.failProgressExecutionVerification(message, errorCode, stepID);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, ExecutionCompletionErrorType errorCode) {
	Services.requestVerification.failCompletionExecutionVerification(message, errorCode);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, RoutingErrorType errorCode) {
	Services.requestVerification.failRoutingVerification(message, errorCode);

	logError(message, errorCode);
}

void ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType errorCode) {
	logError(UnknownInternalError);
}
