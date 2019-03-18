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
	logError(errorCode);
}

template<typename ErrorType>
void ErrorHandler::logError(const Message &message, ErrorType errorType) {
	std::cerr
		/*
		 * Gets the error class name from the template
		 * Note: This is g++-dependent code and should only be used for debugging.
		 */
		<< abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr)
		<< " Error " << "[" << static_cast<uint16_t>(message.serviceType) << "," <<
		static_cast<uint16_t>(message.messageType) << "]: " << errorType << std::endl;
}

template<typename ErrorType>
void ErrorHandler::logError(ErrorType errorType) {
	std::cerr
		/*
		 * Gets the error class name from the template
		 * Note: This is g++-dependent code and should only be used for debugging.
		 */
		<< abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr)
		<< " Error: " << errorType << std::endl;
}
