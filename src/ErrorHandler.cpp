#include <iostream>
#include <cxxabi.h>
#include <ErrorHandler.hpp>

#include "Services/RequestVerificationService.hpp"

// TODO: Use service singleton, as soon as singletons are ready
static RequestVerificationService requestVerificationService;

template<>
void ErrorHandler::reportError(const Message &message, AcceptanceErrorType errorCode) {
	requestVerificationService.failAcceptanceVerification(message);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, ExecutionErrorType errorCode) {
	requestVerificationService.failExecutionVerification(message);

	logError(message, errorCode);
}

template<>
void ErrorHandler::reportError(const Message &message, RoutingErrorType errorCode) {
	requestVerificationService.failRoutingVerification(message);

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
